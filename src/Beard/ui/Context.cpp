
#include <Beard/detail/gr_ceformat.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>
#include <Beard/ui/Container.hpp>

#include <utility>

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
	tty::TerminalInfo term_info
)
	: m_terminal(std::move(term_info))
	, m_event()
	, m_root()
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
		widget = widget->get_parent();
	}
	return false;
}

// update queue

ui::UpdateActions
Context::run_actions(
	ui::Widget::SPtr widget,
	ui::UpdateActions const mask
) {
	auto actions = widget->get_queued_actions();
	if (
		enum_bitand(actions, ui::UpdateActions::flag_parent) &&
		widget->has_parent()
	) {
		widget = widget->get_parent();
	}
	actions = static_cast<ui::UpdateActions>(enum_bitand(actions, mask));
	if (enum_bitand(actions, ui::UpdateActions::reflow)) {
		widget->reflow(
			(widget == m_root)
				? Rect{{0, 0}, m_terminal.get_size()}
				: widget->get_geometry().get_area()
			,
			true
		);
	}
	if (enum_bitand(actions, ui::UpdateActions::render)) {
		m_terminal.clear_back(widget->get_geometry().get_area());
		widget->render(m_terminal);
	}
	return actions;
}

void
Context::run_all_actions() {
	auto const sub = static_cast<ui::UpdateActions>(
		~ enum_cast(m_root->get_queued_actions())
	);
	if (enum_bitand(sub, ui::UpdateActions::reflow)) {
		for (auto wp : m_action_queue) {
			if (!wp.expired()) {
				run_actions(wp.lock(), ui::UpdateActions::reflow);
			}
		}
	} else {
		run_actions(m_root, ui::UpdateActions::reflow);
	}
	if (enum_bitand(sub, ui::UpdateActions::render)) {
		for (auto wp : m_action_queue) {
			if (!wp.expired()) {
				run_actions(wp.lock(), ui::UpdateActions::render);
			}
		}
	} else {
		run_actions(m_root, ui::UpdateActions::render);
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
	for (auto& wp : m_action_queue) {
		auto widget = wp.lock();
		if (widget) {
			widget->clear_actions(false);
		}
	}
	m_action_queue.clear();
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

static KeyInputMatch const
s_kim_root[]{
	{KeyMod::shift, KeyCode::none, '\t', false},
	{KeyMod::none , KeyCode::none, '\t', false},
	{KeyMod::ctrl , KeyCode::none,  'c', false},
};

bool
Context::update(
	unsigned const input_timeout
) {
	tty::Event tty_event;
	switch (m_terminal.poll(tty_event, input_timeout)) {
	case tty::EventType::resize:
		render(true);
		break;

	case tty::EventType::key_input:
		m_event.type = ui::EventType::key_input;
		m_event.key_input = tty_event.key_input;
		if (!push_event(m_event, get_root()->get_focus())) {
			// TODO: Remove ^C hack and add handlers
			auto const kim = key_input_match(m_event.key_input, s_kim_root);
			if (kim) {
				switch (kim->cp) {
				case '\t':
					get_root()->focus_dir(
						(KeyMod::shift == m_event.key_input.mod)
						? ui::FocusDir::prev
						: ui::FocusDir::next
					);
					break;

				case 'c':
					return true;
				}
			}
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
Context::reflow() noexcept {
	Rect const area{{0, 0}, m_terminal.get_size()};
	m_root->reflow(area, true);
}

void
Context::render(
	bool const reflow
) {
	m_root->queue_actions(enum_combine(
		ui::UpdateActions::render,
		reflow
			? ui::UpdateActions::reflow
			: ui::UpdateActions::none
	));
	run_all_actions();
	m_terminal.present();
}

#undef BEARD_SCOPE_CLASS // ui::Context

} // namespace ui
} // namespace Beard
