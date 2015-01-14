/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/String.hpp>
#include <Beard/keys.hpp>
#include <Beard/utility.hpp>
#include <Beard/txt/Defs.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Caps.hpp>
#include <Beard/tty/Terminal.hpp>

#include <duct/traits.hpp>
#include <duct/debug.hpp>
#include <duct/EncodingUtils.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
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

#include <Beard/detail/gr_ceformat.hpp>
#include <Beard/detail/debug.hpp>

#if !defined(BEARD_TTY_TERMINAL_IGNORE_OLD_CASTS) && (	\
	 (DUCT_COMPILER & DUCT_FLAG_COMPILER_GCC) ||		\
	((DUCT_COMPILER & DUCT_FLAG_COMPILER_CLANG) &&		\
	  DUCT_COMPILER > DUCT_FLAG_COMPILER_CLANG33))
//
	#define BEARD_TTY_TERMINAL_IGNORE_OLD_CASTS
#endif

namespace Beard {
namespace tty {

#define BEARD_SCOPE_CLASS tty::Terminal

static_assert(
	std::is_trivially_copyable<tty::Cell>::value,
	"tty::Cell must be trivially copyable"
);

struct Terminal::terminal_private final {
	bool have_orig{false};
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
	tty::CapString::cursor_normal,

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

// D:
// For arrow keys with modifiers, see parm_*_cursor caps
#define BEARD_TTY_IKM_CURSOR_(name_, shift_name_, cap_id_) \
	{KeyMod::none          , KeyCode:: name_, codepoint_none, tty::CapString::key_ ## name_, {nullptr, 0u}}, \
	{KeyMod::shift         , KeyCode:: name_, codepoint_none, tty::CapString::key_ ## shift_name_, {nullptr, 0u}}, \
	{KeyMod::esc           , KeyCode:: name_, codepoint_none, static_cast<tty::CapString>(-1), "[1;3" cap_id_}, \
	{KeyMod::esc_shift     , KeyCode:: name_, codepoint_none, static_cast<tty::CapString>(-1), "[1;4" cap_id_}, \
	{KeyMod::ctrl          , KeyCode:: name_, codepoint_none, static_cast<tty::CapString>(-1), "[1;5" cap_id_}, \
	{KeyMod::ctrl_shift    , KeyCode:: name_, codepoint_none, static_cast<tty::CapString>(-1), "[1;6" cap_id_}, \
	{KeyMod::esc_ctrl      , KeyCode:: name_, codepoint_none, static_cast<tty::CapString>(-1), "[1;7" cap_id_}, \
	{KeyMod::esc_ctrl_shift, KeyCode:: name_, codepoint_none, static_cast<tty::CapString>(-1), "[1;8" cap_id_}
//

static struct {
	KeyMod const mod;
	KeyCode const code;
	char32 const cp;
	tty::CapString const cap;
	txt::Sequence seq;
} const s_input_keymap[]{
// cap input
	{KeyMod::none , KeyCode::insert, codepoint_none, tty::CapString::key_ic, {nullptr, 0u}},
	{KeyMod::shift, KeyCode::insert, codepoint_none, tty::CapString::key_sic, {nullptr, 0u}},
	{KeyMod::none , KeyCode::del, codepoint_none, tty::CapString::key_dc, {nullptr, 0u}},
	{KeyMod::shift, KeyCode::del, codepoint_none, tty::CapString::key_sdc, {nullptr, 0u}},
	{KeyMod::none , KeyCode::home, codepoint_none, tty::CapString::key_home, {nullptr, 0u}},
	{KeyMod::shift, KeyCode::home, codepoint_none, tty::CapString::key_shome, {nullptr, 0u}},
	{KeyMod::none , KeyCode::end, codepoint_none, tty::CapString::key_end, {nullptr, 0u}},
	{KeyMod::shift, KeyCode::end, codepoint_none, tty::CapString::key_send, {nullptr, 0u}},
	{KeyMod::none, KeyCode::pgup, codepoint_none, tty::CapString::key_ppage, {nullptr, 0u}},
	{KeyMod::none, KeyCode::pgdn, codepoint_none, tty::CapString::key_npage, {nullptr, 0u}},

	BEARD_TTY_IKM_CURSOR_(up   , sr    , "A"),
	BEARD_TTY_IKM_CURSOR_(down , sf    , "B"),
	BEARD_TTY_IKM_CURSOR_(left , sleft , "D"),
	BEARD_TTY_IKM_CURSOR_(right, sright, "C"),

	{KeyMod::none, KeyCode::f1, codepoint_none, tty::CapString::key_f1, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f2, codepoint_none, tty::CapString::key_f2, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f3, codepoint_none, tty::CapString::key_f3, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f4, codepoint_none, tty::CapString::key_f4, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f5, codepoint_none, tty::CapString::key_f5, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f6, codepoint_none, tty::CapString::key_f6, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f7, codepoint_none, tty::CapString::key_f7, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f8, codepoint_none, tty::CapString::key_f8, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f9, codepoint_none, tty::CapString::key_f9, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f10, codepoint_none, tty::CapString::key_f10, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f11, codepoint_none, tty::CapString::key_f11, {nullptr, 0u}},
	{KeyMod::none, KeyCode::f12, codepoint_none, tty::CapString::key_f12, {nullptr, 0u}},

// single-char input
	//{KeyMod::ctrl, KeyCode::none, '~', static_cast<tty::CapString>(-1), "\x00"},
	{KeyMod::ctrl, KeyCode::none, '2', static_cast<tty::CapString>(-1), "\x00"},
	{KeyMod::ctrl, KeyCode::none, 'a', static_cast<tty::CapString>(-1), "\x01"},
	{KeyMod::ctrl, KeyCode::none, 'b', static_cast<tty::CapString>(-1), "\x02"},
	{KeyMod::ctrl, KeyCode::none, 'c', static_cast<tty::CapString>(-1), "\x03"},
	{KeyMod::ctrl, KeyCode::none, 'd', static_cast<tty::CapString>(-1), "\x04"},
	{KeyMod::ctrl, KeyCode::none, 'e', static_cast<tty::CapString>(-1), "\x05"},
	{KeyMod::ctrl, KeyCode::none, 'f', static_cast<tty::CapString>(-1), "\x06"},
	{KeyMod::ctrl, KeyCode::none, 'g', static_cast<tty::CapString>(-1), "\x07"},
	{KeyMod::none, KeyCode::backspace, '\x1', static_cast<tty::CapString>(-1), "\x08"},
	{KeyMod::none , KeyCode::none, '\t', static_cast<tty::CapString>(-1), "\x09"},
	{KeyMod::shift, KeyCode::none, '\t', tty::CapString::key_btab, {nullptr, 0u}},
	{KeyMod::ctrl, KeyCode::none, 'j', static_cast<tty::CapString>(-1), "\x0A"},
	{KeyMod::ctrl, KeyCode::none, 'k', static_cast<tty::CapString>(-1), "\x0B"},
	{KeyMod::ctrl, KeyCode::none, 'l', static_cast<tty::CapString>(-1), "\x0C"},
	{KeyMod::none, KeyCode::enter, codepoint_none, static_cast<tty::CapString>(-1), "\x0D"},
	{KeyMod::ctrl, KeyCode::none, 'n', static_cast<tty::CapString>(-1), "\x0E"},
	{KeyMod::ctrl, KeyCode::none, 'o', static_cast<tty::CapString>(-1), "\x0F"},
	{KeyMod::ctrl, KeyCode::none, 'p', static_cast<tty::CapString>(-1), "\x10"},
	{KeyMod::ctrl, KeyCode::none, 'q', static_cast<tty::CapString>(-1), "\x11"},
	{KeyMod::ctrl, KeyCode::none, 'r', static_cast<tty::CapString>(-1), "\x12"},
	{KeyMod::ctrl, KeyCode::none, 's', static_cast<tty::CapString>(-1), "\x13"},
	{KeyMod::ctrl, KeyCode::none, 't', static_cast<tty::CapString>(-1), "\x14"},
	{KeyMod::ctrl, KeyCode::none, 'u', static_cast<tty::CapString>(-1), "\x15"},
	{KeyMod::ctrl, KeyCode::none, 'v', static_cast<tty::CapString>(-1), "\x16"},
	{KeyMod::ctrl, KeyCode::none, 'w', static_cast<tty::CapString>(-1), "\x17"},
	{KeyMod::ctrl, KeyCode::none, 'x', static_cast<tty::CapString>(-1), "\x18"},
	{KeyMod::ctrl, KeyCode::none, 'y', static_cast<tty::CapString>(-1), "\x19"},
	{KeyMod::ctrl, KeyCode::none, 'z', static_cast<tty::CapString>(-1), "\x1A"},
	{KeyMod::none, KeyCode::esc, codepoint_none, static_cast<tty::CapString>(-1), "\x1B\x1B"},
	{KeyMod::ctrl, KeyCode::none, '4', static_cast<tty::CapString>(-1), "\x1C"},
	{KeyMod::ctrl, KeyCode::none, '5', static_cast<tty::CapString>(-1), "\x1D"},
	{KeyMod::ctrl, KeyCode::none, '6', static_cast<tty::CapString>(-1), "\x1E"},
	{KeyMod::ctrl, KeyCode::none, '/', static_cast<tty::CapString>(-1), "\x1F"},
	{KeyMod::none, KeyCode::none, ' ', static_cast<tty::CapString>(-1), "\x20"},
	{KeyMod::none, KeyCode::backspace, codepoint_none, static_cast<tty::CapString>(-1), "\x7F"},
};

#undef BEARD_TTY_IKM_CURSOR_
} // anonymous namespace

struct terminal_internal final
	: duct::traits::restrict_all
{

static_assert(
	std::extent<decltype(s_cap_cache_table)>::value
	== static_cast<std::size_t>(Terminal::CapCache::COUNT),
	"s_cap_cache_table is not the correct size"
);

using KeyDecodeNode = tty::Terminal::KeyDecodeNode;

#define BEARD_SCOPE_FUNC internal::close_fd
static void
close_fd(
	tty::fd_type const fd
) noexcept {
	if (tty::FD_INVALID == fd) {
		return;
	}
	unsigned close_tries = 4;
	while (close_tries--) {
		if (0 == ::close(fd)) {
			break;
		} else {
			signed const err = errno;
			BEARD_DEBUG_CERR_FQN(
				err,
				"failed to close file descriptor"
			);
			if (EINTR == err || EIO == err) {
				// Only retry when we're interrupted by a signal
				// or hit an IO error
				continue;
			} else {
				// EBADF or something non-standard
				break;
			}
		}
	}
}
#undef BEARD_SCOPE_FUNC

// SIGWINCH handling

static void
sigwinch_handler(signed /*signum*/) {
	if (nullptr != s_sigwinch_terminal) {
		s_sigwinch_terminal->m_ev_pending.resize.pending = true;
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

	// Curse you, SIG_DFL
	#ifdef BEARD_TTY_TERMINAL_IGNORE_OLD_CASTS
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wold-style-cast"
	#endif
	if (!BEARD_TERMINAL_SIG_HANDLER_IS_NULLARY_(sig_current)) {
		BEARD_THROW_FQN(
			ErrorCode::tty_sigwinch_handler_already_active,
			"someone else already owns the SIGWINCH handler"
		);
	}
	#ifdef BEARD_TTY_TERMINAL_IGNORE_OLD_CASTS
		#pragma GCC diagnostic pop
	#endif

	struct ::sigaction sig{};
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
			struct ::sigaction sig{};
			#ifdef BEARD_TTY_TERMINAL_IGNORE_OLD_CASTS
				#pragma GCC diagnostic push
				#pragma GCC diagnostic ignored "-Wold-style-cast"
			#endif
			sig.sa_handler = SIG_IGN;
			#ifdef BEARD_TTY_TERMINAL_IGNORE_OLD_CASTS
				#pragma GCC diagnostic pop
			#endif
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
	tty::Terminal::cell_vector_type& cell_buffer,
	tty::Cell const& cell = tty::s_cell_default
) {
	// TODO: Optimize with cache buffer of constant size and std::copy()?
	std::fill(
		cell_buffer.begin(),
		cell_buffer.end(),
		cell
	);
}

static void
resize_buffer(
	geom_value_type const old_width,
	geom_value_type const old_height,
	tty::Terminal::cell_vector_type& oldb,
	unsigned const new_size,
	geom_value_type const new_width,
	geom_value_type const new_height,
	bool const retain
) {
	// TODO: Choose optimal ctor if new buffer is strictly smaller
	if (retain) {
		tty::Terminal::cell_vector_type newb{new_size, tty::s_cell_default};
		if (0u < oldb.size() && 0u < newb.size()) {
			auto const
				min_width  = min_ce(old_width , new_width),
				min_height = min_ce(old_height, new_height)
			;
			auto it_old = oldb.cbegin();
			auto it_new = newb.begin();
			for (geom_value_type y = 0; min_height > y; ++y) {
				std::copy(it_old, it_old + min_width, it_new);
				it_old += old_width;
				it_new += new_width;
			}
		}
		oldb.operator=(std::move(newb));
	} else {
		oldb.resize(new_size);
		std::fill(oldb.begin(), oldb.end(), tty::s_cell_default);
	}
}

// rendering

#define BEARD_TERMINAL_WRITE_STRLIT(s_, sl_) \
	(s_).write((sl_), sizeof(sl_) - 1)

static void
repos_out(
	std::ostream& stream,
	geom_value_type const x,
	geom_value_type const y
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
write_colors(
	std::ostream& stream,
	unsigned const color_fg,
	unsigned const color_bg
) {
	enum : unsigned {
		color16_max = 0x09
	};

	unsigned const def
		= (tty::Color::term_default != color_fg)
		| (tty::Color::term_default != color_bg) << 1u
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
	std::size_t const size = terminal.m_streambuf_out.get_sequence_size();
	ssize_t written = 0;
	signed err = 0;
	unsigned retries = 1;
	do {
		written = ::write(
			terminal.m_tty_fd,
			terminal.m_streambuf_out.get_buffer().data(),
			size
		);
		if (-1 == written) {
			err = errno;
			BEARD_DEBUG_CERR_FQN(
				err,
				"write() failed or was interrupted"
				" (potentially retrying)"
			);
		} else {
			break;
		}
	} while (retries-- && err == EINTR);
	if (-1 != written) {
		if (static_cast<std::size_t>(written) != size) {
			BEARD_DEBUG_MSG_FQN(
				"failed to flush all bytes to terminal"
			);
			// Slide back the remaining data and seek to end of buffer
			terminal.m_stream_out.seekp(
				terminal.m_streambuf_out.discard(
					static_cast<std::size_t>(written)
				)
			);
		} else {
			// Reset buffer (position is 0)
			DUCT_ASSERT(
				terminal.m_streambuf_out.reset(size),
				"failed to resize buffer"
			);
		}
	}
}
#undef BEARD_SCOPE_FUNC

static void
add_key_cap(
	KeyDecodeNode* node,
	char const* it,
	char const* const end,
	KeyMod const mod,
	KeyCode const code,
	char32 const cp
) {
	// On termination by for condition, branch already exists
	for (
		auto next_it = node->next.begin();
		end != it;
	) {
		if (node->next.end() == next_it) {
			// New branch
			while (end != it) {
				node->next.emplace_front(new KeyDecodeNode(
					*it,
					KeyMod::none,
					KeyCode::none,
					codepoint_none
				));
				node = node->next.front().get();
				++it;
			}
			node->mod = mod;
			node->code = code;
			node->cp = cp;
			return;
		} else if ((*it) == (*next_it)->ch) {
			// Existing branch; step in
			node = next_it->get();
			next_it = node->next.begin();
			++it;
		} else {
			++next_it;
		}
	}
}

static std::size_t
decode_key(
	KeyDecodeNode const* node,
	char const* it,
	char const* const end,
	KeyMod& mod,
	KeyCode& code,
	char32& cp
) {
	// NB: Root in the graph is just a dummy
	std::size_t seq_size = 0u;
	for (
		auto next_it = node->next.begin();
		end != it;
	) {
		if (node->next.end() == next_it) {
			// Not found
			break;
		} else if ((*it) == (*next_it)->ch) {
			// Matching branch
			++seq_size;
			if ((*next_it)->is_terminator()) {
				// Terminating sequence
				mod = (*next_it)->mod;
				code = (*next_it)->code;
				cp = (*next_it)->cp;
				return seq_size;
			} else {
				// Step into branch
				node = next_it->get();
				next_it = node->next.begin();
				++it;
			}
		} else {
			++next_it;
		}
	}
	return 0u;
}

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
{
	update_cache();
}

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
Terminal::clear_screen(
	bool const back_dirty
) {
	m_states.set(State::backbuffer_dirty, back_dirty);
	std::fill(m_dirty_rows.begin(), m_dirty_rows.end(), back_dirty);

	terminal_internal::write_attrs(
		*this,
		tty::Color::term_default,
		tty::Color::term_default,
		true
	);
	put_cap_cache(CapCache::clear_screen);
	if (is_caret_visible()) {
		terminal_internal::repos_out(
			m_stream_out, m_caret_pos.x, m_caret_pos.y
		);
	}
	terminal_internal::flush(*this);
}

bool
Terminal::resize(
	geom_value_type const new_width,
	geom_value_type const new_height
) {
	if (new_width == m_tty_size.width && new_height == m_tty_size.height) {
		return false;
	}

	std::size_t const size = new_width * new_height;
	terminal_internal::resize_buffer(
		m_tty_size.width, m_tty_size.height,
		m_cell_backbuffer, size, new_width, new_height,
		m_states.test(State::retain_backbuffer)
	);
	terminal_internal::resize_buffer(
		m_tty_size.width, m_tty_size.height,
		m_cell_frontbuffer, size, new_width, new_height,
		false
	);

	m_dirty_rows.resize(new_height);
	clear_screen(m_states.test(State::retain_backbuffer));

	m_tty_size.width  = new_width;
	m_tty_size.height = new_height;
	return true;
}

#define BEARD_SCOPE_FUNC init
void
Terminal::init(
	tty::fd_type const tty_fd,
	bool const use_sigwinch
) try {
	m_ev_pending.reset();
	m_tty_fd = tty_fd;

	tty::fd_type const epoll_fd = ::epoll_create1(0);
	if (-1 == epoll_fd) {
		BEARD_THROW_CERR(
			ErrorCode::tty_init_failed,
			errno,
			"failed to create epoll instance"
		);
	}

	struct ::epoll_event epoll_ev{};
	epoll_ev.events = EPOLLIN | EPOLLPRI;
	if (0 != ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, m_tty_fd, &epoll_ev)) {
		auto const err = errno;
		::close(epoll_fd);
		BEARD_THROW_CERR(
			ErrorCode::tty_init_failed,
			err,
			"failed to add tty to epoll"
		);
	}
	m_epoll_fd = epoll_fd;

	m_tty_priv->have_orig = false;
	if (0 != ::tcgetattr(m_tty_fd, &m_tty_priv->tios_orig)) {
		BEARD_THROW_CERR(
			ErrorCode::tty_init_failed,
			errno,
			"failed to fetch current termios"
		);
	}
	m_tty_priv->tios = m_tty_priv->tios_orig;
	/*std::memcpy(
		&m_tty_priv->tios,
		&m_tty_priv->tios_orig,
		sizeof(struct ::termios)
	);*/
	m_tty_priv->have_orig = true;

	// Input modes
	m_tty_priv->tios.c_iflag |= IUTF8;
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
	m_tty_priv->tios.c_cflag |= CS8 | CLOCAL;

	// Local modes
	m_tty_priv->tios.c_lflag &=
	~(
		// Disable signal generation
		ISIG |
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

	// TODO: tcsetattr() can succeed despite not applying all modes;
	// should we compare with the new state to make sure the modes
	// were applied?
	if (0 != ::tcsetattr(m_tty_fd, TCSAFLUSH, &m_tty_priv->tios)) {
		BEARD_THROW_CERR(
			ErrorCode::tty_init_failed,
			errno,
			"failed to set termios"
		);
	}

	put_cap_cache(CapCache::enter_ca_mode);
	put_cap_cache(CapCache::keypad_xmit);
	(is_caret_visible())
		? put_cap_cache(CapCache::cursor_normal)
		: put_cap_cache(CapCache::cursor_invisible)
	;
	update_size();
	terminal_internal::flush(*this);

	if (use_sigwinch) {
		terminal_internal::setup_sigwinch_handler(*this);
	}
} catch (...) {
	if (use_sigwinch) {
		terminal_internal::release_sigwinch_handler(*this);
	}
	terminal_internal::close_fd(m_epoll_fd);
	m_epoll_fd = tty::FD_INVALID;

	if (m_tty_priv->have_orig) {
		if (0 != ::tcsetattr(m_tty_fd, TCSAFLUSH, &m_tty_priv->tios_orig)) {
			BEARD_DEBUG_CERR_FQN(
				errno,
				"failed to reset original termios"
			);
		}
		m_tty_priv->have_orig = false;
	}
	m_tty_fd = tty::FD_INVALID;

	throw;
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC deinit
void
Terminal::deinit() {
	m_ev_pending.reset();

	terminal_internal::close_fd(m_epoll_fd);
	terminal_internal::release_sigwinch_handler(*this);

	set_caret_pos(0u, 0u);
	set_caret_visible(false);

	resize(0u, 0u);
	terminal_internal::write_attrs(
		*this,
		tty::Color::term_default,
		tty::Color::term_default,
		true
	);
	put_cap_cache(CapCache::cursor_normal);
	put_cap_cache(CapCache::exit_attribute_mode);
	put_cap_cache(CapCache::clear_screen);
	put_cap_cache(CapCache::exit_ca_mode);
	put_cap_cache(CapCache::keypad_local);
	terminal_internal::flush(*this);

	if (
		m_tty_priv->have_orig &&
		0 != ::tcsetattr(m_tty_fd, TCSAFLUSH, &m_tty_priv->tios_orig)
	) {
		BEARD_DEBUG_CERR_FQN(
			errno,
			"failed to reset original termios"
		);
	}
	m_tty_priv->have_orig = false;

	m_attr_fg_last = tty::Color::term_default;
	m_attr_bg_last = tty::Color::term_default;

	m_streambuf_in.commit_direct(0u, false);
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC poll_input
void
Terminal::poll_input(
	unsigned const input_timeout
) {
	std::size_t seq_size = m_streambuf_in.get_sequence_size();
	if (inbuf_high_mark <= seq_size) {
		seq_size = m_streambuf_in.discard(
			inbuf_size == seq_size && 0u == m_streambuf_in.get_position()
			? seq_size
			: m_streambuf_in.get_position()
		);
	}

	struct ::epoll_event ev;
	signed ready_count = -1, err = 0;
	unsigned retries = 1;
	do {
		ready_count = ::epoll_wait(
			m_epoll_fd, &ev, 1,
			static_cast<signed>(input_timeout)
		);
		if (-1 == ready_count) {
			err = errno;
			if (EINTR != err) {
				BEARD_DEBUG_CERR_FQN(
					err,
					"failed to epoll tty (potentially retrying)"
				);
			} else if (0 != input_timeout && m_ev_pending.resize.pending) {
				// Avoid infinite/long timeout if a SIGWINCH
				// interrupted epoll_wait().
				return;
			}
		} else {
			break;
		}
	} while (retries-- && EINTR == err);

	if (0 < ready_count && (ev.events & (EPOLLIN | EPOLLPRI))) {
		err = 0;
		retries = 1;
		ssize_t amt_read = 0;
		do {
			amt_read = ::read(
				m_tty_fd,
				m_streambuf_in.get_buffer().data() + seq_size,
				inbuf_size - seq_size
			);
			if (-1 == amt_read) {
				err = errno;
				BEARD_DEBUG_CERR_FQN(
					err,
					"failed to read from tty (potentially retrying)"
				);
			} else {
				break;
			}
		} while (retries-- && EINTR == err);
		if (0 < amt_read) {
			m_streambuf_in.commit_direct(
				seq_size + static_cast<std::size_t>(amt_read),
				true
			);
		}
	}
}
#undef BEARD_SCOPE_FUNC

bool
Terminal::parse_input() {
	char const* const buffer
		= m_streambuf_in.get_buffer().data()
		+ m_streambuf_in.get_position()
	;
	bool have_event = false;
	std::size_t seq_size = terminal_internal::decode_key(
		&m_key_decode_graph,
		buffer,
		buffer + m_streambuf_in.get_remaining(),
		m_ev_pending.key_input.mod,
		m_ev_pending.key_input.code,
		m_ev_pending.key_input.cp
	);
	if (0u != seq_size) {
		// Key specified by a cap or single non-ASCII char
		have_event = true;
	} else if ('\033' == buffer[0u]) {
		seq_size = 1u;
		if (m_ev_pending.key_input.escaped) {
			// Already have escape character
			m_ev_pending.key_input.escaped = false;
			m_ev_pending.key_input.mod  = KeyMod::none;
			m_ev_pending.key_input.code = KeyCode::esc;
			m_ev_pending.key_input.cp   = codepoint_none;
			have_event = true;
		} else {
			m_ev_pending.key_input.escaped = true;
			have_event = false;
		}
	} else {
		// Else hopefully a sequence of UTF-8 units
		seq_size = txt::EncUtils::required_first_whole(buffer[0u]);
		if (m_streambuf_in.get_remaining() >= seq_size) {
			char32 cp = codepoint_none;
			txt::EncUtils::decode(
				buffer,
				buffer + seq_size,
				cp,
				codepoint_none
			);
			if (codepoint_none != cp) {
				m_ev_pending.key_input.mod = KeyMod::none;
				m_ev_pending.key_input.code = KeyCode::none;
				m_ev_pending.key_input.cp = cp;
				have_event = true;
			}
		} else {
			seq_size = 0u;
		}
	}
	if (0u < seq_size) {
		m_streambuf_in.pubseekoff(
			static_cast<duct::IO::dynamic_streambuf::off_type>(seq_size),
			std::ios_base::cur,
			std::ios_base::in
		);
	}
	return have_event;
}

// input control

void
Terminal::set_caret_pos(
	geom_value_type x,
	geom_value_type y
) {
	x = max_ce(0, x);
	y = max_ce(0, y);
	if (x != m_caret_pos.x || y != m_caret_pos.y) {
		m_caret_pos.x = x;
		m_caret_pos.y = y;
		if (is_caret_visible()) {
			terminal_internal::repos_out(
				m_stream_out, m_caret_pos.x, m_caret_pos.y
			);
			terminal_internal::flush(*this);
		}
	}
}

void
Terminal::set_caret_visible(
	bool const visible
) {
	if (is_caret_visible() != visible) {
		m_states.set(State::caret_visible, visible);
		if (visible) {
			put_cap_cache(CapCache::cursor_normal);
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
	geom_value_type const x,
	geom_value_type const y,
	tty::Cell const& cell
) noexcept {
	if (0 <= x && x < m_tty_size.width
	 && 0 <= y && y < m_tty_size.height
	 && '\0' != cell.u8block.units[0u]
	) {
		auto const it = m_cell_backbuffer.begin() + (y * m_tty_size.width) + x;
		if (BEARD_TERMINAL_CELL_CMP(*it, cell)) {
			std::copy(&cell, &cell + 1u, it);
			m_states.enable(State::backbuffer_dirty);
			m_dirty_rows[y] = true;
		}
	}
}

void
Terminal::put_sequence(
	geom_value_type x,
	geom_value_type const y,
	txt::Sequence const& seq,
	std::size_t const points,
	tty::attr_type const attr_fg,
	tty::attr_type const attr_bg
) noexcept {
	// TODO: Handle \n and clipping
	if (0 <= x && x < m_tty_size.width && 0 <= y && y < m_tty_size.height) {
		auto const range = min_ce(
			points + unsigned_cast(x),
			static_cast<std::size_t>(m_tty_size.width)
		);
		bool dirtied = false;
		std::size_t ss_size = 0u;
		tty::Cell cell{{}, attr_fg, attr_bg};
		auto it_put = m_cell_backbuffer.begin() + (y * m_tty_size.width) + x;
		auto it_seq = seq.data;
		auto const seq_end = seq.data + seq.size;
		for (
			; seq_end > it_seq && range > unsigned_cast(x);
			++x,
			++it_put,
			it_seq += ss_size
		) {
			ss_size = txt::EncUtils::required_first_whole(
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
			m_states.enable(State::backbuffer_dirty);
			m_dirty_rows[y] = true;
		}
	}
}

void
Terminal::put_line(
	Vec2 position,
	geom_value_type length,
	Axis const direction,
	Cell const& cell
) noexcept {
	// NB: The transpose axis value won't vary
	if (
		'\0' == cell.u8block.units[0u] ||
		0 >= length ||
		!value_in_bounds(
			vec2_axis_value(position, axis_transpose(direction)),
			0, vec2_axis_value(m_tty_size, axis_transpose(direction))
		)
	) {
		return;
	}

	geom_value_type& aval = vec2_axis_ref(position, direction);
	geom_value_type const
		target = value_clamp(
			aval + length,
			0, vec2_axis_value(m_tty_size, direction)
		),
		stride = Axis::horizontal == direction
			? 1
			: m_tty_size.width
	;
	if (vec2_axis_value(m_tty_size, direction) > aval) {
		aval = max_ce(aval, 0);
		bool dirtied = false;
		auto it
			= m_cell_backbuffer.begin()
			+ (position.y * m_tty_size.width) + position.x
		;
		for (; aval < target; ++aval, it += stride) {
			if (BEARD_TERMINAL_CELL_CMP(*it, cell)) {
				std::copy(&cell, &cell + 1u, it);
				m_dirty_rows[position.y] = true;
				dirtied = true;
			}
		}
		if (dirtied) {
			m_states.enable(State::backbuffer_dirty);
		}
	}
}

void
Terminal::put_rect(
	Rect const& rect,
	txt::UTF8Block const (&frame)[8u],
	tty::attr_type const attr_fg,
	tty::attr_type const attr_bg
) noexcept {
	enum : unsigned {
		corner_top_left = 0u,
		edge_top,
		corner_top_right,
		edge_right,
		corner_bot_right,
		edge_bot,
		corner_bot_left,
		edge_left
	};

	if (
		1 >= rect.size.width ||
		1 >= rect.size.height ||
		!rect_intersects(rect, Rect{{0, 0}, m_tty_size})
	) {
		return;
	}

	Vec2 pos = rect.pos;
	tty::Cell cell{frame[edge_top], attr_fg, attr_bg};
	++pos.x;
	put_line(pos, rect.size.width - 2, Axis::x, cell);
	cell.u8block = frame[edge_bot];
	pos.y += rect.size.height - 1;
	put_line(pos, rect.size.width - 2, Axis::x, cell);

	cell.u8block = frame[edge_left];
	--pos.x; pos.y = rect.pos.y + 1;
	put_line(pos, rect.size.height - 2, Axis::y, cell);
	cell.u8block = frame[edge_right];
	pos.x += rect.size.width - 1;
	put_line(pos, rect.size.height - 2, Axis::y, cell);

	cell.u8block = frame[corner_top_left];
	put_cell(
		rect.pos.x,
		rect.pos.y,
		cell
	);
	cell.u8block = frame[corner_top_right];
	put_cell(
		rect.pos.x + rect.size.width - 1,
		rect.pos.y,
		cell
	);
	cell.u8block = frame[corner_bot_right];
	put_cell(
		rect.pos.x + rect.size.width - 1,
		rect.pos.y + rect.size.height - 1,
		cell
	);
	cell.u8block = frame[corner_bot_left];
	put_cell(
		rect.pos.x,
		rect.pos.y + rect.size.height - 1,
		cell
	);
}

void
Terminal::present() {
	if (!is_open() || !m_states.test(State::backbuffer_dirty)) {
		return;
	}

	m_stream_out.clear();
	//put_cap_cache(CapCache::exit_attribute_mode);

	// FIXME: Hide caret when flushing?
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
	geom_value_type row = 0;
	unsigned
		cseq_f = 0u,
		cseq_e = 0u
	;
	for (
		fit_row = m_cell_frontbuffer.begin(),
		fit_row_n = fit_row + m_tty_size.width,
		bit_row = m_cell_backbuffer.cbegin()
		; m_tty_size.height != row
		; fit_row = fit_row_n,
		// FIXME: Danger danger. stdlib should be OK with this?
		// (It's not being dereferenced.)
		fit_row_n += m_tty_size.width,
		bit_row += m_tty_size.width,
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
	m_states.disable(State::backbuffer_dirty);

	// Reset to the caret position
	terminal_internal::repos_out(
		m_stream_out, m_caret_pos.x, m_caret_pos.y
	);
	terminal_internal::flush(*this);
}

void
Terminal::clear_front(
	bool const clear_back
) {
	terminal_internal::clear_buffer(m_cell_frontbuffer);
	if (clear_back) {
		terminal_internal::clear_buffer(m_cell_backbuffer);
	}
	clear_screen(!clear_back);
}

void
Terminal::clear_back(
	tty::Cell const& cell
) noexcept {
	terminal_internal::clear_buffer(m_cell_backbuffer, cell);
	m_states.enable(State::backbuffer_dirty);
	std::fill(m_dirty_rows.begin(), m_dirty_rows.end(), true);
}

void
Terminal::clear_back(
	Rect const& rect,
	tty::Cell const& cell
) noexcept {
	Vec2 const v0{0, 0};
	Quad quad = rect_abs_quad(rect);
	vec2_clamp_min(quad.v1, v0);
	vec2_clamp_max(quad.v2, m_tty_size);
	if (v0 == quad.v1 && make_const(m_tty_size) == quad.v2) {
		clear_back(cell);
	} else {
		// Eliminate degeneracies
		vec2_clamp_min(quad.v2, quad.v1);
		Rect const rect_clamped = quad_rect(quad);
		if (v0 == rect_clamped.size) {
			return;
		}
		auto it_dirty = m_dirty_rows.begin() + rect_clamped.pos.y;
		auto it_row
			= m_cell_backbuffer.begin()
			+ (rect_clamped.pos.y * m_tty_size.width)
			+ rect_clamped.pos.x;
		auto const it_row_e
			= it_row + (rect_clamped.size.height * m_tty_size.width);
		bool row_dirtied = false, state_dirtied = false;
		for (
			; it_row_e > it_row;
			it_row += m_tty_size.width, ++it_dirty
		) {
			auto const it_row_stride = it_row + rect_clamped.size.width;
			for (
				auto it = it_row;
				it_row_stride != it;
				++it
			) {
				if (BEARD_TERMINAL_CELL_CMP(*it, cell)) {
					std::copy(&cell, &cell + 1u, it);
					row_dirtied = true;
				}
			}
			if (row_dirtied) {
				(*it_dirty) = true;
				state_dirtied = true;
				row_dirtied = false;
			}
		}
		if (state_dirtied) {
			m_states.enable(State::backbuffer_dirty);
		}
	}
}

// events

#define BEARD_SCOPE_FUNC poll
tty::EventType
Terminal::poll(
	tty::Event& event,
	unsigned const input_timeout
) {
	event.type = tty::EventType::none;
	if (!is_open()) {
		/* Do nothing. */
	} else if (m_ev_pending.resize.pending) {
		m_ev_pending.resize.pending = false;
		event.resize.old_size = m_tty_size;
		if (update_size()) {
			event.type = tty::EventType::resize;
		}
	} else {
		poll_input(input_timeout);
		bool parse_escape = false;
		l_parse_escape:
		if (0u < m_streambuf_in.get_remaining()) {
			if (parse_input()) {
				/*BEARD_DEBUG_MSG_FQN_F(
					"parsed: seq_size = %zu  pos = %zu  remaining = %zu",
					m_streambuf_in.get_sequence_size(),
					m_streambuf_in.get_position(),
					m_streambuf_in.get_remaining()
				);*/
				event.type = tty::EventType::key_input;
				event.key_input.mod =
				(true == m_ev_pending.key_input.escaped)
					? static_cast<KeyMod>(
						enum_cast(event.key_input.mod) |
						enum_cast(KeyMod::esc)
					)
					: m_ev_pending.key_input.mod
				;
				event.key_input.code = m_ev_pending.key_input.code;
				event.key_input.cp   = m_ev_pending.key_input.cp;
				m_ev_pending.key_input.reset();
			} else if (!parse_escape && m_ev_pending.key_input.escaped) {
				/*BEARD_DEBUG_MSG_FQN_F(
					"escaped: seq_size = %zu  pos = %zu  remaining = %zu",
					m_streambuf_in.get_sequence_size(),
					m_streambuf_in.get_position(),
					m_streambuf_in.get_remaining()
				);*/
				parse_escape = true;
				goto l_parse_escape;
			}
		}
	}
	return event.type;
}
#undef BEARD_SCOPE_FUNC

// operations

void
Terminal::update_cache() {
	// Cache caps
	tty::TerminalInfo::cap_string_map_type::const_iterator cap_it;
	for (
		unsigned idx = 0u;
		enum_cast(Terminal::CapCache::COUNT) > idx;
		++idx
	) {
		if (m_info.lookup_cap_string(s_cap_cache_table[idx], cap_it)) {
			m_cap_cache[idx].assign(cap_it->second);
		} else {
			m_cap_cache[idx].clear();
			BEARD_DEBUG_MSG_FQN_F(
				"missing function cap: %u (CapString) %u (CapCache)",
				idx,
				s_cap_cache_table[idx]
			);
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

	// Cache key decoding graph
	m_key_decode_graph.next.clear();
	for (auto const kmap : s_input_keymap) {
		if (static_cast<tty::CapString>(-1) != kmap.cap) {
			if (m_info.lookup_cap_string(kmap.cap, cap_it)) {
				if (!cap_it->second.empty()) {
					terminal_internal::add_key_cap(
						&m_key_decode_graph,
						cap_it->second.data(),
						cap_it->second.data() + cap_it->second.size(),
						kmap.mod,
						kmap.code,
						kmap.cp
					);
				} else {
					BEARD_DEBUG_MSG_FQN_F(
						"key %u (CapString) %u (KeyCode) %u (codepoint)"
						" is empty",
						static_cast<unsigned>(kmap.cap),
						static_cast<unsigned>(kmap.code),
						static_cast<unsigned>(kmap.cp)
					);
				}
			}
		} else {
			terminal_internal::add_key_cap(
				&m_key_decode_graph,
				kmap.seq.data,
				kmap.seq.data + kmap.seq.size,
				kmap.mod,
				kmap.code,
				kmap.cp
			);
		}
	}
}

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
		terminal_internal::close_fd(tty_fd);
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

	init(tty_fd, use_sigwinch);
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC close
void
Terminal::close() noexcept {
	if (!is_open()) {
		return;
	}

	deinit();
	terminal_internal::close_fd(m_tty_fd);
	m_tty_fd = tty::FD_INVALID;
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC update_size
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
#undef BEARD_SCOPE_FUNC

#undef BEARD_SCOPE_CLASS

} // namespace tty
} // namespace Beard
