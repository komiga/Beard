/**
@file ui/packing.hpp
@brief %Widget packing operations.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_PACKING_HPP_
#define BEARD_UI_PACKING_HPP_

#include <Beard/config.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
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
	ui::slot_vector_type& slots,
	Axis const axis,
	bool const cache_geometry
) noexcept;

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_PACKING_HPP_
