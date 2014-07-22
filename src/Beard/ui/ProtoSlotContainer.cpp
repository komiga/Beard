
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
		if (slot.widget && slot.widget->is_visible()) {
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

signed
ProtoSlotContainer::num_children_impl() const noexcept {
	return m_slots.size();
}

ui::Widget::SPtr
ProtoSlotContainer::get_child_impl(
	ui::index_type const index
) {
	return m_slots.at(static_cast<unsigned>(index)).widget;
}

void
ProtoSlotContainer::remove(
	ui::index_type const s_index
) {
	unsigned index = static_cast<unsigned>(s_index);
	if (0 > s_index || m_slots.size() <= index) {
		throw std::out_of_range{"index is out of bounds"};
	}
	m_slots.erase(m_slots.cbegin() + index);
	for (; m_slots.size() > index; ++index) {
		m_slots[index].widget->set_index(index);
	}
	queue_actions(enum_combine(
		ui::UpdateActions::reflow,
		ui::UpdateActions::render
	));
}

void
ProtoSlotContainer::clear() {
	for (auto const& slot : m_slots) {
		if (slot.widget) {
			slot.widget->clear_parent();
		}
	}
	m_slots.clear();
	queue_actions(enum_combine(
		ui::UpdateActions::reflow,
		ui::UpdateActions::render
	));
}

void
ProtoSlotContainer::set_child(
	ui::index_type const index,
	ui::Widget::SPtr widget
) {
	auto& slot = m_slots.at(static_cast<unsigned>(index));
	if (slot.widget) {
		slot.widget->clear_parent();
	}
	slot.widget = std::move(widget);
	if (slot.widget) {
		slot.widget->set_parent(shared_from_this(), index);
	}
	queue_actions(enum_combine(
		ui::UpdateActions::reflow,
		ui::UpdateActions::render
	));
}

void
ProtoSlotContainer::push_back(
	ui::Widget::SPtr widget
) {
	if (widget) {
		widget->set_parent(shared_from_this(), static_cast<signed>(m_slots.size()));
	}
	m_slots.push_back(ui::Widget::Slot{std::move(widget), {}});
	queue_actions(enum_combine(
		ui::UpdateActions::reflow,
		ui::UpdateActions::render
	));
}

} // namespace ui
} // namespace Beard
