
#include <Beard/ui/packing.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Widget.hpp>

namespace Beard {
namespace ui {

// class Widget implementation

Widget::~Widget() noexcept = default;

// implementation

void
Widget::cache_geometry_impl() noexcept {
	/* Do nothing. */
}

void
Widget::reflow_impl(
	Rect const& area,
	bool const cache
) noexcept {
	if (cache) {
		cache_geometry();
	}
	ui::reflow(area, get_geometry());
}

bool
Widget::handle_event_impl(
	ui::Event const& /*event*/
) noexcept {
	return false;
}

void
Widget::render_impl() noexcept {
	/* Do nothing. */
}

void
Widget::render() noexcept {
	ui::geom_debug_render(get_context().get_terminal(), get_geometry());
	render_impl();
}

} // namespace ui
} // namespace Beard
