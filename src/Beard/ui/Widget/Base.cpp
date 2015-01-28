/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/utility.hpp>
#include <Beard/ui/packing.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Widget/Base.hpp>
#include <Beard/ui/debug.hpp>

#include <duct/debug.hpp>

#include <utility>

namespace Beard {
namespace ui {
namespace Widget {

// class Base implementation

Base::~Base() noexcept = default;

// implementation

void
Base::push_action_graph_impl(
	ui::Widget::set_type& /*set*/
) noexcept {
	/* Do nothing. */
}

void
Base::set_input_control_impl(
	bool const enabled
) noexcept {
	m_flags.set(ui::Widget::Flags::input_control, enabled);
}

void
Base::cache_geometry_impl() noexcept {
	/* Do nothing. */
}

void
Base::reflow_impl() noexcept {
	ui::reflow(geometry());
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

signed
Base::num_children_impl() const noexcept {
	return 0;
}

ui::Widget::SPtr
Base::child_at_impl(
	index_type const /*index*/
) {
	return nullptr;
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
	if (rd.boolean(ui::property_frame_debug_enabled)) {
		ui::geom_debug_render(
			rd.terminal,
			geometry(),
			tty::Color::term_default,
			is_focused()
		);
	}
	render_impl(rd);
}

// properties

void
Base::update_depth(
	ui::Widget::SPtr const& parent
) noexcept {
	if (m_type == ui::Widget::Type::Root) {
		m_depth = -1;
		return;
	} else if (parent) {
		m_depth = parent->depth() + 1;
	} else {
		m_depth = 0;
	}
	for (signed index = 0; index < num_children(); ++index) {
		auto const child = child_at(index);
		if (child) {
			child->update_depth(shared_from_this());
		}
	}
}

void
Base::set_parent(
	ui::Widget::SPtr const& widget
) noexcept {
	m_parent = widget;
	update_depth(widget);
}

void
Base::set_visible(
	bool const visible,
	bool const queue
) noexcept {
	if (is_visible() != visible) {
		m_flags.set(ui::Widget::Flags::visible, visible);
		for (signed index = 0; index < num_children(); ++index) {
			auto const child = child_at(index);
			if (child) {
				child->set_visible(visible, false);
			}
		}
		if (queue) {
			enqueue_actions(
				ui::UpdateActions::flag_parent |
				ui::UpdateActions::reflow |
				ui::UpdateActions::render
			);
		}
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
			enqueue_actions(
				ui::UpdateActions::flag_noclear |
				ui::UpdateActions::render
			);
		}
	}
}

// operations

inline constexpr bool
is_clearing_render(
	ui::UpdateActions const actions
) noexcept {
	return
		ui::UpdateActions::render
		== (actions & (ui::UpdateActions::render | ui::UpdateActions::flag_noclear))
	;
}

inline ui::UpdateActions
join_actions(
	ui::UpdateActions const x,
	ui::UpdateActions const y
) {
	if (is_clearing_render(x) || is_clearing_render(y)) {
		return (x | y) & ~ui::UpdateActions::flag_noclear;
	} else {
		return x | y;
	}
}

void
Base::enqueue_actions(
	ui::UpdateActions actions
) {
	if (enum_cast(actions & ui::UpdateActions::mask_actions)) {
		if (!is_action_queued()) {
			root()->context().enqueue_widget(shared_from_this());
		}
		actions = join_actions(actions, queued_actions());
		if (enum_cast(actions & ui::UpdateActions::flag_parent) && has_parent()) {
			parent()->enqueue_actions(
				actions & ~ui::UpdateActions::flag_parent
			);
		}
		m_flags.set_masked(
			mask_ua,
			static_cast<ui::Widget::Flags>(enum_cast(actions) << shift_ua)
		);
		set_action_queued(true);
	}
}

void
Base::clear_actions(
	bool const dequeue
) {
	if (dequeue) {
		root()->context().dequeue_widget(shared_from_this());
	}
	m_flags.remove(mask_ua);
	set_action_queued(false);
}

void
Base::push_action_graph(
	ui::Widget::set_type& set,
	ui::UpdateActions actions
) noexcept {
	actions &= ~ui::UpdateActions::flag_parent;
	auto const prev_actions = queued_actions() & ~ui::UpdateActions::flag_parent;
	m_flags.set_masked(
		mask_ua,
		static_cast<ui::Widget::Flags>(enum_cast(actions) << shift_ua)
	);
	bool const push = set.insert(this).second || actions != prev_actions;
	DUCT_DEBUGF(
		"Widget::Base::push_action_graph: %8x %16p %3d %u",
		type(), this, depth(), unsigned{push}
	);
	if (push) {
		push_action_graph_impl(set);
	}
}

} // namespace Widget
} // namespace ui
} // namespace Beard
