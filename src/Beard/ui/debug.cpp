
#include <Beard/tty/Defs.hpp>
#include <Beard/ui/debug.hpp>

namespace Beard {
namespace ui {

namespace {
static tty::UTF8Block const
s_frame_single[]{
	{U'┏'},
	{U'━'},
	{U'┓'},
	{U'┃'},
	{U'┛'},
	{U'━'},
	{U'┗'},
	{U'┃'}
};

static tty::UTF8Block const
s_frame_double[]{
	{U'╔'},
	{U'═'},
	{U'╗'},
	{U'║'},
	{U'╝'},
	{U'═'},
	{U'╚'},
	{U'║'}
};
} // anonymous namespace

void
geom_debug_render(
	tty::Terminal& term,
	ui::Geom const& geometry,
	uint16_t const attr_bg,
	bool const focused
) noexcept {
	term.put_rect(
		geometry.get_area(),
		s_frame_single,
		focused
			? tty::Color::white
			: tty::Color::red
		,
		attr_bg
	);
	term.put_rect(
		geometry.get_frame(),
		s_frame_single,
		focused
			? tty::Color::white
			: tty::Color::blue
		,
		attr_bg
	);
}

} // namespace ui
} // namespace Beard
