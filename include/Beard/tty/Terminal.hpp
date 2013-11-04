/**
@file tty/Terminal.hpp
@brief %Terminal class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_TTY_TERMINAL_HPP_
#define BEARD_TTY_TERMINAL_HPP_

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/utility.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/TerminalInfo.hpp>

#include <duct/cc_unique_ptr.hpp>
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
	}

	moveable_ostream&
	operator=(
		moveable_ostream&& other
	) {
		base::operator=(std::move(other));
		this->set_rdbuf(other.rdbuf());
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
	}

	moveable_istream&
	operator=(
		moveable_istream&& other
	) {
		base::operator=(std::move(other));
		this->set_rdbuf(other.rdbuf());
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

	enum class CapCache : unsigned {
		clear_screen = 0u,

		cursor_invisible,
		cursor_visible,

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

	void
	put_cap_cache(
		CapCache const cap
	);

	bool
	resize(
		unsigned const new_width,
		unsigned const new_height
	);

	void
	clear(
		bool const clear_frontbuffer
	);

	void
	init();

	void
	deinit();

private:
	duct::cc_unique_ptr<terminal_private> m_tty_priv;
	tty::fd_type m_tty_fd{tty::FD_INVALID};
	tty::TerminalInfo m_info{};

	String m_cap_cache[enum_cast(Terminal::CapCache::COUNT)]{};
	unsigned m_cap_max_colors{8u};

	duct::IO::dynamic_streambuf m_streambuf_in {0x80, 0u, 0x80};
	duct::IO::dynamic_streambuf m_streambuf_out{0x800};
	moveable_istream m_stream_in {m_streambuf_in};
	moveable_ostream m_stream_out{m_streambuf_out};

	unsigned m_tty_width{0u};
	unsigned m_tty_height{0u};

	bool m_caret_visible{false};
	unsigned m_caret_x{0};
	unsigned m_caret_y{0};

	unsigned m_attr_fg_last{tty::Color::term_default};
	unsigned m_attr_bg_last{tty::Color::term_default};

	aux::vector<bool> m_dirty_rows{};
	cell_vector_type m_cell_backbuffer{};
	cell_vector_type m_cell_frontbuffer{};

	struct {
		struct {
			bool pending{false};
			unsigned old_width{0u};
			unsigned old_height{0u};
		} resize;
	} ev_pending{};

	Terminal(Terminal const&) = delete;
	Terminal& operator=(Terminal const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Terminal() noexcept;

	/** Default constructor. */
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
		Get terminal width.
	*/
	unsigned
	get_width() const noexcept {
		return m_tty_width;
	}

	/**
		Get terminal height.
	*/
	unsigned
	get_height() const noexcept {
		return m_tty_height;
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
		Set terminal info.

		@param term_info %Terminal info.
	*/
	void
	set_info(
		tty::TerminalInfo term_info
	) noexcept {
		m_info = std::move(term_info);
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
		unsigned const x,
		unsigned const y
	);

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
		return m_caret_visible;
	}
/// @}

/** @name Rendering */ /// @{
	/**
		Put a cell on the backbuffer.

		@note If @a x or @a y are out-of-bounds, this function will
		fail silently.

		@param x X position.
		@param y Y position.
		@param cell %Cell to put.
	*/
	void
	put_cell(
		unsigned const x,
		unsigned const y,
		tty::Cell const& cell
	) noexcept;

	/**
		Put a string on the backbuffer.

		@note If @a x or @a y are out-of-bounds, this function will
		fail silently.

		@param x X position.
		@param y Y position.
		@param seq %Sequence to put.
		@param attr_fg Foreground attributes.
		@param attr_bg Background attributes.
	*/
	void
	put_sequence(
		unsigned x,
		unsigned const y,
		tty::Sequence const& seq,
		uint16_t const attr_fg = tty::Color::term_default,
		uint16_t const attr_bg = tty::Color::term_default
	) noexcept;

	/**
		Flip buffers and render cells.
	*/
	void
	render();
/// @}

/** @name Events */ /// @{
	/**
		Poll for an event.

		@returns The event type, or @c tty::EventType::none if no
		event is available.
		@param[out] event %Event object to store the result.
	*/
	tty::EventType
	poll(
		tty::Event& event
	);
/// @}

/** @name Operations */ /// @{
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

		@note This can be used to query and update to the actual
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

#endif // BEARD_TTY_TERMINAL_HPP_
