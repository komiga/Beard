/**
@file ui/Context.hpp
@brief UI context.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_CONTEXT_HPP_
#define BEARD_UI_CONTEXT_HPP_

#include <Beard/config.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Defs.hpp>

#include <utility>

namespace Beard {
namespace ui {

// Forward declarations
class Context;

/**
	@addtogroup ui
	@{
*/

/**
	UI context.
*/
class Context final {
private:
	tty::Terminal m_terminal;
	ui::Event m_event;

	ui::WidgetSPtr m_root;
	ui::WidgetWPtr m_focused;

	Context(Context const&) = delete;
	Context& operator=(Context const&) = delete;

	bool
	push_event(
		ui::Event const& event,
		ui::WidgetSPtr widget
	) noexcept;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Context() noexcept;

	/** Default constructor. */
	Context();

	/**
		Constructor with terminal info.

		@param term_info %Terminal info.
	*/
	Context(
		tty::TerminalInfo term_info
	);

	/** Move constructor. */
	Context(Context&&);
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Context& operator=(Context&&);
/// @}

/** @name Properties */ /// @{
	/**
		Get terminal (mutable).
	*/
	tty::Terminal&
	get_terminal() noexcept {
		return m_terminal;
	}

	/**
		Get terminal.
	*/
	tty::Terminal const&
	get_terminal() const noexcept {
		return m_terminal;
	}

	/**
		Set root widget.

		@param root New root widget.
	*/
	void
	set_root(
		ui::WidgetSPtr root
	) noexcept {
		m_root = std::move(root);
	}

	/**
		Get root widget (mutable).
	*/
	ui::WidgetSPtr&
	get_root() noexcept {
		return m_root;
	}

	/**
		Get root widget.
	*/
	ui::WidgetSPtr const&
	get_root() const noexcept {
		return m_root;
	}

	/**
		Get focused widget.
	*/
	ui::WidgetSPtr
	get_focused() noexcept {
		return m_focused.lock();
	}

	/**
		Check if there is a focused widget.
	*/
	bool
	has_focused() const noexcept {
		return !m_focused.expired();
	}
/// @}

/** @name Operations */ /// @{
	/**
		Open terminal and start UI control.

		@throws Error{ErrorCode::tty_...}
		See tty::Terminal::open(String const&, bool const) and
		tty::Terminal::open(tty::fd_type const, bool const).
	*/
	void
	open(
		String const& tty_path,
		bool const use_sigwinch
	);

	/**
		Close terminal and stop UI control.
	*/
	void
	close() noexcept;

	/**
		Poll for events and update widgets.

		@param input_timeout Input polling timeout in milliseconds.
	*/
	bool
	update(
		unsigned const input_timeout
	);

	/**
		Reflow root widget.
	*/
	void
	reflow() noexcept;

	/**
		Render.

		@param reflow Whether to reflow the root widget.
	*/
	void
	render(
		bool const reflow
	);
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_CONTEXT_HPP_
