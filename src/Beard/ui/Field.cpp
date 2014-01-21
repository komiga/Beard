
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

namespace {
static ui::Widget::type_info const
s_type_info{
	ui::Widget::Type::Field,
	enum_combine(
		ui::Widget::TypeFlags::focusable
	)
};
} // anonymous namespace

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
				m_cursor.col() - (inner_width * 0.5)
			);
		}
	}
}

// implementation

ui::Widget::type_info const&
Field::get_type_info_impl() const noexcept {
	return s_type_info;
}

void
Field::reflow_impl(
	Rect const& area,
	bool const cache
) noexcept {
	base_type::reflow_impl(area, cache);
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
			set_input_control(!has_input_control());
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
			queue_actions(ui::UpdateActions::render);
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
			queue_actions(ui::UpdateActions::render);
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
	tty::Terminal& terminal
) noexcept {
	auto const& frame = get_geometry().get_frame();
	auto const& node = m_cursor.get_node();
	tty::Cell cell_end = tty::make_cell(
		'[',
		tty::Color::term_default,
		tty::Color::term_default |
		is_focused() ? tty::Attr::inverted : 0
	);
	terminal.put_cell(frame.pos.x, frame.pos.y, cell_end);
	auto const inner_width = max_ce(geom_value_type{0}, frame.size.width - 2);
	auto const put_count = min_ce(
		static_cast<txt::Cursor::difference_type>(inner_width),
		max_ce(
			txt::Cursor::difference_type{0},
			signed_cast(node.points()) - m_view.col()
		)
	);
	terminal.put_sequence(
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
		tty::Color::term_default | tty::Attr::underline,
		tty::Color::term_default
	);
	tty::Cell const cell_ul = tty::make_cell(
		' ',
		tty::Color::term_default | tty::Attr::underline
	);
	terminal.put_line(
		{static_cast<geom_value_type>(frame.pos.x + 1 + put_count), frame.pos.y},
		inner_width - put_count,
		Axis::horizontal,
		cell_ul
	);
	cell_end.u8block.assign(']');
	terminal.put_cell(
		frame.pos.x + frame.size.width - 1,
		frame.pos.y,
		cell_end
	);
	if (has_input_control()) {
		terminal.set_caret_pos(
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
	queue_actions(enum_combine(
		ui::UpdateActions::flag_parent,
		ui::UpdateActions::reflow,
		ui::UpdateActions::render
	));
}

} // namespace ui
} // namespace Beard