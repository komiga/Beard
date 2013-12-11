/**
@file ui/Widget.hpp
@brief Base widget class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_WIDGET_HPP_
#define BEARD_UI_WIDGET_HPP_

#include <Beard/config.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Geom.hpp>
#include <Beard/ui/debug.hpp>

#include <duct/StateStore.hpp>

#include <memory>
#include <functional>

namespace Beard {
namespace ui {

// Forward declarations
class Context; // external
class Widget;

/**
	@addtogroup ui
	@{
*/

/**
	Base widget class.
*/
class Widget
	: public std::enable_shared_from_this<Widget>
{
protected:
	/**
		%Flags.
	*/
	enum class Flags : unsigned {
		/**
			%Widget is enabled.
		*/
		enabled			= 1u << 0,
		/**
			%Widget is visible.
		*/
		visible			= 1u << 1,
	};

	/**
		Flag store type.
	*/
	using flag_store_type = duct::StateStore<Widget::Flags>;

private:
	ui::WidgetType m_type;
	flag_store_type m_flags;
	std::reference_wrapper<ui::Context> m_context;
	ui::WidgetWPtr m_parent;
	ui::Geom m_geometry;

	Widget() = delete;
	Widget(Widget const&) = delete;
	Widget& operator=(Widget const&) = delete;

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
		cache_geometry() implementation.

		Base definition does nothing.
	*/
	virtual void
	cache_geometry_impl() noexcept;

	/**
		reflow() implementation.

		Base definition generically reflows geometry.
	*/
	virtual void
	reflow_impl(
		Rect const& area
	) noexcept;

	/**
		handle_event() implementation.

		Base definition returns @c false.
	*/
	virtual bool
	handle_event_impl(
		ui::Event const& event
	) noexcept;

	/**
		render() implementation.

		Base definition does nothing.
	*/
	virtual void
	render_impl() noexcept;
/// @}

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	virtual
	~Widget() noexcept = 0;

protected:
	/** Move constructor. */
	Widget(Widget&&) noexcept = default;

	/**
		Constructor with properties.

		@param type Type.
		@param flags Flags.
		@param context Context.
		@param parent Parent.
		@param geometry Geometry.
	*/
	explicit
	Widget(
		ui::WidgetType const type,
		Widget::Flags const flags,
		ui::Context& context,
		ui::WidgetWPtr parent,
		ui::Geom geometry
	) noexcept
		: m_type(type)
		, m_flags(flags)
		, m_context(context)
		, m_parent(parent)
		, m_geometry(std::move(geometry))
	{}
/// @}

/** @name Operators */ /// @{
protected:
	/** Move assignment operator. */
	Widget& operator=(Widget&&) noexcept = default;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type.
	*/
	ui::WidgetType
	get_type() const noexcept {
		return m_type;
	}

	/**
		Get context.
	*/
	ui::Context&
	get_context() const noexcept {
		return m_context;
	}

	/**
		Set parent.

		@param widget New parent.
	*/
	void
	set_parent(
		ui::WidgetSPtr const& widget
	) noexcept {
		m_parent = widget;
	}

	void
	set_parent(
		ui::WidgetWPtr const& widget
	) noexcept {
		m_parent = widget;
	}
	/** @} */

	/**
		Get parent.

		@note The return value will be empty (@c nullptr) if either
		there is no parent or the parent was destroyed.
	*/
	ui::WidgetSPtr
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

		@param geometry New geometry.
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
		Show or hide the widget.

		@param visible Whether to make the widget visible or
		invisible.
	*/
	void
	set_visible(
		bool const visible
	) noexcept {
		m_flags.set(Widget::Flags::visible, visible);
	}

	/**
		Check if the widget is visible.
	*/
	bool
	is_visible() const noexcept {
		return m_flags.test(Widget::Flags::visible);
	}

	/**
		Enable or disable the widget.

		@param enabled Whether to enable or disable the widget.
	*/
	void
	set_enabled(
		bool const enabled
	) noexcept {
		m_flags.set(Widget::Flags::enabled, enabled);
	}

	/**
		Check if the widget is enabled.
	*/
	bool
	is_enabled() const noexcept {
		return m_flags.test(Widget::Flags::enabled);
	}
/// @}

/** @name Operations */ /// @{
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
	*/
	void
	reflow(
		Rect const& area
	) noexcept {
		reflow_impl(area);
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
	*/
	void
	render() noexcept;
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_WIDGET_HPP_
