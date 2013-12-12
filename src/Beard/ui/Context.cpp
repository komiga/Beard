
#include <Beard/detail/gr_ceformat.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Widget.hpp>

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
	, m_focused()
{}

bool
Context::push_event(
	ui::Event const& event,
	ui::WidgetSPtr widget
) noexcept {
	while (widget) {
		if (widget->handle_event(event)) {
			return true;
		}
		widget = widget->get_parent();
	}
	return false;
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
	m_terminal.close();
}

static KeyInputMatch const
s_kim_quit[]{
	{KeyMod::ctrl, KeyCode::none, 'c', false}
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
		if (!push_event(m_event, get_focused())) {
			// TODO: Remove ^C hack and add handlers
			if (key_input_match(tty_event.key_input, s_kim_quit)) {
				return true;
			}
		}
		break;

	case tty::EventType::none:
		break;
	}
	return false;
}

void
Context::reflow() noexcept {
	if (m_root) {
		Rect const area{{0, 0}, m_terminal.get_size()};
		m_root->reflow(area, true);
	}
}

void
Context::render(
	bool const reflow
) {
	// TODO: Propagate cleared state to m_root->render()
	if (reflow) {
		this->reflow();
	}
	if (m_root) {
		m_root->render();
	}
	m_terminal.present();
}

#undef BEARD_SCOPE_CLASS // ui::Context

} // namespace ui
} // namespace Beard
