/**
@file ui/Container.hpp
@brief %Container widget.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_CONTAINER_HPP_
#define BEARD_UI_CONTAINER_HPP_

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget.hpp>

#include <utility>
#include <memory>

namespace Beard {
namespace ui {

// Forward declarations
class Container;

/**
	@addtogroup ui
	@{
*/

/**
	%Widget container.
*/
class Container final
	: public ui::Widget
{
private:
	Axis m_orientation;
	ui::slot_vector_type m_slots;

	Container() noexcept = delete;
	Container(Container const&) = delete;
	Container& operator=(Container const&) = delete;

	void
	cache_geometry_impl() noexcept override;

	void
	reflow_impl(
		Rect const& area
	) noexcept override;

	void
	render_impl() noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Container() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Container(
		ui::Context& context,
		Axis const orientation,
		std::size_t const slot_count,
		ui::WidgetWPtr&& parent
	) noexcept
		: Widget(
			ui::WidgetType::Container,
			enum_combine(ui::Widget::Flags::visible),
			context,
			std::move(parent),
			{{0, 0}, Axis::both, Axis::both}
		)
		, m_orientation(orientation)
		, m_slots(slot_count)
	{}
	/** @endcond */ // INTERNAL

	/**
		Construct container.

		@throws std::bad_alloc
		If allocation fails.

		@param context Context.
		@param orientation Orientation.
		@param slot_count Number of slots to reserve.
		@param parent Parent.
	*/
	static std::shared_ptr<ui::Container>
	make(
		ui::Context& context,
		Axis const orientation,
		std::size_t const slot_count = 0u,
		ui::WidgetWPtr parent = ui::WidgetWPtr()
	) {
		return std::make_shared<ui::Container>(
			context, orientation, slot_count, std::move(parent)
		);
	}

	/** Move constructor. */
	Container(Container&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Container& operator=(Container&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set orientation.

		@param orientation Orientation.
	*/
	void
	set_orientation(
		Axis const orientation
	) noexcept {
		m_orientation = orientation;
	}

	/**
		Get orientation.
	*/
	Axis
	get_orientation() const noexcept {
		return m_orientation;
	}

	/**
		Get slots (mutable).
	*/
	ui::slot_vector_type&
	get_slots() noexcept {
		return m_slots;
	}

	/**
		Get slots.
	*/
	ui::slot_vector_type const&
	get_slots() const noexcept {
		return m_slots;
	}

	/**
		Get slot count.
	*/
	ui::slot_vector_type::size_type
	get_slot_count() const noexcept {
		return m_slots.size();
	}
/// @}

/** @name Slots */ /// @{
	/**
		Remove all slots.
	*/
	void
	clear() {
		m_slots.clear();
	}

	/**
		Set slot widget by index.

		@throws std::out_of_range
		If @a idx is out of bounds.

		@param idx %Slot index.
		@param widget %Widget.
	*/
	void
	set_slot_widget(
		ui::slot_vector_type::size_type const idx,
		ui::WidgetSPtr widget
	) {
		m_slots.at(idx).widget = std::move(widget);
	}

	/**
		Get slot widget by index.

		@throws std::out_of_range
		If @a idx is out of bounds.

		@param idx %Slot index.
	*/
	ui::WidgetSPtr
	get_slot_widget(
		ui::slot_vector_type::size_type const idx
	) {
		return m_slots.at(idx).widget;
	}

	/**
		Add a slot to the end of the container.

		@param widget %Widget to add.
	*/
	void
	push_back(
		ui::WidgetSPtr widget
	) {
		if (widget) {
			widget->set_parent(this->shared_from_this());
		}
		m_slots.push_back(ui::Slot{std::move(widget), {}});
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_CONTAINER_HPP_
