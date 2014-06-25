/**
@file ui/packing.hpp
@brief %Widget packing operations.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
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

	@param area Available area.
	@param geom Geometry to reflow.
*/
void
reflow(
	Rect const& area,
	ui::Geom& geom
) noexcept;

/**
	Reflow slots.

	@param area Available area.
	@param slots %Slots.
	@param axis %Axis to pack along.
	@param cache_geometry Whether to cache widget geometry.
*/
void
reflow_slots(
	Rect const& area,
	ui::Widget::slot_vector_type& slots,
	Axis const axis,
	bool const cache_geometry
) noexcept;

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

