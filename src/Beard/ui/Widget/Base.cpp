
#include <Beard/ui/packing.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Widget/Base.hpp>
#include <Beard/ui/debug.hpp>

#include <utility>

namespace Beard {
namespace ui {
namespace Widget {

// class Base implementation

Base::~Base() noexcept = default;

// implementation

void
Base::cache_geometry_impl() noexcept {
	/* Do nothing. */
}

void
Base::reflow_impl(
	Rect const& area,
	bool const cache
) noexcept {
	if (cache) {
		cache_geometry();
	}
	ui::reflow(area, get_geometry());
}

bool
Base::handle_event_impl(
	ui::Event const& /*event*/
) noexcept {
	return false;
}

void
Base::render_impl(
	ui::Widget::RenderData& /*rd*/
) noexcept {
	/* Do nothing. */
}

bool
Base::handle_event(
	ui::Event const& event
) noexcept {
	if (
		// Avoid SPtr construction if we can
		signal_event_filter.is_bound() &&
		signal_event_filter(
			std::move(shared_from_this()),
			event
		)
	) {
		return true;
	}
	return handle_event_impl(event);
}

void
Base::render(
	ui::Widget::RenderData& rd
) noexcept {
	if (!is_visible()) {
		return;
	}
	if (rd.get_boolean(ui::property_frame_debug_enabled)) {
		ui::geom_debug_render(
			rd.terminal,
			get_geometry(),
			tty::Color::term_default,
			is_focused()
		);
	}
	render_impl(rd);
}

// properties

void
Base::set_visible(
	bool const visible
) noexcept {
	if (is_visible() != visible) {
		m_flags.set(ui::Widget::Flags::visible, visible);
		queue_actions(enum_combine(
			ui::UpdateActions::flag_parent,
			ui::UpdateActions::reflow,
			ui::UpdateActions::render
		));
	}
}

void
Base::set_focused(
	bool const focused
) noexcept {
	if (is_focused() != focused) {
		ui::Event event;
		event.type = ui::EventType::focus_changed;
		event.focus_changed.previous = is_focused();
		m_flags.set(ui::Widget::Flags::focused, focused);
		if (!handle_event(event)) {
			queue_actions(enum_combine(
				ui::UpdateActions::render,
				ui::UpdateActions::flag_noclear
			));
		}
	}
}

void
Base::set_focus_index(
	ui::focus_index_type const index
) noexcept {
	if (
		index != m_focus_index &&
		get_type_info().type_flags.test(ui::Widget::TypeFlags::focusable) &&
		is_root_valid()
	) {
		m_root.lock()->notify_focus_index_changing(
			shared_from_this(),
			m_focus_index,
			index
		);
	}
	m_focus_index = index;
}

// operations

constexpr bool
is_clearing_render(
	ui::UpdateActions const actions
) noexcept {
	return
		enum_cast(ui::UpdateActions::render)
		== enum_bitand(
			actions,
			enum_combine(
				ui::UpdateActions::render,
				ui::UpdateActions::flag_noclear
			)
		)
	;
}

void
Base::queue_actions(
	ui::UpdateActions const actions
) {
	if (enum_bitand(actions, ui::UpdateActions::mask_actions)) {
		if (!is_action_queued()) {
			get_root()->get_context().enqueue_widget(shared_from_this());
		}

		auto const current = get_queued_actions();
		auto new_actions = enum_cast(current) | enum_cast(actions);
		if (
			is_clearing_render(current) ||
			is_clearing_render(actions)
		) {
			new_actions &= ~ enum_cast(ui::UpdateActions::flag_noclear);
		}
		m_flags.set_masked(
			mask_ua,
			static_cast<ui::Widget::Flags>(
				new_actions << shift_ua
			)
		);
		set_action_queued(true);
	}
}

void
Base::clear_actions(
	bool const dequeue
) {
	if (dequeue) {
		get_root()->get_context().dequeue_widget(shared_from_this());
	}
	m_flags.remove(mask_ua);
	set_action_queued(false);
}

} // namespace Widget
} // namespace ui
} // namespace Beard
