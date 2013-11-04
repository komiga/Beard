
#include <Beard/utility.hpp>
#include <Beard/detail/gr_ceformat.hpp>
#include <Beard/detail/debug.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Caps.hpp>
#include <Beard/tty/Terminal.hpp>

#include <duct/traits.hpp>
#include <duct/char.hpp>
#include <duct/EncodingUtils.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#include <cerrno>
#include <cstring>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <iterator>
#include <string>

namespace Beard {
namespace tty {

#define BEARD_SCOPE_CLASS tty::Terminal

static_assert(
	std::is_trivially_copyable<tty::Cell>::value,
	"tty::Cell must be trivially copyable"
);

struct Terminal::terminal_private final {
	struct ::termios tios_orig;
	struct ::termios tios;
};

namespace {
enum : unsigned {
	attr_mask = 0xFF00,
	attr_mask_color = 0x00FF,
};

static tty::Terminal* s_sigwinch_terminal{nullptr};

static tty::CapString const
s_cap_cache_table[]{
	tty::CapString::clear_screen,

	tty::CapString::cursor_invisible,
	tty::CapString::cursor_visible,

	tty::CapString::enter_ca_mode,
	tty::CapString::exit_ca_mode,

	tty::CapString::exit_attribute_mode,
	tty::CapString::enter_blink_mode,
	tty::CapString::enter_bold_mode,
	tty::CapString::enter_reverse_mode,
	tty::CapString::enter_underline_mode,

	tty::CapString::keypad_local,
	tty::CapString::keypad_xmit,
};

static constexpr tty::Cell const
s_cell_default{
	{' '},
	tty::Color::term_default,
	tty::Color::term_default
};
} // anonymous namespace

struct terminal_internal final
	: duct::traits::restrict_all
{

#define BEARD_SCOPE_FUNC internal::close_tty
static void
close_tty(
	tty::fd_type const tty_fd
) noexcept {
	unsigned close_tries = 4;
	while (close_tries--) {
		if (0 == ::close(tty_fd)) {
			break;
		} else {
			auto const err = errno;
			BEARD_DEBUG_CERR_FQN(
				err,
				"failed to close terminal file descriptor"
			);
			if (EINTR == err || EIO == err) {
				// Only retry when we're either interrupted by a
				// signal or hit an I/O error
				continue;
			} else {
				// EBADF or something nonstandard
				break;
			}
		}
	}
}
#undef BEARD_SCOPE_FUNC

// SIGWINCH handling

static void
sigwinch_handler(int /*signum*/) {
	if (nullptr != s_sigwinch_terminal) {
		s_sigwinch_terminal->update_size();
	}
}

// NB: Pursuant to SIGACTION(2), sa_handler and sa_sigaction
// may be part of a union
#define BEARD_TERMINAL_SIG_HANDLER_IS_NULLARY_(sa_)			\
	(  (SIG_DFL == (sa_).sa_handler							\
	 || SIG_IGN == (sa_).sa_handler							\
	)&&(static_cast<void*>(&(sa_).sa_sigaction)				\
	 == static_cast<void*>(&(sa_).sa_handler)				\
     || nullptr == (sa_).sa_sigaction						\
	))
//

#define BEARD_SCOPE_FUNC internal::setup_sigwinch_handler
static void
setup_sigwinch_handler(
	tty::Terminal& terminal
) {
	// Another terminal already owns the handler
	if (nullptr != s_sigwinch_terminal) {
		BEARD_THROW_FQN(
			ErrorCode::tty_sigwinch_handler_already_active,
			"another terminal already owns the SIGWINCH handler"
		);
	}

	// Check if someone else owns the handler
	struct ::sigaction sig_current;
	if (0 != ::sigaction(SIGWINCH, nullptr, &sig_current)) {
		BEARD_THROW_CERR(
			ErrorCode::tty_sigaction_failed,
			errno,
			"sigaction() fetch failed"
		);
	}

	if (!BEARD_TERMINAL_SIG_HANDLER_IS_NULLARY_(sig_current)) {
		BEARD_THROW_FQN(
			ErrorCode::tty_sigwinch_handler_already_active,
			"someone else already owns the SIGWINCH handler"
		);
	}

	struct ::sigaction sig;
	sig.sa_handler = sigwinch_handler;
	sig.sa_flags = 0;
	if (0 != ::sigaction(SIGWINCH, &sig, nullptr)) {
		BEARD_THROW_CERR(
			ErrorCode::tty_sigaction_failed,
			errno,
			"sigaction() replacement failed"
		);
	}

	s_sigwinch_terminal = &terminal;
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC internal::release_sigwinch_handler
static void
release_sigwinch_handler(
	tty::Terminal& terminal
) noexcept {
	if (&terminal != s_sigwinch_terminal) {
		return;
	}

	// Neither of these calls should normally fail, but if they do
	// we'll assume for whatever reason that the signal handler is
	// no longer active.

	// Release only if we actually own the handler (if someone else
	// stole it, let them keep it)
	struct ::sigaction sig_current;
	if (0 == ::sigaction(SIGWINCH, nullptr, &sig_current)) {
		if (sigwinch_handler == sig_current.sa_handler) {
			struct ::sigaction sig;
			sig.sa_handler = SIG_IGN;
			sig.sa_flags = 0;
			if (0 != ::sigaction(SIGWINCH, &sig, nullptr)) {
				BEARD_DEBUG_CERR_FQN(
					errno,
					"failed to restore the default SIGWINCH handler"
				);
			}
		} else {
			BEARD_DEBUG_MSG_FQN(
				"some grubby fellow stole the SIGWINCH"
				" handler from us"
			);
		}
	} else {
		BEARD_DEBUG_CERR_FQN(
			errno,
			"failed to fetch the current SIGWINCH handler"
		);
	}

	s_sigwinch_terminal = nullptr;
}
#undef BEARD_SCOPE_FUNC

// cell buffer

static void
clear_buffer(
	tty::Terminal::cell_vector_type& cell_buffer
) {
	// TODO: Optimize with cache buffer of constant size and std::copy()?
	std::fill(
		cell_buffer.begin(),
		cell_buffer.end(),
		s_cell_default
	);
}

static void
resize_buffer(
	unsigned const old_width,
	unsigned const old_height,
	tty::Terminal::cell_vector_type& oldb,
	unsigned const new_size,
	unsigned const new_width,
	unsigned const new_height,
	bool const retain
) {
	// TODO: Choose optimal ctor if new buffer is strictly smaller
	tty::Terminal::cell_vector_type newb{new_size, s_cell_default};
	if (retain && 0u < oldb.size() && 0u < newb.size()) {
		unsigned const
			min_width  = std::min(old_width , new_width),
			min_height = std::min(old_height, new_height)
		;
		auto it_old = oldb.cbegin();
		auto it_new = newb.begin();
		for (unsigned y = 0u; min_height > y; ++y) {
			std::copy(it_old, it_old + min_width, it_new);
			it_old += old_width;
			it_new += new_width;
		}
	}
	oldb.operator=(std::move(newb));
}

// rendering

#define BEARD_TERMINAL_WRITE_STRLIT(s_, sl_) \
	(s_).write((sl_), sizeof(sl_) - 1)

static void
repos_out(
	std::ostream& stream,
	unsigned const x,
	unsigned const y
) {
	BEARD_TERMINAL_WRITE_STRLIT(stream, "\033[");
	String str{std::to_string(y + 1u)};
	stream.write(str.data(), str.size());
	BEARD_TERMINAL_WRITE_STRLIT(stream, ";");

	str.assign(std::to_string(x + 1u));
	stream.write(str.data(), str.size());
	BEARD_TERMINAL_WRITE_STRLIT(stream, "H");
}

static void
write_caret(
	std::ostream& stream,
	unsigned const caret_x,
	unsigned const caret_y
) {
	repos_out(stream, caret_x, caret_y);
}

static void
write_colors(
	std::ostream& stream,
	unsigned const color_fg,
	unsigned const color_bg
) {
	enum : unsigned {
		color16_max = 0x09
	};

	unsigned const def = 
		(tty::Color::term_default != color_fg) |
		(tty::Color::term_default != color_bg) << 1u
	;
	if (!def) {
		return;
	} else if ((color16_max >= color_fg) && (color16_max >= color_bg)) {
		char cc = ('0' - 1);
		switch (def) {
		case 0x01: // FG
			BEARD_TERMINAL_WRITE_STRLIT(stream, "\033[3");
			cc += color_fg;
			stream.write(&cc, 1);
			BEARD_TERMINAL_WRITE_STRLIT(stream, "m");
			break;

		case 0x02: // BG
			BEARD_TERMINAL_WRITE_STRLIT(stream, "\033[4");
			cc += color_bg;
			stream.write(&cc, 1);
			BEARD_TERMINAL_WRITE_STRLIT(stream, "m");
			break;

		case 0x03: // FG && BG
			BEARD_TERMINAL_WRITE_STRLIT(stream, "\033[3");
			cc += color_fg;
			stream.write(&cc, 1);
			BEARD_TERMINAL_WRITE_STRLIT(stream, ";4");
			cc = ('0' - 1) + color_bg;
			stream.write(&cc, 1);
			BEARD_TERMINAL_WRITE_STRLIT(stream, "m");
			break;
		}
	} else {
		// TODO: xterm-style color extension support
	}
}

static void
write_attrs(
	Terminal& terminal,
	unsigned const attr_fg,
	unsigned const attr_bg,
	bool const force = false
) {
	if (force
	 || attr_fg != terminal.m_attr_fg_last
	 || attr_bg != terminal.m_attr_bg_last
 	) {
		terminal.put_cap_cache(Terminal::CapCache::exit_attribute_mode);
		write_colors(
			terminal.m_stream_out,
			attr_fg & attr_mask_color,
			attr_bg & attr_mask_color
		);
		if (attr_fg & tty::Attr::bold) {
			terminal.put_cap_cache(Terminal::CapCache::enter_bold_mode);
		}
		if (attr_fg & tty::Attr::underline) {
			terminal.put_cap_cache(Terminal::CapCache::enter_underline_mode);
		}
		if (attr_bg & tty::Attr::blink) {
			terminal.put_cap_cache(Terminal::CapCache::enter_blink_mode);
		}
		if ((attr_fg & tty::Attr::inverted)
		 || (attr_bg & tty::Attr::inverted)
	 	) {
			terminal.put_cap_cache(Terminal::CapCache::enter_reverse_mode);
		}
		terminal.m_attr_fg_last = attr_fg;
		terminal.m_attr_bg_last = attr_bg;
	}
}

// I/O

#define BEARD_SCOPE_FUNC internal::flush
static void
flush(
	tty::Terminal& terminal
) {
	std::size_t size = terminal.m_streambuf_out.get_sequence_size();
	ssize_t written = 0;
	unsigned tries = 2;
	do {
		written = ::write(
			terminal.m_tty_fd,
			terminal.m_streambuf_out.get_buffer().data(),
			size
		);
		if (-1 == written) {
			BEARD_DEBUG_CERR_FQN(
				errno,
				"write() failed or was interrupted"
				" (potentially retrying)"
			);
		} else {
			break;
		}
	} while (
		tries-- &&
		(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
	);
	if (-1 != written && written != static_cast<ssize_t>(size)) {
		BEARD_DEBUG_MSG_FQN(
			"failed to flush all bytes to terminal"
		);

		// Slide back the remaining data
		size = static_cast<std::size_t>(size - written);
		std::memmove(
			terminal.m_streambuf_out.get_buffer().data(),
			terminal.m_streambuf_out.get_buffer().data() + written,
			size
		);

		// Reset and inform the streambuf of the data
		terminal.m_streambuf_out.reset(0x800 < size ? size : 0x800);
		terminal.m_stream_out.seekp(size);
		terminal.m_stream_out.seekp(0u);
	}
}
#undef BEARD_SCOPE_FUNC

}; // struct terminal_internal


// class Terminal implementation

Terminal::~Terminal() noexcept {
	this->close();
}

Terminal::Terminal()
	: m_tty_priv(new Terminal::terminal_private())
{}

Terminal::Terminal(
	tty::TerminalInfo term_info
)
	: m_tty_priv(new Terminal::terminal_private())
	, m_info(std::move(term_info))
{}

Terminal::Terminal(Terminal&&) = default;
Terminal& Terminal::operator=(Terminal&&) = default;

void
Terminal::put_cap_cache(
	CapCache const cap
) {
	auto const& str = m_cap_cache[enum_cast(cap)];
	m_stream_out.write(str.data(), str.size());
}

void
Terminal::clear(
	bool const clear_frontbuffer
) {
	if (clear_frontbuffer) {
		terminal_internal::clear_buffer(m_cell_frontbuffer);
	}
	put_cap_cache(CapCache::clear_screen);
	terminal_internal::write_attrs(
		*this,
		tty::Color::term_default,
		tty::Color::term_default,
		true
	);
	if (m_caret_visible) {
		terminal_internal::write_caret(
			m_stream_out, m_caret_x, m_caret_y
		);
	}
	terminal_internal::flush(*this);
}

bool
Terminal::resize(
	unsigned const new_width,
	unsigned const new_height
) {
	if (new_width == m_tty_width && new_height == m_tty_height) {
		return false;
	}

	std::size_t const size = new_width * new_height;
	terminal_internal::resize_buffer(
		m_tty_width, m_tty_height,
		m_cell_backbuffer, size, new_width, new_height, true
	);
	terminal_internal::resize_buffer(
		m_tty_width, m_tty_height,
		m_cell_frontbuffer, size, new_width, new_height, false
	);

	clear(false);
	m_dirty_rows.clear();
	m_dirty_rows.resize(new_height);
	m_dirty_rows.flip();

	if (!ev_pending.resize.pending) {
		ev_pending.resize.old_width  = m_tty_width;
		ev_pending.resize.old_height = m_tty_height;
		ev_pending.resize.pending = true;
	}
	m_tty_width  = new_width;
	m_tty_height = new_height;
	return true;
}

#define BEARD_SCOPE_FUNC init
void
Terminal::init() {
	if (0 != ::tcgetattr(m_tty_fd, &m_tty_priv->tios_orig)) {
		BEARD_THROW_CERR(
			ErrorCode::tty_init_failed,
			errno,
			"failed to fetch current termios"
		);
	}
	m_tty_priv->tios = m_tty_priv->tios_orig;

	// Input modes
	m_tty_priv->tios.c_iflag &=
	~(
		// Disable BREAK ignore and side-effects
		IGNBRK | BRKINT |
		// Ensure parity errors are read as NUL
		IGNPAR | PARMRK |
		// Disable 8th-bit stripping
		ISTRIP |
		// Disable all CR- and NL- affecting flags
		INLCR | IGNCR | ICRNL |
		// Disable XON/XOFF flow control
		IXON
	);

	// Output modes
	// Disable implementation-defined output processing
	m_tty_priv->tios.c_oflag &= ~(OPOST);

	// Control modes
	// Disable parity generation on input and checking on output;
	// set character size to 8
	m_tty_priv->tios.c_cflag &= ~(CSIZE | PARENB);
	m_tty_priv->tios.c_cflag |= CS8;

	// Local modes
	m_tty_priv->tios.c_lflag &=
	~(
		// Disable signal generation
		//ISIG |
		// Disable canonical mode
		ICANON |
		// Disable echoing
		ECHO | ECHONL |
		// Disable implementation-defined input processing
		IEXTEN
	);

	// Special characters
	// Minimum number of chars for noncanonical read
	m_tty_priv->tios.c_cc[VMIN] = 0;
	// Timeout for noncanonical read (deciseconds)
	m_tty_priv->tios.c_cc[VTIME] = 0;

	// TODO: tcsetattr can succeed despite not applying all modes;
	// should we compare with the new state to make sure the modes
	// were applied?
	if (0 != ::tcsetattr(m_tty_fd, TCSAFLUSH, &m_tty_priv->tios)) {
		// TODO: reset to tios_orig
		BEARD_THROW_CERR(
			ErrorCode::tty_init_failed,
			errno,
			"failed to set termios"
		);
	}

	update_size();

	put_cap_cache(CapCache::enter_ca_mode);
	put_cap_cache(CapCache::keypad_xmit);
	(m_caret_visible)
		? put_cap_cache(CapCache::cursor_visible)
		: put_cap_cache(CapCache::cursor_invisible)
	;
	terminal_internal::flush(*this);
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC deinit
void
Terminal::deinit() {
	set_caret_pos(0u, 0u);
	set_caret_visible(false);

	bool const resized = resize(0u, 0u);
	put_cap_cache(CapCache::exit_attribute_mode);
	if (!resized) {
		put_cap_cache(CapCache::clear_screen);
	}
	put_cap_cache(CapCache::cursor_visible);
	put_cap_cache(CapCache::keypad_local);
	put_cap_cache(CapCache::exit_ca_mode);
	terminal_internal::flush(*this);

	m_cap_max_colors = 8u;
	m_attr_fg_last = tty::Color::term_default;
	m_attr_bg_last = tty::Color::term_default;

	ev_pending.resize.pending = false;

	if (0 != ::tcsetattr(m_tty_fd, TCSAFLUSH, &m_tty_priv->tios_orig)) {
		BEARD_DEBUG_CERR_FQN(
			errno,
			"failed to reset original termios"
		);
	}
}
#undef BEARD_SCOPE_FUNC

// input control

void
Terminal::set_caret_pos(
	unsigned const x,
	unsigned const y
) {
	if (x != m_caret_x || y != m_caret_y) {
		m_caret_x = x;
		m_caret_y = y;
		if (m_caret_visible) {
			terminal_internal::write_caret(
				m_stream_out, m_caret_x, m_caret_y
			);
			terminal_internal::flush(*this);
		}
	}
}

void
Terminal::set_caret_visible(
	bool const visible
) {
	if (visible != m_caret_visible) {
		m_caret_visible = visible;
		if (m_caret_visible) {
			put_cap_cache(CapCache::cursor_visible);
		} else {
			put_cap_cache(CapCache::cursor_invisible);
		}
		terminal_internal::flush(*this);
	}
}

// rendering

// TODO: Change colors to term_default if they are larger than the
// terminal's maximum number of colors

#define BEARD_TERMINAL_CELL_CMP(x_, y_) \
	std::memcmp(&(x_), &(y_), sizeof(tty::Cell))

void
Terminal::put_cell(
	unsigned const x,
	unsigned const y,
	tty::Cell const& cell
) noexcept {
	if (x < m_tty_width && y < m_tty_height
	 && '\0' != cell.u8block.units[0u]
	) {
		auto it = m_cell_backbuffer.begin() + (y * m_tty_width) + x;
		if (BEARD_TERMINAL_CELL_CMP(*it, cell)) {
			std::copy(&cell, &cell + 1u, it);
			m_dirty_rows[y] = true;
		}
	}
}

void
Terminal::put_sequence(
	unsigned x,
	unsigned const y,
	tty::Sequence const& seq,
	uint16_t const attr_fg,
	uint16_t const attr_bg
) noexcept {
	if (x < m_tty_width && y < m_tty_height) {
		bool dirtied = false;
		std::size_t ss_size = 0u;
		tty::Cell cell{{}, attr_fg, attr_bg};
		auto it_put = m_cell_backbuffer.begin() + (y * m_tty_width) + x;
		auto it_seq = seq.data;
		auto const seq_end = seq.data + seq.size;
		for (
			; seq_end > it_seq && x < m_tty_width;
			++x,
			++it_put,
			it_seq += ss_size
		) {
			ss_size = tty::EncUtils::required_first_whole(
				*it_seq
			);
			if (seq_end < seq.data + ss_size) {
				// incomplete sequence
				BEARD_DEBUG_MSG_FQN(
					"encountered incomplete code unit sub-sequence"
					" in Sequence"
				);
				break;
			}
			cell.u8block.assign(
				it_seq,
				ss_size
			);

			// put_cell() inlined
			if (BEARD_TERMINAL_CELL_CMP(*it_put, cell)) {
				std::copy(&cell, &cell + 1u, it_put);
				dirtied = true;
			}
		}
		if (dirtied) {
			m_dirty_rows[y] = true;
		}
	}
}

void
Terminal::render() {
	m_stream_out.clear();
	//put_cap_cache(CapCache::exit_attribute_mode);

	// TODO: No-change optimization: add state flag to signify any
	// change in the backbuffer
	// TODO: Gah, this is such a mess; try reducing some of the
	// loop state
	// TODO: Debug output/error on !m_stream_out.good()
	auto it_dirty = m_dirty_rows.begin();
	cell_vector_type::iterator
		fit_row,
		fit_row_n,
		fit_col
	;
	cell_vector_type::const_iterator
		bit_row,
		bit_col
	;
	unsigned
		row = 0u,
		cseq_f = 0u,
		cseq_e = 0u
	;
	for (
		fit_row = m_cell_frontbuffer.begin(),
		fit_row_n = fit_row + m_tty_width,
		bit_row = m_cell_backbuffer.cbegin()
		; m_tty_height != row
		; fit_row = fit_row_n,
		// FIXME: Danger danger. stdlib should be OK with this?
		// (It's not being dereferenced.)
		fit_row_n += m_tty_width,
		bit_row += m_tty_width,
		++it_dirty,
		++row
	) {
		if (!*it_dirty) {
			continue;
		}
		for (
			fit_col = fit_row,
			bit_col = m_cell_backbuffer.cbegin()
					+ std::distance(m_cell_frontbuffer.begin(), fit_row),
			cseq_f = 0u,
			cseq_e = 0u
			; fit_col != fit_row_n
			; ++fit_col, ++bit_col, ++cseq_e
		) {
			if (BEARD_TERMINAL_CELL_CMP(*fit_col, *bit_col)) {
				// Cell differs from the front buffer cell;
				// reposition if necessary and render cell
				if (cseq_f == cseq_e) {
					// This cell is the head of the sequence;
					// reposition
					terminal_internal::repos_out(
						m_stream_out,
						std::distance(fit_row, fit_col),
						row
					);
				}
				terminal_internal::write_attrs(
					*this,
					bit_col->attr_fg,
					bit_col->attr_bg
				);
				m_stream_out.write(
					bit_col->u8block.units,
					bit_col->u8block.size()
				);
			} else if (cseq_f != cseq_e) {
				// Previous cell was last in differing sequence; copy
				// all cells in sequence to front buffer
				std::copy(bit_row + cseq_f, bit_col, fit_row + cseq_f);
				cseq_f = cseq_e + 1u;
			} else {
				// Previous cell did not differ; increment first iter
				// for copy sequence to track bit_col (ergo, prevent
				// above branch on next iteration)
				++cseq_f;
			}
		}
		if (cseq_f != cseq_e) {
			// Handle terminating copy sequence
			std::copy(bit_row + cseq_f, bit_col, fit_row + cseq_f);
		}
		(*it_dirty) = false;
	}

	if (m_caret_visible) {
		terminal_internal::write_caret(
			m_stream_out, m_caret_x, m_caret_y
		);
	}
	terminal_internal::flush(*this);
}

// events

tty::EventType
Terminal::poll(
	tty::Event& event
) {
	if (ev_pending.resize.pending) {
		event.resize.old_width  = ev_pending.resize.old_width;
		event.resize.old_height = ev_pending.resize.old_height;
		ev_pending.resize.pending = false;
		return tty::EventType::resize;
	}
	return tty::EventType::none;
}

// operations

// NB: If the terminal is uninitialized (i.e., closed), it
// will not own the SIGWINCH handler.
#define BEARD_TERMINAL_CHECK_OPEN_ERRORS_()							\
	if (is_open()) {												\
		BEARD_THROW_FQN(											\
			ErrorCode::tty_terminal_already_open,					\
			"terminal is already open"								\
		);															\
	}																\
	if (!m_info.is_initialized()) {									\
		BEARD_THROW_FQN(											\
			ErrorCode::tty_terminal_info_uninitialized,				\
			"terminal info is not initialized"						\
		);															\
	}
//

#define BEARD_SCOPE_FUNC open
namespace {
BEARD_DEF_FMT_FQN_CERR(
	s_err_open_bad_path,
	"failed to open terminal from device file `%s`"
);
} // anonymous namespace

void
Terminal::open(
	String const& tty_path,
	bool const use_sigwinch
) {
	BEARD_TERMINAL_CHECK_OPEN_ERRORS_();

	tty::fd_type const tty_fd = ::open(tty_path.c_str(), O_RDWR);
	if (tty::FD_INVALID == tty_fd) {
		BEARD_THROW_FMT_CERR(
			ErrorCode::tty_device_open_failed,
			s_err_open_bad_path,
			errno,
			tty_path
		);
	}

	try {
		this->open(tty_fd, use_sigwinch);
	} catch (...) {
		terminal_internal::close_tty(tty_fd);
		throw;
	}
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC open
void
Terminal::open(
	tty::fd_type const tty_fd,
	bool const use_sigwinch
) {
	BEARD_TERMINAL_CHECK_OPEN_ERRORS_();

	if (1 != ::isatty(tty_fd)) {
		switch (errno) {
		// Should be impossible (indicates no error)
		case 0:
			break;

		// POSIX.1-2001
		#ifdef ENOTTY
		case ENOTTY:
		#endif

		case EINVAL:
			BEARD_THROW_FQN(
				ErrorCode::tty_invalid_fd,
				"file descriptor does not refer to a terminal"
			);
			break;

		case EBADF:
		default:
			BEARD_THROW_FQN(
				ErrorCode::tty_invalid_fd,
				"file descriptor is invalid"
			);
			break;
		}
	}

	tty::TerminalInfo::cap_string_map_type::const_iterator iterator;
	for (
		unsigned idx = 0u;
		enum_cast(Terminal::CapCache::COUNT) > idx;
		++idx
	) {
		if (m_info.lookup_cap_string(s_cap_cache_table[idx], iterator)) {
			m_cap_cache[idx].assign(iterator->second);
		} else {
			m_cap_cache[idx].clear();
		}
	}

	// TODO: Should colors be disabled if the terminal says they're
	// not supported? (What if it's lying to us!?)
	// NB: Assuming terminal is capable of at least 8 colors
	auto const max_colors = m_info.get_cap_number(tty::CapNumber::max_colors);
	m_cap_max_colors =
		(tty::CAP_NUMBER_NOT_SUPPORTED == max_colors)
		? 8u
		: static_cast<unsigned>(max_colors)
	;

	if (use_sigwinch) {
		terminal_internal::setup_sigwinch_handler(*this);
	}

	try {
		m_tty_fd = tty_fd;
		init();
	} catch (...) {
		m_tty_fd = tty::FD_INVALID;
		throw;
	}
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC close
void
Terminal::close() noexcept {
	if (!is_open()) {
		return;
	}

	terminal_internal::release_sigwinch_handler(*this);

	deinit();

	terminal_internal::close_tty(m_tty_fd);
	m_tty_fd = tty::FD_INVALID;
}
#undef BEARD_SCOPE_FUNC

bool
Terminal::update_size() {
	if (is_open()) {
		struct ::winsize ws;
		if (0 == ::ioctl(m_tty_fd, TIOCGWINSZ, &ws)) {
			return resize(ws.ws_col, ws.ws_row);
		} else {
			BEARD_DEBUG_CERR_FQN(
				errno,
				"ioctl() TIOCGWINSZ failed"
			);
		}
	}
	return false;
}

#undef BEARD_SCOPE_CLASS

} // namespace tty
} // namespace Beard
