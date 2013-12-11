
#include <Beard/ui/packing.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Container.hpp>

namespace Beard {
namespace ui {

// class Container implementation

Container::~Container() noexcept = default;

void
Container::cache_geometry_impl() noexcept {
	// TODO
}

void
Container::reflow_impl(
	Rect const& area
) noexcept {
	ui::reflow(area, get_geometry());
	ui::reflow_slots(
		get_geometry().get_frame(),
		m_slots,
		m_orientation,
		true
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
