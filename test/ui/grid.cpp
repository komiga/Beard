
#include <Beard/aux.hpp>
#include <Beard/utility.hpp>
#include <Beard/Error.hpp>
#include <Beard/geometry.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Ops.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Geom.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Container.hpp>
#include <Beard/ui/Label.hpp>
#include <Beard/ui/Button.hpp>
#include <Beard/ui/ProtoGrid.hpp>
#include <Beard/ui/packing.hpp>

#include <duct/debug.hpp>

#include <cassert>
#include <utility>
#include <iostream>
#include <iomanip>

#include "../common/common.hpp"

using namespace Beard;

class TestGrid final
	: public ui::ProtoGrid
{
private:
	using base_type = ui::ProtoGrid;

public:
	struct Column final {
		enum class Flags : unsigned {
			focused = 1 << 0
		};

		duct::StateStore<Flags> states{};
		String value{};
	};

	struct Row final {
		enum class Flags : unsigned {
			selected = 1 << 0
		};

		duct::StateStore<Flags> states{};
		aux::vector<Column> columns{};

		Row() = default;

		explicit
		Row(
			std::size_t const size
		)
			: states()
			, columns(size)
		{}

		Column&
		operator[](
			std::size_t const index
		) noexcept {
			return columns[index];
		}

		Column const&
		operator[](
			std::size_t const index
		) const noexcept {
			return columns[index];
		}
	};

	using row_vector_type = aux::vector<Row>;

private:
	enum class ctor_priv {};

	TestGrid() noexcept = delete;
	TestGrid(TestGrid const&) = delete;
	TestGrid& operator=(TestGrid const&) = delete;

	struct {
		ui::index_type col{-1};
		ui::index_type row{-1};
	} m_cursor{};

	row_vector_type m_rows;

private:
// ui::Widget::Base implementation
	ui::Widget::type_info const&
	get_type_info_impl() const noexcept override;

	void
	reflow_impl(
		Rect const& area,
		bool const cache
	) noexcept override;

	bool
	handle_event_impl(
		ui::Event const& event
	) noexcept override;

	void
	render_impl(
		ui::Widget::RenderData& rd
	) noexcept override;

// ui::ProtoGrid implementation
	void
	content_action(
		ui::ProtoGrid::ContentAction action,
		ui::index_type row_begin,
		ui::index_type count
	) noexcept override;

	void
	render_header(
		ui::GridRenderData& grid_rd,
		ui::index_type const col_begin,
		ui::index_type const col_end,
		Rect const& frame
	) noexcept override;

	void
	render_content(
		ui::GridRenderData& grid_rd,
		ui::index_type const row,
		ui::index_type const col_begin,
		ui::index_type const col_end,
		Rect const& frame
	) noexcept override;

private:
	void
	adjust_view() noexcept;

public:
	~TestGrid() noexcept override = default;

	TestGrid(
		ctor_priv const,
		ui::RootWPtr&& root,
		ui::index_type const col_count,
		ui::index_type const row_count,
		ui::group_hash_type const group,
		ui::Widget::WPtr&& parent
	) noexcept
		: base_type(
			std::move(root),
			enum_combine(ui::Widget::Flags::visible),
			group,
			{{0, 0}, true, Axis::both, Axis::both},
			std::move(parent),
			col_count,
			row_count
		)
		, m_rows(row_count)
	{
		for (auto& row : m_rows) {
			row.columns.resize(col_count);
		}
	}

	static aux::shared_ptr<TestGrid>
	make(
		ui::RootWPtr root,
		ui::index_type const col_count,
		ui::index_type const row_count,
		ui::group_hash_type const group = ui::group_default,
		ui::focus_index_type const focus_index = ui::focus_index_lazy,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		auto p = aux::make_shared<TestGrid>(
			ctor_priv{},
			std::move(root),
			col_count,
			row_count,
			group,
			std::move(parent)
		);
		p->set_focus_index(focus_index);
		return p;
	}

	TestGrid(TestGrid&&) = default;
	TestGrid& operator=(TestGrid&&) = default;

public:
	row_vector_type&
	get_rows() noexcept {
		return m_rows;
	}

	void
	set_cursor(
		ui::index_type col,
		ui::index_type row
	) noexcept;

	void
	col_abs(
		ui::index_type const col
	) noexcept {
		set_cursor(col, m_cursor.row);
	}

	void
	col_step(
		ui::index_type const amt
	) noexcept {
		col_abs(m_cursor.col + amt);
	}

	void
	row_abs(
		ui::index_type row
	) noexcept {
		set_cursor(m_cursor.col, row);
	}

	void
	row_step(
		ui::index_type const amt
	) noexcept {
		row_abs(m_cursor.row + amt);
	}
};

namespace {
static ui::Widget::type_info const
s_type_info{
	static_cast<ui::Widget::Type>(
		enum_cast(ui::Widget::Type::USERSPACE_BASE) + 1000u
	),
	enum_combine(
		ui::Widget::TypeFlags::focusable
	)
};
} // anonymous namespace

ui::Widget::type_info const&
TestGrid::get_type_info_impl() const noexcept {
	return s_type_info;
}

void
TestGrid::reflow_impl(
	Rect const& area,
	bool const cache
) noexcept {
	base_type::reflow_impl(area, cache);
	Rect view_frame = get_geometry().get_frame();
	++view_frame.pos.x;
	++view_frame.pos.y;
	view_frame.size.width -= 2;
	view_frame.size.height -= 2;
	reflow_view(view_frame);
	adjust_view();
	queue_header_render();
	queue_cell_render(0, get_row_count());
}

bool
TestGrid::handle_event_impl(
	ui::Event const& event
) noexcept {
	switch (event.type) {
	case ui::EventType::key_input:
		switch (event.key_input.code) {
		case KeyCode::up   : row_step(-1); break;
		case KeyCode::down : row_step(+1); break;
		case KeyCode::left : col_step(-1); break;
		case KeyCode::right: col_step(+1); break;
		case KeyCode::home: row_abs(0); break;
		case KeyCode::pgup: row_step(min_ce(0, -get_view().fit_count - 1)); break;
		case KeyCode::pgdn: row_step(max_ce(0, +get_view().fit_count - 1)); break;
		case KeyCode::end : row_abs(max_ce(0, get_row_count() - 1)); break;

		case KeyCode::f1:
			set_header_enabled(!is_header_enabled());
			queue_actions(enum_combine(
				ui::UpdateActions::reflow,
				ui::UpdateActions::render
			));
			break;

		case KeyCode::del: erase(m_cursor.row, 1); break;

		case KeyCode::insert:
			if (KeyMod::shift == event.key_input.mod) {
				insert_before(m_cursor.row, 1);
			} else {
				insert_after(m_cursor.row, 1);
			}
			break;

		default:
			switch (event.key_input.cp) {
			case 'a': select_all(); break;
			case 'A': select_none(); break;
			case ' ': select_toggle(m_cursor.row, 1); break;
			case 'D': erase_selected(); break;
			}
			break;
		}
		break;

	case ui::EventType::focus_changed:
		// Default action on a false return from focus_changed is
		// to queue a non-clearing render. Simpler to queue a cell
		// render (needed due to due to the debug backdrop) than
		// it would be to queue a clearing render.
		//queue_cell_render(0, get_row_count());
		return false;

	default:
		break;
	}
	return false; // TODO
}

void
TestGrid::render_impl(
	ui::Widget::RenderData& rd
) noexcept {
	/*DUCT_DEBUGF(
		"clearing render: %d",
		signed{!enum_bitand(
			get_queued_actions(),
			ui::UpdateActions::flag_noclear
		)}
	);*/

	ui::GridRenderData grid_rd{
		rd,
		is_focused(),
		is_focused()
	};
	auto const& view = get_view();

	render_view(
		grid_rd,
		!enum_bitand(
			get_queued_actions(),
			ui::UpdateActions::flag_noclear
		)
	);

	Rect const empty_frame{
		{view.content_frame.pos.x, view.content_frame.pos.y + view.row_count},
		{
			view.content_frame.size.width,
			max_ce(0, view.fit_count - view.row_count)
		}
	};
	if (0 < empty_frame.size.height) {
		grid_rd.rd.terminal.clear_back(
			empty_frame,
			tty::make_cell('X',
				tty::Color::black,
				tty::Color::blue
			)
		);
	}
}

void
TestGrid::content_action(
	ui::ProtoGrid::ContentAction action,
	ui::index_type row_begin,
	ui::index_type count
) noexcept {
	using ContentAction = ui::ProtoGrid::ContentAction;

	// Cast insert_after in terms of insert_before
	if (ContentAction::insert_after == action) {
		++row_begin;
	}
	row_begin = value_clamp(row_begin, 0, get_row_count());
	auto const row_end = min_ce(row_begin + count, get_row_count());
	auto clear_flag = ui::UpdateActions::none;
	switch (action) {
	// Select
	case ContentAction::select:
	case ContentAction::unselect: {
		bool const enable = ContentAction::select == action;
		auto const end = m_rows.begin() + row_end;
		for (auto it = m_rows.begin() + row_begin; end > it; ++it) {
			it->states.set(
				Row::Flags::selected,
				enable
			);
		}
		queue_cell_render(row_begin, row_end);
		clear_flag = ui::UpdateActions::flag_noclear;
	}	break;

	case ContentAction::select_toggle: {
		auto const end = m_rows.begin() + row_end;
		for (auto it = m_rows.begin() + row_begin; end > it; ++it) {
			it->states.set(
				Row::Flags::selected,
				!it->states.test(Row::Flags::selected)
			);
		}
		queue_cell_render(row_begin, row_end);
		clear_flag = ui::UpdateActions::flag_noclear;
	}	break;

	// Insert
	case ContentAction::insert_after: // fall-through
	case ContentAction::insert_before:
		m_rows.insert(
			m_rows.begin() + row_begin,
			static_cast<std::size_t>(count),
			Row{static_cast<std::size_t>(get_col_count())}
		);
		content_action_internal(
			ContentAction::insert_before,
			row_begin,
			count
		);
		if (1 == get_row_count()) {
			set_cursor(m_cursor.col, 0);
			m_rows[m_cursor.row][m_cursor.col].states.enable(
				Column::Flags::focused
			);
		}
		adjust_view();
		break;

	// Erase
	case ContentAction::erase:
		m_rows.erase(
			m_rows.cbegin() + row_begin,
			m_rows.cbegin() + row_end
		);
		content_action_internal(
			ContentAction::erase,
			row_begin,
			count
		);
		break;

	case ContentAction::erase_selected:
		ui::index_type
			head = 0,
			tail = head,
			rcount = 0
		;
		for (; get_row_count() >= tail;) {
			if (
				get_row_count() == tail ||
				!m_rows[tail].states.test(Row::Flags::selected)
			) {
				if (tail > head) {
					m_rows.erase(
						m_rows.cbegin() + head,
						m_rows.cbegin() + tail
					);
					rcount = tail - head;
					content_action_internal(ContentAction::erase, head, rcount);
					if (head <= m_cursor.row) {
						set_cursor(
							m_cursor.col,
							max_ce(head, m_cursor.row - rcount)
						);
					}
					// tail is not selected, so no sense in checking it again
					tail = ++head;
				} else {
					head = ++tail;
				}
			} else {
				++tail;
			}
		}
		break;
	} // switch (action)

	// Post action
	switch (action) {
	case ContentAction::erase:
	case ContentAction::erase_selected:
		// Let cursor clamp to new bounds
		set_cursor(m_cursor.col, m_cursor.row);
		if (0 < get_row_count()) {
			// Set focused flag in case set_cursor() did nothing
			m_rows[m_cursor.row][m_cursor.col].states.enable(
				Column::Flags::focused
			);
		}
		adjust_view();
		break;

	default:
		break;
	}

	queue_actions(enum_combine(
		ui::UpdateActions::render,
		clear_flag
	));
}

void
TestGrid::render_header(
	ui::GridRenderData& grid_rd,
	ui::index_type const col_begin,
	ui::index_type const col_end,
	Rect const& frame
) noexcept {
	auto& rd = grid_rd.rd;
	Rect cell_frame = frame;
	cell_frame.pos.x += col_begin * 10;
	String name{};
	for (auto col = col_begin; col_end > col; ++col) {
		cell_frame.size.width = min_ce(
			10,
			frame.pos.x + frame.size.width - cell_frame.pos.x
		);
		name = "Column " + std::to_string(col);
		rd.terminal.put_sequence(
			cell_frame.pos.x,
			cell_frame.pos.y,
			txt::Sequence{name, 0u, name.size()},
			cell_frame.size.width,
			grid_rd.primary_fg | tty::Attr::bold,
			tty::Color::blue
		);
		if (10 > cell_frame.size.width) {
			break;
		}
		cell_frame.pos.x += 10;
	}
}

void
TestGrid::render_content(
	ui::GridRenderData& grid_rd,
	ui::index_type const row,
	ui::index_type const col_begin,
	ui::index_type const col_end,
	Rect const& frame
) noexcept {
	/*DUCT_DEBUGF(
		"render_content: row = %3d, col_range = {%3d, %3d}"
		", view.col_range = {%3d, %3d}, pos = {%3d, %3d}",
		row, col_begin, col_end,
		get_view().col_range.x, get_view().col_range.y,
		frame.pos.x, frame.pos.y
	);*/

	auto& rd = grid_rd.rd;
	auto const& r = m_rows[row];
	tty::attr_type attr_fg = grid_rd.content_fg;
	tty::attr_type attr_bg = grid_rd.content_bg;
	if (r.states.test(Row::Flags::selected)) {
		attr_fg = grid_rd.selected_fg;
		attr_bg = grid_rd.selected_bg;
	}

	Rect cell_frame = frame;
	cell_frame.pos.x += col_begin * 10;
	auto const end
		= get_col_count() < col_end
		? r.columns.cend()
		: r.columns.cbegin() + col_end
	;
	auto cell = tty::make_cell(' ', attr_fg, attr_bg);
	for (auto it = r.columns.cbegin() + col_begin; end > it; ++it) {
		cell_frame.size.width = min_ce(
			10,
			frame.pos.x + frame.size.width - cell_frame.pos.x
		);
		cell.attr_bg
			= it->states.test(Column::Flags::focused)
			? attr_bg | tty::Attr::inverted
			: attr_bg
		;
		rd.terminal.put_line(
			cell_frame.pos,
			cell_frame.size.width,
			Axis::horizontal,
			cell
		);
		rd.terminal.put_sequence(
			cell_frame.pos.x,
			cell_frame.pos.y,
			txt::Sequence{it->value, 0u, it->value.size()},
			cell_frame.size.width,
			attr_fg,
			cell.attr_bg
		);
		if (10 > cell_frame.size.width) {
			break;
		}
		cell_frame.pos.x += 10;
	}
}

void
TestGrid::adjust_view() noexcept {
	auto const& view = get_view();
	if (
		view.row_range.x >  m_cursor.row ||
		view.row_range.y <= m_cursor.row ||
		view.col_range.x >  m_cursor.col ||
		view.col_range.y <= m_cursor.col
	) {
		auto row_begin = m_cursor.row;
		if (view.row_range.y <= m_cursor.row) {
			row_begin -= view.fit_count - 1;
		}
		update_view(
			row_begin,
			row_begin + view.fit_count,
			0,
			get_col_count(),
			true
		);
		queue_actions(ui::UpdateActions::render);
	}
}

void
TestGrid::set_cursor(
	ui::index_type col,
	ui::index_type row
) noexcept {
	col = value_clamp(col, 0, max_ce(0, get_col_count() - 1));
	row = value_clamp(row, 0, max_ce(0, get_row_count() - 1));
	if (col != m_cursor.col || row != m_cursor.row) {
		if (
			value_in_bounds(m_cursor.row, 0, get_row_count()) &&
			value_in_bounds(m_cursor.col, 0, get_col_count())
		) {
			m_rows[m_cursor.row][m_cursor.col].states.disable(
				Column::Flags::focused
			);
			queue_cell_render(
				m_cursor.row, m_cursor.row + 1,
				m_cursor.col, m_cursor.col + 1
			);
		}
		if (!m_rows.empty()) {
			m_rows[row][col].states.enable(
				Column::Flags::focused
			);
			queue_cell_render(
				row, row + 1,
				col, col + 1
			);
		}
		queue_actions(enum_combine(
			ui::UpdateActions::render,
			ui::UpdateActions::flag_noclear
		));
		m_cursor.col = col;
		m_cursor.row = row;
		adjust_view();
	}
}

signed
main(
	signed argc,
	char* argv[]
) {
	if (2 > argc || 3 < argc) {
		std::cerr <<
			"invalid arguments\n"
			"usage: packing terminfo-file-path [tty-path]\n"
		;
		return -1;
	}

	ui::Context ctx;
	tty::Terminal& term = ctx.get_terminal();

	char const* const info_path = argv[1];
	if (!load_term_info(term.get_info(), info_path)) {
		return -2;
	}
	term.update_cache();

	bool use_sigwinch = false;
	String tty_path{};
	if (2 < argc) {
		tty_path.assign(argv[2]);
	} else {
		tty_path.assign(tty::this_path());
		use_sigwinch = true;
	}

	try {
		ctx.open(tty_path, use_sigwinch);
	} catch (Error const& ex) {
		report_error(ex);
	}

	auto root = ui::Root::make(ctx, Axis::vertical);
	ctx.set_root(root);

	auto grid = TestGrid::make(root, 16, 120);
	signed row = 0, col = 0;
	for (auto& r : grid->get_rows()) {
		auto const row_str = std::to_string(row);
		for (auto& c : r.columns) {
			c.value = std::to_string(col) + " , " + row_str;
			++col;
		}
		col = 0;
		++row;
	}
	grid->set_cursor(0, 0);

	auto cont = ui::Container::make(root, Axis::vertical);
	cont->push_back(std::move(grid));
	cont->push_back(ui::Button::make(root, "blahblahblah"));

	root->push_back(cont);
	ctx.render(true);
	std::cout.flush();

	while (!ctx.update(10u)) {}
	ctx.close();
	return 0;
}
