/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/tty/Defs.hpp>
#include <Beard/ui/debug.hpp>

namespace Beard {
namespace ui {

void
geom_debug_render(
	tty::Terminal& term,
	ui::Geom const& geometry,
	tty::attr_type const attr_bg,
	bool const focused
) noexcept {
	term.put_rect(
		geometry.area(),
		focused
			? tty::s_frame_single_heavy
			: tty::s_frame_single
		,
		focused
			? tty::Color::white
			: tty::Color::red
		,
		attr_bg
	);
	term.put_rect(
		geometry.frame(),
		focused
			? tty::s_frame_single_heavy
			: tty::s_frame_single
		,
		focused
			? tty::Color::white
			: tty::Color::blue
		,
		attr_bg
	);
}

} // namespace ui
} // namespace Beard
