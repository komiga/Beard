/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Base widget class.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Geom.hpp>
#include <Beard/ui/Signal.hpp>

#include <duct/StateStore.hpp>

#include <utility>
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
	friend class ui::Root;

public:
	/**
		Signal for filtering events.

		This is called before handle_event_impl() and bypasses it if
		the signal function returns @c true.

		Parameters:

		-# The widget.
		-# The event.
	*/
	ui::Signal<bool(
		ui::Widget::SPtr,
		ui::Event const&
	)> signal_event_filter;

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

	ui::Widget::Type const m_type;
	flag_store_type m_flags;
	ui::index_type m_depth;
	ui::index_type m_index{0};
	ui::group_hash_type m_group;
	ui::Geom m_geometry;
	ui::RootWPtr m_root;
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
		set_input_control() implementation.

		%Base definition assigns Widget::Flags::input_control.
	*/
	virtual void
	set_input_control_impl(
		bool const enabled
	) noexcept;

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

	/**
		num_children() implementation.

		%Base definition returns 0.
	*/
	virtual signed
	num_children_impl() const noexcept;

	/**
		get_child() implementation.

		%Base definition returns nullptr.
	*/
	virtual ui::Widget::SPtr
	get_child_impl(
		ui::index_type const index
	);
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

		@param type %Widget type.
		@param flags Flags.
		@param group Property group name.
		@param geometry Geometry.
		@param root %Root.
		@param parent Parent.
	*/
	explicit
	Base(
		ui::Widget::Type const type,
		ui::Widget::Flags const flags,
		ui::group_hash_type const group,
		ui::Geom&& geometry,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent
	) noexcept
		: m_type(type)
		, m_flags(flags)
		, m_depth(type == ui::Widget::Type::Root ? -1 : 0)
		, m_group(group)
		, m_geometry(std::move(geometry))
		, m_root(std::move(root))
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
		Get type.
	*/
	ui::Widget::Type
	get_type() const noexcept {
		return m_type;
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

private:
	void
	update_depth(
		ui::Widget::SPtr const& parent
	) noexcept;

public:

	/**
		Set parent.
	*/
	void
	set_parent(
		ui::Widget::SPtr const& widget
	) noexcept;

	void
	set_parent(
		ui::Widget::WPtr const& widget
	) noexcept {
		set_parent(widget.lock());
	}
	/** @} */

	/**
		Set parent and index.
	*/
	void
	set_parent(
		ui::Widget::WPtr const& widget,
		unsigned const index
	) noexcept {
		set_parent(widget);
		set_index(index);
	}

	/**
		Set parent and index.
	*/
	void
	set_parent(
		ui::Widget::SPtr const& widget,
		unsigned const index
	) noexcept {
		set_parent(widget);
		set_index(index);
	}

	/**
		Clear parent and reset index.
	*/
	void
	clear_parent() noexcept {
		set_parent(ui::Widget::WPtr(), 0);
	}

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
		Check if the widget is focusable.

		@param and_visible Whether to also check visibility.
	*/
	bool
	is_focusable(
		bool const and_visible = false
	) const noexcept {
		return m_flags.test(
			ui::Widget::Flags::trait_focusable | (
				and_visible
				? ui::Widget::Flags::visible
				: ui::Widget::Flags::none
			)
		);
	}

	/**
		Check if the widget is a container.
	*/
	bool
	is_container() const noexcept {
		return m_flags.test(ui::Widget::Flags::trait_container);
	}

	/**
		Show or hide the widget.
	*/
	void
	set_visible(
		bool const visible
	) noexcept;

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
		set_input_control_impl(enabled);
	}

	/**
		Check if the widget is in input-control mode.
	*/
	bool
	has_input_control() const noexcept {
		return m_flags.test(ui::Widget::Flags::input_control);
	}

	/**
		Get depth of widget.
	*/
	ui::index_type
	get_depth() const noexcept {
		return m_depth;
	}

	/**
		Set index of widget in parent.
	*/
	void
	set_index(
		ui::index_type const index
	) {
		m_index = max_ce(0, index);
	}

	/**
		Get index of widget in parent.
	*/
	ui::index_type
	get_index() const noexcept {
		return m_index;
	}

	/**
		Get the number of children.

		@note This returns 0 if the widget is not a container.
	*/
	signed
	num_children() const noexcept {
		return is_container() ? num_children_impl() : 0;
	}

	/**
		Check if widget has children.
	*/
	bool
	has_children() const noexcept {
		return 0 < num_children();
	}

	/**
		Get child by index.

		@note This returns nullptr if the widget is not a container.
	*/
	ui::Widget::SPtr
	get_child(
		ui::index_type const index
	) {
		return is_container() ? get_child_impl(max_ce(0, index)) : nullptr;
	}

	/**
		Get last child index.
	*/
	ui::index_type
	get_last_child_index() const noexcept {
		signed const num = num_children_impl();
		return max_ce(0, num - 1);
	}

	/**
		Get last child.
	*/
	ui::Widget::SPtr
	get_last_child() noexcept {
		signed const num = num_children_impl();
		return 0 < num ? get_child_impl(num - 1) : nullptr;
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
	) noexcept;

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
