/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/utility.hpp>
#include <Beard/ui/packing.hpp>
#include <Beard/ui/ProtoSlotContainer.hpp>

#include <Beard/detail/gr_core.hpp>

namespace Beard {
namespace ui {

// class ProtoSlotContainer implementation

#define BEARD_SCOPE_CLASS ui::ProtoSlotContainer

ProtoSlotContainer::~ProtoSlotContainer() noexcept = default;

void
ProtoSlotContainer::push_action_graph_impl(
	ui::Widget::set_type& set
) noexcept {
	// If we're clearing, there's no reason for children to clear
	auto push_actions = queued_actions();
	if (
		ui::UpdateActions::render
		== (push_actions & (ui::UpdateActions::render | ui::UpdateActions::flag_noclear))
	) {
		push_actions |= ui::UpdateActions::flag_noclear;
	}
	ui::UpdateActions child_actions;
	for (auto& slot : m_slots) {
		if (slot.widget->is_visible()) {
			child_actions = slot.widget->queued_actions();
			child_actions |= push_actions;
			slot.widget->push_action_graph(set, child_actions);
		}
	}
}

void
ProtoSlotContainer::cache_geometry_impl() noexcept {
	Vec2 rs = geometry().request_size();
	rs = {0, 0};
	if (m_orientation == Axis::vertical) {
		for (auto& slot : m_slots) {
			if (slot.widget->is_visible()) {
				Vec2 const& ws = slot.widget->geometry().request_size();
				rs.width = max_ce(rs.width, ws.width);
				rs.height += ws.height;
			}
		}
	} else {
		for (auto& slot : m_slots) {
			if (slot.widget->is_visible()) {
				Vec2 const& ws = slot.widget->geometry().request_size();
				rs.width += ws.width;
				rs.height = max_ce(rs.height, ws.height);
			}
		}
	}
	if (!geometry().is_static()) {
		geometry().set_request_size(std::move(rs));
	}
}

void
ProtoSlotContainer::reflow_impl() noexcept {
	base_type::reflow_impl();
	ui::reflow_slots(
		geometry().frame(),
		m_slots,
		m_orientation
	);
}

signed
ProtoSlotContainer::num_children_impl() const noexcept {
	return m_slots.size();
}

ui::Widget::SPtr
ProtoSlotContainer::child_at_impl(
	ui::index_type const index
) {
	return m_slots.at(static_cast<unsigned>(index)).widget;
}

void
ProtoSlotContainer::remove(
	ui::index_type index
) noexcept {
	auto size = signed_cast(m_slots.size());
	if (index < 0 || index >= size) {
		return;
	}
	m_slots[index].widget->clear_parent();
	m_slots.erase(m_slots.cbegin() + index);
	--size;
	for (; index < size; ++index) {
		m_slots[index].widget->set_index(index);
	}
	enqueue_actions(
		ui::UpdateActions::reflow |
		ui::UpdateActions::render
	);
}

void
ProtoSlotContainer::clear() {
	for (auto const& slot : m_slots) {
		slot.widget->clear_parent();
	}
	m_slots.clear();
	enqueue_actions(
		ui::UpdateActions::reflow |
		ui::UpdateActions::render
	);
}

#define BEARD_SCOPE_FUNC set_child
void
ProtoSlotContainer::set_child(
	ui::index_type const index,
	ui::Widget::SPtr widget
) {
	if (!widget) {
		BEARD_THROW_FQN(
			ErrorCode::ui_container_null_widget,
			"cannot set child to null"
		);
	}
	auto& slot = m_slots.at(static_cast<unsigned>(index));
	slot.widget->clear_parent();
	slot.widget = std::move(widget);
	slot.widget->set_parent(shared_from_this(), index);
	enqueue_actions(
		ui::UpdateActions::reflow |
		ui::UpdateActions::render
	);
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC push_back
void
ProtoSlotContainer::push_back(
	ui::Widget::SPtr widget
) {
	if (!widget) {
		BEARD_THROW_FQN(
			ErrorCode::ui_container_null_widget,
			"cannot add null child"
		);
	}
	widget->set_parent(shared_from_this(), static_cast<signed>(m_slots.size()));
	m_slots.push_back(ui::Widget::Slot{std::move(widget), {}});
	enqueue_actions(
		ui::UpdateActions::reflow |
		ui::UpdateActions::render
	);
}
#undef BEARD_SCOPE_FUNC

#undef BEARD_SCOPE_CLASS // ProtoSlotContainer

} // namespace ui
} // namespace Beard
