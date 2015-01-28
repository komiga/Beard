/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Prototype slot-based container widget.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
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

public:
	/**
		Shared pointer.
	*/
	using SPtr = aux::shared_ptr<ui::ProtoSlotContainer>;

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
	push_action_graph_impl(
		ui::Widget::set_type& set
	) noexcept override;

	virtual void
	cache_geometry_impl() noexcept override;

	virtual void
	reflow_impl() noexcept override;

	signed
	num_children_impl() const noexcept override;

	ui::Widget::SPtr
	child_at_impl(
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
	*/
	ProtoSlotContainer(
		ui::Widget::Type const type,
		ui::Widget::Flags const flags,
		ui::group_hash_type const group,
		ui::Geom&& geometry,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent,
		Axis const orientation
	) noexcept
		: base_type(
			type,
				ui::Widget::Flags::trait_container |
				flags
			,
			group,
			std::move(geometry),
			std::move(root),
			std::move(parent)
		)
		, m_orientation(orientation)
		, m_slots()
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
	orientation() const noexcept {
		return m_orientation;
	}

	/**
		Get slots (mutable).
	*/
	ui::Widget::slot_vector_type&
	slots() noexcept {
		return m_slots;
	}

	/**
		Get slots.
	*/
	ui::Widget::slot_vector_type const&
	slots() const noexcept {
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

		@throws Error{ErrorCode::ui_container_null_widget}
		If @a widget is null.

		@param index Index.
		@param widget %Widget.
	*/
	void
	set_child(
		ui::index_type const index,
		ui::Widget::SPtr widget
	);

	/**
		Remove child by index.

		@param index Index to remove.
	*/
	void
	remove(
		ui::index_type index
	) noexcept;

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
