/**
@file ui/Widget/Base.hpp
@brief %Base widget class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_WIDGET_HPP_
#define BEARD_UI_WIDGET_HPP_

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Geom.hpp>

#include <duct/StateStore.hpp>

#include <memory>
#include <functional>

namespace Beard {
namespace ui {

// Forward declarations
class Root; // external

namespace Widget {

// Forward declarations
class Base;

/**
	@addtogroup ui
	@{
*/

/**
	%Base widget class.
*/
class Base
	: public aux::enable_shared_from_this<Base>
{
public:
	friend class ui::Root;

protected:
	/**
		Flag store type.
	*/
	using flag_store_type = duct::StateStore<ui::Widget::Flags>;

private:
	enum : unsigned {
		shift_ua = enum_cast(ui::Widget::Flags::COUNT)
	};

	static constexpr ui::Widget::Flags const
	mask_ua = static_cast<ui::Widget::Flags>(
		enum_cast(ui::UpdateActions::mask_all) << shift_ua
	);

	ui::RootWPtr m_root;
	flag_store_type m_flags;
	ui::group_hash_type m_group;
	ui::focus_index_type m_focus_index;
	ui::Geom m_geometry;
	ui::Widget::WPtr m_parent;

	Base() = delete;
	Base(Base const&) = delete;
	Base& operator=(Base const&) = delete;

protected:
/** @name Internal state */ /// @{
	/**
		Get flag store (mutable).
	*/
	flag_store_type&
	get_flag_store() noexcept {
		return m_flags;
	}

	/**
		Get flag store.
	*/
	flag_store_type const&
	get_flag_store() const noexcept {
		return m_flags;
	}
/// @}

/** @name Implementation */ /// @{
	/**
		get_type_info() implementation.
	*/
	virtual ui::Widget::type_info const&
	get_type_info_impl() const noexcept = 0;

	/**
		cache_geometry() implementation.

		%Base definition does nothing.
	*/
	virtual void
	cache_geometry_impl() noexcept;

	/**
		reflow() implementation.

		%Base definition generically reflows geometry. If @a cache is
		@c true, cache_geometry() will be called before reflowing.
	*/
	virtual void
	reflow_impl(
		Rect const& area,
		bool const cache
	) noexcept;

	/**
		handle_event() implementation.

		%Base definition returns @c false.
	*/
	virtual bool
	handle_event_impl(
		ui::Event const& event
	) noexcept;

	/**
		render() implementation.

		%Base definition does nothing.
	*/
	virtual void
	render_impl(
		ui::Widget::RenderData& rd
	) noexcept;
/// @}

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	virtual
	~Base() noexcept = 0;

protected:
	/** Move constructor. */
	Base(Base&&) noexcept = default;

	/**
		Constructor with properties.

		@param root %Root.
		@param flags Flags.
		@param group Property group name.
		@param parent Parent.
		@param geometry Geometry.
	*/
	explicit
	Base(
		ui::RootWPtr&& root,
		ui::Widget::Flags const flags,
		ui::group_hash_type const group,
		ui::Geom&& geometry,
		ui::Widget::WPtr&& parent
	) noexcept
		: m_root(std::move(root))
		, m_flags(flags)
		, m_group(group)
		, m_focus_index(ui::focus_index_none)
		, m_geometry(std::move(geometry))
		, m_parent(std::move(parent))
	{}
/// @}

/** @name Operators */ /// @{
protected:
	/** Move assignment operator. */
	Base& operator=(Base&&) noexcept = default;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type information.
	*/
	ui::Widget::type_info const&
	get_type_info() const noexcept {
		return get_type_info_impl();
	}

	/**
		Get root.
	*/
	ui::RootSPtr
	get_root() const noexcept {
		return m_root.lock();
	}

	/**
		Get root (weak).
	*/
	ui::RootWPtr
	get_root_weak() const noexcept {
		return m_root;
	}

	/**
		Check if the root is valid.
	*/
	bool
	is_root_valid() const noexcept {
		return !m_root.expired();
	}

	/**
		Set property group.
	*/
	void
	set_group(
		ui::group_hash_type const group
	) noexcept {
		m_group = group;
	}

	/**
		Get property group.
	*/
	ui::group_hash_type
	get_group() const noexcept {
		return m_group;
	}

	/**
		Set parent.
	*/
	void
	set_parent(
		ui::Widget::SPtr const& widget
	) noexcept {
		m_parent = widget;
	}

	void
	set_parent(
		ui::Widget::WPtr const& widget
	) noexcept {
		m_parent = widget;
	}
	/** @} */

	/**
		Get parent.

		@note The return value will be empty (@c nullptr) if either
		there is no parent or the parent was destroyed.
	*/
	ui::Widget::SPtr
	get_parent() const noexcept {
		return m_parent.lock();
	}

	/**
		Check if the widget has a parent.
	*/
	bool
	has_parent() const noexcept {
		return !m_parent.expired();
	}

	/**
		Set geometry.
	*/
	void
	set_geometry(
		ui::Geom geometry
	) noexcept {
		m_geometry = std::move(geometry);
	}

	/**
		Get geometry (mutable).
	*/
	ui::Geom&
	get_geometry() noexcept {
		return m_geometry;
	}

	/**
		Get geometry.
	*/
	ui::Geom const&
	get_geometry() const noexcept {
		return m_geometry;
	}

	/**
		Set whether an action is queued for the widget.
	*/
	void
	set_action_queued(
		bool const queued
	) noexcept {
		m_flags.set(ui::Widget::Flags::queued_actions, queued);
	}

	/**
		Check if the widget has queued update actions.
	*/
	bool
	is_action_queued() const noexcept {
		return m_flags.test(ui::Widget::Flags::queued_actions);
	}

	/**
		Get queued update actions.
	*/
	ui::UpdateActions
	get_queued_actions() const noexcept {
		return static_cast<ui::UpdateActions>(
			enum_cast(m_flags.get_states(mask_ua)) >> shift_ua
		);
	}

	/**
		Show or hide the widget.
	*/
	void
	set_visible(
		bool const visible
	) noexcept {
		m_flags.set(ui::Widget::Flags::visible, visible);
	}

	/**
		Check if the widget is visible.
	*/
	bool
	is_visible() const noexcept {
		return m_flags.test(ui::Widget::Flags::visible);
	}

	/**
		Enable or disable the widget.
	*/
	void
	set_enabled(
		bool const enabled
	) noexcept {
		m_flags.set(ui::Widget::Flags::enabled, enabled);
	}

	/**
		Check if the widget is enabled.
	*/
	bool
	is_enabled() const noexcept {
		return m_flags.test(ui::Widget::Flags::enabled);
	}

	/**
		Set focused.

		@note This will call handle_event() with a
		ui::EventType::focus_changed if @a focused differs from the
		current value. If handle_event() returns @c false, a no-clear
		render will be queued.
	*/
	void
	set_focused(
		bool const focused
	) noexcept;

	/**
		Check if the widget is focused.
	*/
	bool
	is_focused() const noexcept {
		return m_flags.test(ui::Widget::Flags::focused);
	}

	/**
		Set input-control mode.
	*/
	void
	set_input_control(
		bool const enabled
	) noexcept {
		m_flags.set(ui::Widget::Flags::input_control, enabled);
	}

	/**
		Check if the widget is in input-control mode.
	*/
	bool
	has_input_control() const noexcept {
		return m_flags.test(ui::Widget::Flags::input_control);
	}

public:
	/**
		Set focus index.

		@note This will modify the root's focus map and should not be
		called from a constructor or destructor.
	*/
	void
	set_focus_index(
		ui::focus_index_type const index
	) noexcept;

	/**
		Get focus index.
	*/
	ui::focus_index_type
	get_focus_index() const noexcept {
		return m_focus_index;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Queue update actions.

		@note This has no effect if @a actions does not contain an
		action. See ui::UpdateActions::mask_actions.

		@note If a clearing render is either already queued or
		is being requested, ui::UpdateActions::flag_noclear is
		elided.

		@param actions Actions to queue.
	*/
	void
	queue_actions(
		ui::UpdateActions const actions
	);

	/**
		Clear all update actions.

		@note This will also clear the @c queued_action flag.

		@param dequeue Whether to dequeue the widget from its context.
	*/
	void
	clear_actions(
		bool const dequeue = true
	);

	/**
		Cache dynamic geometry.
	*/
	void
	cache_geometry() noexcept {
		cache_geometry_impl();
	}

	/**
		Rejigger the geometry of the widget and its associated
		widgets.

		@param area Area available to the widget.
		@param cache Whether to cache geometry.
	*/
	void
	reflow(
		Rect const& area,
		bool const cache
	) noexcept {
		reflow_impl(area, cache);
	}

	/**
		Handle an event.

		@returns Whether the event was handled.
		@param event %Event.
	*/
	bool
	handle_event(
		ui::Event const& event
	) noexcept {
		return handle_event_impl(event);
	}

	/**
		Render the widget.

		@param rd Render data.
	*/
	void
	render(
		ui::Widget::RenderData& rd
	) noexcept;
/// @}
};

/** @} */ // end of doc-group ui

} // namespace Widget
} // namespace ui
} // namespace Beard

#endif // BEARD_UI_WIDGET_HPP_
