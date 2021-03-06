/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>
#include <Beard/ui/Container.hpp>

#include <duct/debug.hpp>

#include <utility>

#include <Beard/detail/gr_ceformat.hpp>

namespace Beard {
namespace ui {

// class Context implementation

#define BEARD_SCOPE_CLASS ui::Context

Context::~Context() noexcept {
	close();
}

Context::Context() = default;
Context::Context(Context&&) = default;

Context& Context::operator=(Context&&) = default;

Context::Context(
	ui::PropertyMap property_map
)
	: m_terminal()
	, m_property_map(std::move(property_map))
{}

Context::Context(
	tty::TerminalInfo term_info,
	ui::PropertyMap property_map
)
	: m_terminal(std::move(term_info))
	, m_property_map(std::move(property_map))
{}

bool
Context::push_event(
	ui::Event const& event,
	ui::Widget::SPtr widget
) noexcept {
	while (widget) {
		if (widget->handle_event(event)) {
			return true;
		}
		widget = widget->parent();
	}
	return false;
}

// update queue

ui::UpdateActions
Context::run_actions(
	ui::Widget::RenderData& rd,
	ui::Widget::Base* widget,
	ui::UpdateActions const mask
) {
	auto const actions
		= widget->queued_actions()
		& (mask | ui::UpdateActions::mask_flags)
	;
	if (enum_cast(actions & ui::UpdateActions::reflow)) {
		widget->reflow();
	}
	if (enum_cast(actions & ui::UpdateActions::render)) {
		// TODO: Optimization: only clear if the terminal hasn't been
		// cleared entirely?
		if (!enum_cast(actions & ui::UpdateActions::flag_noclear)) {
			m_terminal.clear_back(widget->geometry().area());
		}
		rd.update_group(widget->group());
		widget->render(rd);
	}
	return actions;
}

inline static bool
widget_depth_less(
	ui::Widget::Base const* const lhs,
	ui::Widget::Base const* const rhs
) noexcept {
	return lhs->depth() < rhs->depth();
}

void
Context::run_all_actions() {
	ui::Widget::RenderData rd{
		*this,
		m_terminal,
		m_property_map,
		ui::group_null,
		m_property_map.cend(),
		m_property_map.find(m_fallback_group)
	};

	DUCT_DEBUG("Context: start frame");
	for (auto wp : m_action_queue) {
		if (!wp.expired()) {
			auto const widget = wp.lock();
			auto const actions = widget->queued_actions();
			if (
				enum_cast(actions & ui::UpdateActions::flag_parent) &&
				widget->has_parent()
			) {
				auto parent = widget->parent();
				if (parent->is_action_queued()) {
					widget->clear_actions(false);
					parent->enqueue_actions(actions);
					continue;
				}
			}
			if (widget->is_visible()) {
				widget->push_action_graph(m_execution_set, actions);
			} else {
				widget->clear_actions(false);
			}
		}
	}

	for (auto widget : m_execution_set) {
		m_execution_set_ordered.push_back(widget);
	}
	std::sort(
		m_execution_set_ordered.begin(),
		m_execution_set_ordered.end(),
		widget_depth_less
	);

	for (auto it = m_execution_set_ordered.rbegin(); it != m_execution_set_ordered.rend(); ++it) {
		if (enum_cast((*it)->queued_actions() & ui::UpdateActions::reflow)) {
			(*it)->cache_geometry();
		}
	}
	for (auto widget : m_execution_set_ordered) {
		run_actions(rd, widget, ui::UpdateActions::reflow);
	}
	for (auto widget : m_execution_set_ordered) {
		run_actions(rd, widget, ui::UpdateActions::render);
		widget->clear_actions(false);
	}
	clear_actions();
}

void
Context::enqueue_widget(
	ui::Widget::SPtr const& widget
) {
	m_action_queue.emplace(widget);
}

void
Context::dequeue_widget(
	ui::Widget::SPtr const& widget
) {
	auto it = m_action_queue.find(widget);
	if (m_action_queue.cend() != it) {
		m_action_queue.erase(it);
	}
}

void
Context::clear_actions() {
	for (auto wp : m_action_queue) {
		if (!wp.expired()) {
			wp.lock()->clear_actions(false);
		}
	}
	m_action_queue.clear();
	m_execution_set.clear();
	m_execution_set_ordered.clear();
}

void
Context::set_root(
	ui::RootSPtr root
) noexcept {
	m_root = std::move(root);
	if (m_root) {
		m_root->geometry().set_area(
			{{0, 0}, m_terminal.size()}
		);
		m_root->enqueue_actions(
			ui::UpdateActions::render |
			ui::UpdateActions::reflow
		);
	}
}

// operations

#define BEARD_SCOPE_FUNC open
void
Context::open(
	String const& tty_path,
	bool const use_sigwinch
) {
	if (m_terminal.is_open()) {
		BEARD_THROW_FQN(
			ErrorCode::ui_context_already_open,
			"context is already open"
		);
	}
	m_terminal.open(tty_path, use_sigwinch);
}
#undef BEARD_SCOPE_FUNC

void
Context::close() noexcept {
	clear_actions();
	m_terminal.close();
}

bool
Context::update(
	unsigned const input_timeout
) {
	tty::Event tty_event;
	ui::Widget::SPtr focus;
	m_event.type = ui::EventType::none;
	switch (m_terminal.poll(tty_event, input_timeout)) {
	case tty::EventType::resize:
		m_root->geometry().set_area(
			{{0, 0}, m_terminal.size()}
		);
		m_root->enqueue_actions(
			ui::UpdateActions::render |
			ui::UpdateActions::reflow
		);
		break;

	case tty::EventType::key_input:
		m_event.type = ui::EventType::key_input;
		m_event.key_input = tty_event.key_input;
		focus
			= root()->has_focus()
			? root()->focused_widget()
			: root()
		;
		if (push_event(m_event, focus)) {
			return true;
		}
		break;

	case tty::EventType::none:
		break;
	}

	if (!m_action_queue.empty()) {
		run_all_actions();
		m_terminal.present();
	}
	return false;
}

void
Context::render(
	bool const reflow
) {
	m_root->enqueue_actions(
		ui::UpdateActions::render |
		(reflow ? ui::UpdateActions::reflow : ui::UpdateActions::none)
	);
	run_all_actions();
	m_terminal.present();
}

#undef BEARD_SCOPE_CLASS // ui::Context

} // namespace ui
} // namespace Beard
