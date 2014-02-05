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
#include <Beard/aux.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/PropertyGroup.hpp>
#include <Beard/ui/PropertyMap.hpp>

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
	friend class ui::Widget::Base;

private:
	using action_queue_set_type = aux::set<
		ui::Widget::WPtr,
		aux::owner_less<ui::Widget::WPtr>
	>;

	tty::Terminal m_terminal;
	ui::Event m_event;

	ui::PropertyMap m_property_map;
	ui::group_hash_type m_fallback_group{ui::group_default};
	action_queue_set_type m_action_queue;
	ui::RootSPtr m_root;

	Context(Context const&) = delete;
	Context& operator=(Context const&) = delete;

	bool
	push_event(
		ui::Event const& event,
		ui::Widget::SPtr widget
	) noexcept;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Context() noexcept;

	/** Default constructor. */
	Context();

	/**
		Constructor with property map.

		@param property_map %Property map.
	*/
	Context(
		ui::PropertyMap property_map
	);

	/**
		Constructor with terminal info and property map.

		@param term_info %Terminal info.
		@param property_map %Property map.
	*/
	Context(
		tty::TerminalInfo term_info,
		ui::PropertyMap property_map = ui::PropertyMap{true}
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
		Set property map.
	*/
	void
	set_property_map(
		ui::PropertyMap property_map
	) {
		m_property_map = std::move(property_map);
	}

	/**
		Get property map (mutable).
	*/
	ui::PropertyMap&
	get_property_map() noexcept {
		return m_property_map;
	}

	/**
		Get property map.
	*/
	ui::PropertyMap const&
	get_property_map() const noexcept {
		return m_property_map;
	}

	/**
		Set fallback property group.
	*/
	void
	set_fallback_group(
		ui::group_hash_type const fallback_group
	) noexcept {
		m_fallback_group = fallback_group;
	}

	/**
		Get fallback property group.
	*/
	ui::group_hash_type
	get_fallback_group() const noexcept {
		return m_fallback_group;
	}

	/**
		Set root.
	*/
	void
	set_root(
		ui::RootSPtr root
	) noexcept {
		m_root = std::move(root);
	}

	/**
		Get root.
	*/
	ui::RootSPtr const&
	get_root() const noexcept {
		return m_root;
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

/** @name Update queue */ /// @{
private:
	ui::UpdateActions
	run_actions(
		ui::Widget::RenderData& rd,
		ui::Widget::SPtr widget,
		ui::UpdateActions const mask
	);

	void
	run_all_actions();

	void
	enqueue_widget(
		ui::Widget::SPtr const& widget
	);

	void
	dequeue_widget(
		ui::Widget::SPtr const& widget
	);

public:
	/**
		Unqueue and clear all update actions.
	*/
	void
	clear_actions();
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_CONTEXT_HPP_
