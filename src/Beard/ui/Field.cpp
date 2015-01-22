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
#include <Beard/ui/Field.hpp>

#include <duct/CharacterSet.hpp>

#include <iostream>

namespace Beard {
namespace ui {

// class Field implementation

Field::~Field() noexcept = default;

void
Field::update_view() noexcept {
	if (!has_input_control()) {
		m_view.col_extent(txt::Extent::head);
	} else {
		auto const& frame = get_geometry().get_frame();
		auto const inner_width = max_ce(0, frame.size.width - 2 - 1);
		if (
			m_view.col() > m_cursor.col() ||
			m_view.col() + inner_width < m_cursor.col()
		) {
			m_view.col_abs(
				m_cursor.col() - (inner_width / 2)
			);
		}
	}
}

// implementation

void
Field::set_input_control_impl(
	bool const enabled
) noexcept {
	base_type::set_input_control_impl(enabled);
	signal_control_changed(
		std::move(std::static_pointer_cast<ui::Field>(
			shared_from_this()
		)),
		has_input_control()
	);
	get_root()->get_context().get_terminal().set_caret_visible(
		has_input_control()
	);
	/*m_cursor.col_extent(
		has_input_control()
		? txt::Extent::tail
		: txt::Extent::head
	);*/
	update_view();
	queue_actions(
		ui::UpdateActions::render |
		ui::UpdateActions::flag_noclear
	);
}

void
Field::reflow_impl() noexcept {
	base_type::reflow_impl();
	update_view();
}

namespace {
static KeyInputMatch const
s_kim_icontrol[]{
	{KeyMod::none, KeyCode::enter, codepoint_none, false}/*,
	{KeyMod::none, KeyCode::none, ' ', false}*/
};

static duct::CharacterSet const
s_input_blacklist{
	"\t"
};
}; // anonymous namespace

bool
Field::handle_event_impl(
	ui::Event const& event
) noexcept {
	switch (event.type) {
	case ui::EventType::key_input:
		if (
			key_input_match(event.key_input, s_kim_icontrol) ||
			(has_input_control() && KeyCode::esc == event.key_input.code)
		) {
			if (has_input_control()) {
				signal_user_modified(
					std::move(std::static_pointer_cast<ui::Field>(
						shared_from_this()
					)),
					KeyCode::esc != event.key_input.code
				);
			}
			set_input_control(!has_input_control());
			return true;
		} else if (has_input_control()) {
			switch (event.key_input.code) {
			case KeyCode::up   : m_cursor.row_prev(); break;
			case KeyCode::down : m_cursor.row_next(); break;
			case KeyCode::left : m_cursor.col_prev(); break;
			case KeyCode::right: m_cursor.col_next(); break;
			case KeyCode::home: m_cursor.col_extent(txt::Extent::head); break;
			case KeyCode::end : m_cursor.col_extent(txt::Extent::tail); break;
			case KeyCode::del      : m_cursor.erase(); break;
			case KeyCode::backspace: m_cursor.erase_before(); break;
			default:
				if (
					codepoint_none != event.key_input.cp &&
					(!m_filter || m_filter(event.key_input.cp)) &&
					!s_input_blacklist.contains(event.key_input.cp)
				) {
					m_cursor.insert_step(event.key_input.cp);
				}
				break;
			}
			update_view();
			queue_actions(
				ui::UpdateActions::render |
				ui::UpdateActions::flag_noclear
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
Field::render_impl(
	ui::Widget::RenderData& rd
) noexcept {
	auto const& frame = get_geometry().get_frame();
	auto const& node = m_cursor.get_node();

	bool const use_underline = rd.get_boolean(
		ui::property_field_content_underline
	);
	tty::attr_type const
		content_fg
		= (use_underline) ? tty::Attr::underline : 0
		| rd.get_attr(is_focused()
			? ui::property_content_fg_active
			: ui::property_content_fg_inactive
		),
		content_bg
		= rd.get_attr(is_focused()
			? ui::property_content_bg_active
			: ui::property_content_bg_inactive
		)
	;
	tty::Cell cell_end = tty::make_cell(
		'[',
		rd.get_attr(is_focused()
			? ui::property_primary_fg_active
			: ui::property_primary_fg_inactive
		),
		rd.get_attr(is_focused()
			? ui::property_primary_bg_active
			: ui::property_primary_bg_inactive
		)
	);
	rd.terminal.put_cell(frame.pos.x, frame.pos.y, cell_end);
	auto const inner_width = max_ce(geom_value_type{0}, frame.size.width - 2);
	auto const put_count = min_ce(
		static_cast<txt::Cursor::difference_type>(inner_width),
		max_ce(
			txt::Cursor::difference_type{0},
			signed_cast(node.points()) - m_view.col()
		)
	);
	rd.terminal.put_sequence(
		frame.pos.x + 1,
		frame.pos.y,
		txt::Sequence{
			&*node.cbegin() + m_view.index(),
			unsigned_cast(max_ce(
				txt::Cursor::difference_type{0},
				signed_cast(node.units()) - m_view.index()
			))
		},
		put_count,
		content_fg,
		content_bg
	);
	tty::Cell const cell_ul = tty::make_cell(
		' ',
		content_fg,
		content_bg
	);
	rd.terminal.put_line(
		{static_cast<geom_value_type>(frame.pos.x + 1 + put_count), frame.pos.y},
		inner_width - put_count,
		Axis::horizontal,
		cell_ul
	);
	cell_end.u8block.assign(']');
	rd.terminal.put_cell(
		frame.pos.x + frame.size.width - 1,
		frame.pos.y,
		cell_end
	);
	if (has_input_control()) {
		rd.terminal.set_caret_pos(
			frame.pos.x
			+ min_ce(
				inner_width,
				1 + static_cast<geom_value_type>(
					m_cursor.col() - m_view.col()
				)
			),
			frame.pos.y
		);
	}
}

// properties

void
Field::set_text(
	String const& text
) {
	m_cursor.col_extent(txt::Extent::head);
	m_cursor.assign(text);
	m_cursor.col_extent(txt::Extent::tail);
	update_view();
	queue_actions(
		ui::UpdateActions::flag_parent |
		ui::UpdateActions::reflow |
		ui::UpdateActions::render
	);
}

} // namespace ui
} // namespace Beard
