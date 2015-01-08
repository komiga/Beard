/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Terminal class.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/String.hpp>
#include <Beard/keys.hpp>
#include <Beard/geometry.hpp>
#include <Beard/utility.hpp>
#include <Beard/txt/Defs.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/TerminalInfo.hpp>

#include <duct/cc_unique_ptr.hpp>
#include <duct/StateStore.hpp>
#include <duct/IO/dynamic_streambuf.hpp>

#include <utility>
#include <istream>
#include <ostream>

namespace Beard {
namespace tty {

// Forward declarations
class Terminal;

/**
	@addtogroup tty
	@{
*/

/** @cond INTERNAL */
namespace {

class moveable_ostream final
	: public std::basic_ostream<char>
{
private:
	using base = std::basic_ostream<char>;

public:
	moveable_ostream(
		std::basic_streambuf<char>& streambuf
	)
		: base(&streambuf)
	{}

	moveable_ostream(
		moveable_ostream&& other
	)
		: base(std::move(other))
	{
		this->set_rdbuf(other.rdbuf());
		other.set_rdbuf(nullptr);
	}

	moveable_ostream&
	operator=(
		moveable_ostream&& other
	) {
		base::operator=(std::move(other));
		this->set_rdbuf(other.rdbuf());
		other.set_rdbuf(nullptr);
		return *this;
	}
};

class moveable_istream final
	: public std::basic_istream<char>
{
private:
	using base = std::basic_istream<char>;

public:
	moveable_istream(
		std::basic_streambuf<char>& streambuf
	)
		: base(&streambuf)
	{}

	moveable_istream(
		moveable_istream&& other
	)
		: base(std::move(other))
	{
		this->set_rdbuf(other.rdbuf());
		other.set_rdbuf(nullptr);
	}

	moveable_istream&
	operator=(
		moveable_istream&& other
	) {
		base::operator=(std::move(other));
		this->set_rdbuf(other.rdbuf());
		other.set_rdbuf(nullptr);
		return *this;
	}
};

} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	%Terminal.
*/
class Terminal final {
private:
	using cell_vector_type = aux::vector<tty::Cell>;

	// Hide termios header from the user's eye :(
	struct terminal_private;
	friend struct terminal_internal;

	enum : unsigned {
		inbuf_size = 0x80,
		inbuf_high_mark = 0x60,
		outbuf_size = 0x800
	};

	enum class State : unsigned {
		retain_backbuffer = bit(0u),
		backbuffer_dirty = bit(1u),
		caret_visible = bit(2u)
	};

	enum class CapCache : unsigned {
		clear_screen = 0u,

		cursor_invisible,
		cursor_normal,

		enter_ca_mode,
		exit_ca_mode,

		exit_attribute_mode, // SGR0
		enter_blink_mode,
		enter_bold_mode,
		enter_reverse_mode,
		enter_underline_mode,

		keypad_local,
		keypad_xmit,

		COUNT
	};

	// CapString keys and non-ASCII single-char key combinations are
	// flattened into a digraph for faster lookup. Top level is a
	// dummy. A match is found as soon as true == is_terminator().
	// The number of nodes traversed to a match is the length of the
	// sequence representing the key (size of the CapString, 1 for
	// non-ASCII single-char input).
	struct KeyDecodeNode {
		// Urk. Can't guarantee forward_list supports incomplete
		// types :(
		using list_type
		= aux::forward_list<
			duct::cc_unique_ptr<KeyDecodeNode>
		>;

		char ch;
		KeyMod mod;
		KeyCode code;
		char32 cp;
		list_type next;

		KeyDecodeNode(
			char ch,
			KeyMod mod,
			KeyCode code,
			char32 cp
		) noexcept
			: ch(ch)
			, mod(mod)
			, code(code)
			, cp(cp)
		{}

		bool
		is_terminator() const noexcept {
			return
				KeyCode::none != code ||
				codepoint_none != cp
			;
		}
	};

	void
	put_cap_cache(
		CapCache const cap
	);

	bool
	resize(
		geom_value_type const new_width,
		geom_value_type const new_height
	);

	void
	clear_screen(
		bool const back_dirty
	);

	void
	init(
		tty::fd_type const tty_fd,
		bool const use_sigwinch
	);

	void
	deinit();

	void
	poll_input(
		unsigned const input_timeout
	);

	bool
	parse_input();

private:
	duct::StateStore<State> m_states{};

	duct::cc_unique_ptr<terminal_private> m_tty_priv;
	tty::fd_type m_tty_fd{tty::FD_INVALID};
	tty::TerminalInfo m_info{};

	String m_cap_cache[enum_cast(CapCache::COUNT)]{};
	unsigned m_cap_max_colors{8u};
	KeyDecodeNode m_key_decode_graph{
		'\0',
		KeyMod::none,
		KeyCode::none,
		codepoint_none
	};

	tty::fd_type m_epoll_fd{tty::FD_INVALID};
	duct::IO::dynamic_streambuf m_streambuf_in {inbuf_size, 0u, inbuf_size};
	duct::IO::dynamic_streambuf m_streambuf_out{outbuf_size};
	moveable_ostream m_stream_out{m_streambuf_out};

	Vec2 m_tty_size{0, 0};
	Vec2 m_caret_pos{0, 0};

	unsigned m_attr_fg_last{tty::Color::term_default};
	unsigned m_attr_bg_last{tty::Color::term_default};

	aux::vector<bool> m_dirty_rows{};
	cell_vector_type m_cell_backbuffer {};
	cell_vector_type m_cell_frontbuffer{};

	struct {
		struct {
			bool pending{false};
			Vec2 old_size{0, 0};

			void
			reset() noexcept {
				pending = false;
				old_size.width = 0;
				old_size.height = 0;
			}
		} resize;

		struct {
			bool escaped{false};
			KeyMod mod{KeyMod::none};
			KeyCode code{KeyCode::none};
			char32 cp{codepoint_none};

			void
			reset() noexcept {
				escaped = false;
				mod = KeyMod::none;
				code = KeyCode::none;
				cp = codepoint_none;
			}
		} key_input;

		void
		reset() noexcept {
			resize.reset();
			key_input.reset();
		}
	} m_ev_pending{};

	Terminal(Terminal const&) = delete;
	Terminal& operator=(Terminal const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Terminal() noexcept;

	/**
		Default constructor.

		@note The terminal info and cache are uninitialized with this
		ctor. See update_cache() and set_info(tty::TerminalInfo).
	*/
	Terminal();

	/**
		Constructor with terminal info.

		@param term_info %Terminal info.
	*/
	Terminal(
		tty::TerminalInfo term_info
	);

	/** Move constructor. */
	Terminal(Terminal&&);
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Terminal& operator=(Terminal&&);
/// @}

/** @name Properties */ /// @{
	/**
		Check if the terminal is initialized.
	*/
	bool
	is_open() const noexcept {
		return tty::FD_INVALID != m_tty_fd;
	}

	/**
		Get size.
	*/
	Vec2 const&
	get_size() const noexcept {
		return m_tty_size;
	}

	/**
		Get terminal width.
	*/
	geom_value_type
	get_width() const noexcept {
		return m_tty_size.width;
	}

	/**
		Get terminal height.
	*/
	geom_value_type
	get_height() const noexcept {
		return m_tty_size.height;
	}

	/**
		Set terminal info.

		@note This will call update_cache().

		@param term_info %Terminal info.
	*/
	void
	set_info(
		tty::TerminalInfo term_info
	) noexcept {
		m_info = std::move(term_info);
		update_cache();
	}

	/**
		Get terminal info (mutable).
	*/
	tty::TerminalInfo&
	get_info() noexcept {
		return m_info;
	}

	/**
		Get terminal info.
	*/
	tty::TerminalInfo const&
	get_info() const noexcept {
		return m_info;
	}

	/**
		Enable or disable back buffer retention after a resize.

		@note This is disabled by default. During a resize, the front
		buffer is cheaply cleared. When this option is disabled, the
		back buffer is also cleared; otherwise, the entire back buffer
		is marked as dirty and all non-default cells will be rewritten
		to the front buffer on the next present().

		@param enable Whether to enable or disable retention.
	*/
	void
	set_opt_retain_backbuffer(
		bool const enable
	) noexcept {
		m_states.set(State::retain_backbuffer, enable);
	}
/// @}

/** @name Input control */ /// @{
	/**
		Set caret position.

		@note The caret is not made visible by this function.

		@param x X position.
		@param y Y position.
	*/
	void
	set_caret_pos(
		geom_value_type x,
		geom_value_type y
	);

	/**
		Get caret position.
	*/
	Vec2 const&
	get_caret_pos() const noexcept {
		return m_caret_pos;
	}

	/**
		Set caret visibility.

		@param visible Whether the caret is visible.
	*/
	void
	set_caret_visible(
		bool const visible
	);

	/**
		Check if the caret is visible.
	*/
	bool
	is_caret_visible() const noexcept {
		return m_states.test(State::caret_visible);
	}
/// @}

/** @name Rendering */ /// @{
	/**
		Put a cell on the back buffer.

		@note If @a x or @a y are out-of-bounds, this function will
		fail silently.

		@param x X position.
		@param y Y position.
		@param cell %Cell to put.
	*/
	void
	put_cell(
		geom_value_type const x,
		geom_value_type const y,
		tty::Cell const& cell
	) noexcept;

	/**
		Put a string on the back buffer.

		@note If @a x or @a y are out-of-bounds, this function will
		fail silently.

		@param x X position.
		@param y Y position.
		@param seq %Sequence to put.
		@param points Number of code points to put.
		@param attr_fg Foreground attributes.
		@param attr_bg Background attributes.
	*/
	void
	put_sequence(
		geom_value_type x,
		geom_value_type const y,
		txt::Sequence const& seq,
		std::size_t const points,
		tty::attr_type const attr_fg = tty::Color::term_default,
		tty::attr_type const attr_bg = tty::Color::term_default
	) noexcept;

	/**
		Put a line on the back buffer.

		@note Out-of-bounds areas in the rectangle are not rendered.

		@param position Position.
		@param length Length of the line.
		@param direction Direction of the line.
		@param cell %Cell to put.
	*/
	void
	put_line(
		Vec2 position,
		geom_value_type const length,
		Axis const direction,
		Cell const& cell
	) noexcept;

	/**
		Put a rectangle on the back buffer.

		@note Out-of-bounds areas in the rectangle are not rendered.

		@param rect Rectangle bounds.
		@param frame Rectangle frame. Clockwise from the top-left:
		-# top-left corner
		-# top border
		-# top-right corner
		-# right border
		-# bottom-right corner
		-# bottom border
		-# bottom-left corner
		-# left border
		@param attr_fg Foreground attributes.
		@param attr_bg Background attributes.
	*/
	void
	put_rect(
		Rect const& rect,
		txt::UTF8Block const (&frame)[8u],
		tty::attr_type const attr_fg = tty::Color::term_default,
		tty::attr_type const attr_bg = tty::Color::term_default
	) noexcept;

	/**
		Write changes in the back buffer to the front buffer.
	*/
	void
	present();

	/**
		Clear the front buffer.

		@note This immediately clears the front buffer (i.e., the
		terminal screen itself).

		@remarks To clear both buffers, @c clear_front(true) is
		cheaper than <code>clear_back(); present();</code>.

		@param clear_back Whether to also clear the back buffer.
	*/
	void
	clear_front(
		bool const clear_back
	);

	/**
		Clear the back buffer.

		@param cell Fill cell.
	*/
	void
	clear_back(
		tty::Cell const& cell = tty::s_cell_default
	) noexcept;

	/**
		Clear a rectangle of the back buffer.

		@param rect Rectangle to clear.
		@param cell Fill cell.
	*/
	void
	clear_back(
		Rect const& rect,
		tty::Cell const& cell = tty::s_cell_default
	) noexcept;
/// @}

/** @name Events */ /// @{
	/**
		Poll for an event.

		@returns The event type, or @c tty::EventType::none if no
		event is available.
		@param[out] event %Event object to store the result.
		@param input_timeout Input polling timeout in milliseconds.
	*/
	tty::EventType
	poll(
		tty::Event& event,
		unsigned const input_timeout
	);
/// @}

/** @name Operations */ /// @{
	/**
		Update cached information.

		@note This primarily caches information from
		tty::TerminalInfo. It should be called if the info object is
		modified without calling set_info().
	*/
	void
	update_cache();

	/**
		Open terminal from device path.

		@remarks See also @c ttyname() and @c ctermid().

		@throws Error{...}
		See open(tty::fd_type const, bool const).

		@throws Error{ErrorCode::tty_device_open_failed}
		If @a tty_path could not be opened.

		@param tty_path Path to terminal device (usually
		just <code>/dev/tty</code>).
		@param use_sigwinch Whether to register @c SIGWINCH signal
		handler for resize events.

		@sa open(tty::fd_type const, bool const)
	*/
	void
	open(
		String const& tty_path,
		bool const use_sigwinch
	);

	/**
		Open terminal with file descriptor.

		@note @a tty_fd should be opened with @c O_RDWR.

		@warning If this call fails, the callee retains ownership
		of @a tty_fd.

		@warning The user should not overwrite the @c SIGWINCH
		handler while an open terminal owns the handler.

		@post On error, @c !is_open() and terminal does not own
		@c SIGWINCH handler.

		@post On success, @c is_open()==true and terminal
		owns @a tty_fd and the @c SIGWINCH signal handler
		(if @c use_sigwinch==true).

		@throws Error{ErrorCode::tty_terminal_already_open}
		If the terminal is already open.

		@throws Error{ErrorCode::tty_terminal_info_uninitialized}
		If the terminal info is uninitialized.

		@throws Error{ErrorCode::tty_invalid_fd}
		If @a tty_fd does not refer to a terminal or is otherwise
		invalid.

		@throws Error{ErrorCode::tty_sigaction_failed}
		If @a use_sigwinch is @c true and a @c sigaction() call
		failed.

		@throws Error{ErrorCode::tty_sigwinch_handler_already_active}
		If @a use_sigwinch is @c true and another terminal already
		owns the @c SIGWINCH signal handler.

		@throws Error{ErrorCode::tty_init_failed}
		If terminal setup failed.

		@param tty_fd %Terminal file descriptor.
		@param use_sigwinch Whether to register @c SIGWINCH signal
		handler for resize events.
	*/
	void
	open(
		tty::fd_type const tty_fd,
		bool const use_sigwinch
	);

	/**
		Close the terminal.
	*/
	void
	close() noexcept;

	/**
		Update the size of the terminal.

		@note This will not trigger a resize event.

		@remarks This can be used to query and update to the actual
		terminal size if the @c SIGWINCH handler is not enabled.

		@returns @c true if the terminal size changed.
	*/
	bool
	update_size();
/// @}
};

/** @} */ // end of doc-group tty

} // namespace tty
} // namespace Beard
