
#include <Beard/utility.hpp>
#include <Beard/keys.hpp>
#include <Beard/geometry.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Button.hpp>

namespace Beard {
namespace ui {

// class Button implementation

Button::~Button() noexcept = default;

// implementation

void
Button::cache_geometry_impl() noexcept {
	if (get_geometry().is_static()) {
		return;
	}

	auto& rs = get_geometry().get_request_size();
	rs.width = max_ce(1, 2 + static_cast<signed>(m_text.size()));
	rs.height = 1;
}

void
Button::reflow_impl(
	Rect const& area,
	bool const cache
) noexcept {
	Widget::reflow_impl(area, cache);

	auto const& geom = get_geometry();
	auto const& fr = geom.get_frame();
	auto const& rs = geom.get_request_size();
	m_gc_pos = fr.pos;
	if (geom.expands_and_fills(Axis::x)) {
		m_gc_pos.x
			+= (fr.size.width / 2)
			- min_ce(rs.width, fr.size.width) / 2
		;
	}
	if (geom.expands_and_fills(Axis::y)) {
		m_gc_pos.y
			+= (fr.size.height / 2)
			- min_ce(rs.height, fr.size.height) / 2
		;
	}
}

namespace {
static KeyInputMatch const
s_kim_pressed[]{
	{KeyMod::none, KeyCode::enter, codepoint_none, false},
	{KeyMod::none, KeyCode::none, ' ', false}
};
}; // anonymous namespace

bool
Button::handle_event_impl(
	ui::Event const& event
) noexcept {
	switch (event.type) {
	case ui::EventType::key_input:
		if (key_input_match(event.key_input, s_kim_pressed)) {
			signal_pressed();
			return true;
		}
		break;

	default:
		break;
	}
	return false;
}

void
Button::render_impl() noexcept {
	auto& term = get_context().get_terminal();
	auto const& frame = get_geometry().get_frame();
	term.put_sequence(
		m_gc_pos.x + (static_cast<signed>(m_text.size()) < frame.size.width),
		m_gc_pos.y,
		tty::Sequence{m_text, static_cast<unsigned>(max_ce(0, frame.size.width))},
		tty::Color::term_default,
		tty::Color::term_default | tty::Attr::inverted
	);
}

} // namespace ui
} // namespace Beard
