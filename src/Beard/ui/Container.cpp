
#include <Beard/utility.hpp>
#include <Beard/ui/packing.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Container.hpp>

namespace Beard {
namespace ui {

// class Container implementation

Container::~Container() noexcept = default;

void
Container::cache_geometry_impl() noexcept {
	Vec2 rs = get_geometry().get_request_size();
	for (auto& slot : m_slots) {
		if (slot.widget) {
			slot.widget->cache_geometry();
			Vec2 const& ws = slot.widget->get_geometry().get_request_size();
			rs.width  = max_ce(rs.width , ws.width);
			rs.height = max_ce(rs.height, ws.height);
		}
	}
	if (!get_geometry().is_static()) {
		get_geometry().set_request_size(std::move(rs));
	}
}

void
Container::reflow_impl(
	Rect const& area,
	bool const cache
) noexcept {
	Widget::reflow_impl(area, cache);
	ui::reflow_slots(
		get_geometry().get_frame(),
		m_slots,
		m_orientation,
		false
	);
}

void
Container::render_impl() noexcept {
	for (auto& slot : m_slots) {
		if (slot.widget) {
			slot.widget->render();
		}
	}
}

} // namespace ui
} // namespace Beard
