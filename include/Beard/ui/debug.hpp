/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Debug operations.
*/

#pragma once

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
	@param focused Whether to highlight the frame.
*/
void
geom_debug_render(
	tty::Terminal& term,
	ui::Geom const& geometry,
	tty::attr_type const attr_bg = tty::Color::term_default,
	bool const focused = false
) noexcept;

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
