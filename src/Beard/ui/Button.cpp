/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/utility.hpp>
#include <Beard/keys.hpp>
#include <Beard/geometry.hpp>
#include <Beard/txt/Defs.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Button.hpp>

#include <iostream>

namespace Beard {
namespace ui {

// class Button implementation

Button::~Button() noexcept = default;

// implementation

void
Button::cache_geometry_impl() noexcept {
	if (geometry().is_static()) {
		return;
	}

	auto& rs = geometry().request_size();
	rs.width = max_ce(1, 2 + static_cast<signed>(m_text.size()));
	rs.height = 1;
}

void
Button::reflow_impl() noexcept {
	base_type::reflow_impl();

	auto const& geom = geometry();
	auto const& fr = geom.frame();
	auto const& rs = geom.request_size();
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
			signal_pressed(
				std::move(std::static_pointer_cast<ui::Button>(
					shared_from_this()
				))
			);
			return true;
		}
		break;

	default:
		break;
	}
	return false;
}

void
Button::render_impl(
	ui::Widget::RenderData& rd
) noexcept {
	auto const& frame = geometry().frame();
	rd.terminal.put_sequence(
		m_gc_pos.x + (signed_cast(m_text.size()) < frame.size.width),
		m_gc_pos.y,
		txt::Sequence{m_text, 0u, m_text.size()},
		unsigned_cast(max_ce(0, frame.size.width)),
		rd.attr(is_focused()
			? ui::property_primary_fg_active
			: ui::property_primary_fg_inactive
		),
		rd.attr(is_focused()
			? ui::property_primary_bg_active
			: ui::property_primary_bg_inactive
		)
	);
}

// properties

void
Button::set_text(
	String text
) {
	m_text.assign(std::move(text));
	enqueue_actions(
		ui::UpdateActions::flag_parent |
		ui::UpdateActions::reflow |
		ui::UpdateActions::render
	);
}

} // namespace ui
} // namespace Beard
