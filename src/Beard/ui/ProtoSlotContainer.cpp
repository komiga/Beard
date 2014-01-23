
#include <Beard/utility.hpp>
#include <Beard/ui/packing.hpp>
#include <Beard/ui/ProtoSlotContainer.hpp>

namespace Beard {
namespace ui {

// class ProtoSlotContainer implementation

ProtoSlotContainer::~ProtoSlotContainer() noexcept = default;

void
ProtoSlotContainer::cache_geometry_impl() noexcept {
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
ProtoSlotContainer::reflow_impl(
	Rect const& area,
	bool const cache
) noexcept {
	base_type::reflow_impl(area, cache);
	ui::reflow_slots(
		get_geometry().get_frame(),
		m_slots,
		m_orientation,
		false
	);
}

void
ProtoSlotContainer::render_impl(
	ui::Widget::RenderData& rd
) noexcept {
	for (auto& slot : m_slots) {
		if (slot.widget) {
			rd.update_group(slot.widget->get_group());
			slot.widget->render(rd);
		}
	}
}

std::size_t
ProtoSlotContainer::size_impl() const noexcept {
	return m_slots.size();
}

void
ProtoSlotContainer::clear_impl() {
	for (auto const& slot : m_slots) {
		slot.widget->set_parent(nullptr);
	}
	m_slots.clear();
}

void
ProtoSlotContainer::set_widget_impl(
	std::size_t const idx,
	ui::Widget::SPtr widget
) {
	auto& slot = m_slots.at(idx);
	if (slot.widget) {
		slot.widget->set_parent(nullptr);
	}
	slot.widget = std::move(widget);
	if (slot.widget) {
		slot.widget->set_parent(shared_from_this());
	}
}

ui::Widget::SPtr
ProtoSlotContainer::get_widget_impl(
	std::size_t const idx
) const {
	return m_slots.at(idx).widget;
}

void
ProtoSlotContainer::push_back_impl(
	ui::Widget::SPtr widget
) {
	if (widget) {
		widget->set_parent(shared_from_this());
	}
	m_slots.push_back(ui::Widget::Slot{std::move(widget), {}});
}

} // namespace ui
} // namespace Beard
