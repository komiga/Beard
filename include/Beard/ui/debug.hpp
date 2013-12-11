/**
@file ui/debug.hpp
@brief Debug operations.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_DEBUG_HPP_
#define BEARD_UI_DEBUG_HPP_

#include <Beard/config.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Geom.hpp>
#include <Beard/tty/Terminal.hpp>

namespace Beard {
namespace ui {

// Forward declarations

/**
	@addtogroup ui
	@{
*/

/**
	Debug-render geometry.

	@param term %Terminal.
	@param geometry Geometry.
	@param attr_bg Background attributes.
*/
void
geom_debug_render(
	tty::Terminal& term,
	ui::Geom const& geometry,
	uint16_t const attr_bg = tty::Color::term_default
) noexcept;

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_DEBUG_HPP_
