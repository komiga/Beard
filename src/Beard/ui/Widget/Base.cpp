
#include <Beard/ui/packing.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Widget/Base.hpp>
#include <Beard/ui/debug.hpp>

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
	tty::Terminal& /*terminal*/
) noexcept {
	/* Do nothing. */
}

void
Base::render(
	tty::Terminal& terminal
) noexcept {
#ifndef NDEBUG
	ui::geom_debug_render(
		terminal,
		get_geometry(),
		tty::Color::term_default,
		is_focused()
	);
#endif
	render_impl(terminal);
}

// properties

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

void
Base::queue_actions(
	ui::UpdateActions const actions
) {
	if (enum_cast(actions) & ~enum_cast(ui::UpdateActions::flag_parent)) {
		if (!is_action_queued()) {
			get_root()->get_context().enqueue_widget(shared_from_this());
		}
		m_flags.set_masked(
			ui::Widget::Flags::none,
			static_cast<ui::Widget::Flags>(
				enum_cast(m_flags.get_states(mask_ua)) |
				(enum_cast(actions) << shift_ua)
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
