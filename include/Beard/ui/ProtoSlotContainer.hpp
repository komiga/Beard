/**
@file ui/ProtoSlotContainer.hpp
@brief Prototype slot-based container widget.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>

#include <utility>

namespace Beard {
namespace ui {

// Forward declarations
class ProtoSlotContainer;

/**
	@addtogroup ui
	@{
*/

/**
	Prototype slot-based container widget.
*/
class ProtoSlotContainer
	: public ui::Widget::Base
{
private:
	using base_type = ui::Widget::Base;

protected:
	/** Orientation. */
	Axis m_orientation;
	/** %Slots. */
	ui::Widget::slot_vector_type m_slots;

private:
	ProtoSlotContainer() noexcept = delete;
	ProtoSlotContainer(ProtoSlotContainer const&) = delete;
	ProtoSlotContainer& operator=(ProtoSlotContainer const&) = delete;

protected:
	virtual void
	cache_geometry_impl() noexcept override;

	virtual void
	reflow_impl(
		Rect const& area,
		bool const cache
	) noexcept override;

	virtual void
	render_impl(
		ui::Widget::RenderData& rd
	) noexcept override;

	signed
	num_children_impl() const noexcept override;

	ui::Widget::SPtr
	get_child_impl(
		ui::index_type const index
	) override;

protected:
/** @name Constructors and destructor */ /// @{
	/**
		Destructor.

		@note This will explicitly @c clear() the container.
	*/
	virtual
	~ProtoSlotContainer() noexcept override = 0;

	/**
		Constructor.

		@param type %Widget type.
		@param flags Flags.
		@param group Property group name.
		@param geometry Geometry.
		@param root %Root.
		@param parent Parent.
		@param orientation Orientation.
		@param slot_count Number of slots to reserve.
	*/
	ProtoSlotContainer(
		ui::Widget::Type const type,
		ui::Widget::Flags const flags,
		ui::group_hash_type const group,
		ui::Geom&& geometry,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent,
		Axis const orientation,
		std::size_t const slot_count
	) noexcept
		: base_type(
			type,
			enum_combine(
				ui::Widget::Flags::trait_container,
				flags
			),
			group,
			std::move(geometry),
			std::move(root),
			std::move(parent)
		)
		, m_orientation(orientation)
		, m_slots(slot_count)
	{}

	/** Move constructor. */
	ProtoSlotContainer(ProtoSlotContainer&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	ProtoSlotContainer& operator=(ProtoSlotContainer&&) = default;
/// @}

public:
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
	ui::Widget::slot_vector_type&
	get_slots() noexcept {
		return m_slots;
	}

	/**
		Get slots.
	*/
	ui::Widget::slot_vector_type const&
	get_slots() const noexcept {
		return m_slots;
	}
/// @}

/** @name Collection */ /// @{
	/**
		Remove all widgets.
	*/
	void
	clear();

	/**
		Set child by index.

		@throws std::out_of_range
		If @a index is out of bounds.

		@param index Index.
		@param widget %Widget.
	*/
	void
	set_child(
		ui::index_type const index,
		ui::Widget::SPtr widget
	);

	/**
		Add a widget to the end of the container.

		@param widget %Widget to add.
	*/
	void
	push_back(
		ui::Widget::SPtr widget
	);
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
