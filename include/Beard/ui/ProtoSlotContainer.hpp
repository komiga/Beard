/**
@file ui/ProtoSlotContainer.hpp
@brief Prototype slot-based container widget.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_PROTOSLOTCONTAINER_HPP_
#define BEARD_UI_PROTOSLOTCONTAINER_HPP_

#include <Beard/config.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>
#include <Beard/ui/ProtoContainer.hpp>

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
	: public ui::ProtoContainer
{
private:
	using base_type = ui::ProtoContainer;

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

	/**
		size() implementation.

		Base definition returns @c m_slots.size().
	*/
	std::size_t
	size_impl() const noexcept override final;

	/**
		clear() implementation.

		Base definition unsets the parents of all slot widgets
		and then calls @c m_slots.clear().
	*/
	virtual void
	clear_impl() override;

	/**
		get_widget() implementation.

		Base definition returns @c m_slots.at(idx).
	*/
	ui::Widget::SPtr
	get_widget_impl(
		std::size_t const idx
	) const override final;

	/**
		set_widget() implementation.

		Base definition unsets parent of current widget,
		sets slot widget to @a widget, then sets the parent of
		@a widget to the container.
	*/
	virtual void
	set_widget_impl(
		std::size_t const idx,
		ui::Widget::SPtr widget
	) override;

	/**
		push_back() implementation.

		Base definition calls @c m_slots.push_pack() and sets
		the parent of @a widget to the container.
	*/
	virtual void
	push_back_impl(
		ui::Widget::SPtr widget
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

		@param root %Root.
		@param flags Flags.
		@param group %Property group.
		@param geometry Geometry.
		@param parent Parent.
		@param orientation Orientation.
		@param slot_count Number of slots to reserve.
	*/
	ProtoSlotContainer(
		ui::RootWPtr&& root,
		ui::Widget::Flags const flags,
		ui::group_hash_type const group,
		ui::Geom&& geometry,
		ui::Widget::WPtr&& parent,
		Axis const orientation,
		std::size_t const slot_count
	) noexcept
		: base_type(
			std::move(root),
			flags,
			group,
			std::move(geometry),
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
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_PROTOSLOTCONTAINER_HPP_
