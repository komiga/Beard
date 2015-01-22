/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Widget packing operations.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Geom.hpp>

namespace Beard {
namespace ui {

// Forward declarations

/**
	@addtogroup ui
	@{
*/

/**
	Reflow widget geometry.

	@param geom Geometry to reflow.
*/
void
reflow(
	ui::Geom& geom
) noexcept;

/**
	Reflow widget geometry into an area.

	@param geom Geometry to reflow.
	@param area New widget area.
*/
void
reflow_into(
	ui::Geom& geom,
	Rect const& area
) noexcept;

/**
	Reflow slots.

	@note This will assign widget areas, but not reflow the widgets
	or their geometries to avoid compound re-reflowing through the
	action queue. The parent widget should push the slot widgets from
	@c ui::Widget::push_action_graph_impl() if they are visible.

	@param area Available area.
	@param slots %Slots.
	@param axis %Axis to pack along.
*/
void
reflow_slots(
	Rect const& area,
	ui::Widget::slot_vector_type& slots,
	Axis const axis
) noexcept;

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
