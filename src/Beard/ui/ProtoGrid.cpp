/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/ui/Defs.hpp>
#include <Beard/ui/ProtoGrid.hpp>

namespace Beard {
namespace ui {

// class ProtoGrid implementation

ProtoGrid::~ProtoGrid() noexcept = default;

// internals

namespace {
inline void
expand_range(
	Vec2& range,
	ui::index_type const begin,
	ui::index_type const end
) noexcept {
	if (-1 != range.x) {
		if (range.x == range.y) {
			range.x = begin;
			range.y = end;
		} else if (-1 == begin && begin == end) {
			range.x = -1;
			range.y = -1;
		} else {
			range.x = min_ce(range.x, begin);
			range.y = max_ce(range.y, end);
		}
	}
}
} // anonymous namespace

void
ProtoGrid::update_view(
	ui::index_type row_begin,
	ui::index_type row_end,
	ui::index_type col_begin,
	ui::index_type col_end,
	bool const retain_intersection
) noexcept {
	row_begin = value_clamp(row_begin, 0, m_row_count);
	row_end = min_ce(
		value_clamp(row_end, row_begin, m_row_count),
		row_begin + m_view.fit_count
	);
	col_begin = value_clamp(col_begin, 0, m_col_count);
	col_end = value_clamp(col_end, col_begin, m_col_count);

	Vec2 const nrr{row_begin, row_end};
	auto const orc = m_view.row_count;
	Vec2 const orr = m_view.row_range;

	m_view.col_count = m_col_count - col_begin;
	m_view.row_count = row_end - row_begin;
	m_view.col_range = {col_begin, m_col_count};
	m_view.row_range = nrr;

	/*DUCT_DEBUGF(
		"update_view(): fit_count = %3d, row_count = %3d, col_count = %3d,\n"
		"  row_range = {%3d, %3d}, col_range = {%3d, %3d}"
		", orc = %3d"
		", dirty.rows.size = %zu",
		m_view.fit_count, m_view.row_count, m_view.col_count,
		m_view.row_range.x, m_view.row_range.y,
		m_view.col_range.x, m_view.col_range.y,
		orc, m_dirty.rows.size()
	);*/

	if (retain_intersection) {
		Vec2 const v_refresh{-1, -1};
		switch (range_rel(nrr, orr)) {
		case RangeRel::equal:
			// Nothing to do
			break;

		case RangeRel::disjoint_before:
		case RangeRel::disjoint_after:
			std::fill(
				m_dirty.rows.begin(),
				m_dirty.rows.end(),
				v_refresh
			);
			break;

		case RangeRel::subset:
		case RangeRel::intersection_after:
			// fall-through

		case RangeRel::superset:
		case RangeRel::intersection_before:
			auto const head = m_dirty.rows.begin();
			if (nrr.x > orr.x) {
				auto const idx = nrr.x - orr.x;
				auto const amt = min_ce(m_view.row_count, orc - idx);
				std::copy(head + idx, head + idx + amt, head);
				std::fill(head + idx + amt, m_dirty.rows.end(), v_refresh);
			} else if (nrr.x < orr.x) {
				auto const idx = orr.x - nrr.x;
				auto const amt = min_ce(orc, m_view.row_count - idx);
				std::copy_backward(head, head + amt, head + idx + amt);
				std::fill(head, head + amt, v_refresh);
				std::fill(head + idx + amt, m_dirty.rows.end(), v_refresh);
			} else {
				std::fill(
					m_dirty.rows.begin() + m_view.row_count,
					m_dirty.rows.end(),
					v_refresh
				);
			}
			break;
		}
	}
}

void
ProtoGrid::content_action_internal(
	ui::ProtoGrid::ContentAction const action,
	ui::index_type row_begin,
	ui::index_type count
) noexcept {
	if (0 >= count) {
		return;
	}

	row_begin = max_ce(0, row_begin);
	switch (action) {
	case ContentAction::insert_after:
		// Cast insert_after in terms of insert_before
		++row_begin;
		// fall-through

	case ContentAction::insert_before:
	case ContentAction::erase:
		// We only handle these three actions
		break;

	default:
		// Nothing to do for other actions
		return;
	}

	row_begin = min_ce(row_begin, m_row_count);
	auto const row_end = min_ce(row_begin + count, m_row_count);

	RangeRel const rel = range_rel(Vec2{row_begin, row_end}, m_view.row_range);
	bool const not_disjoint = RangeRel::disjoint_after != rel;

	// For both erasure and insertion, queue everything from
	// push_begin to the end of the view.
	// Bound-checked for a tiny bit of optimization (don't need to
	// mark anything outside the view itself)
	auto const push_begin = max_ce(row_begin, m_view.row_range.x);
	if (not_disjoint && push_begin < m_view.row_range.y) {
		std::fill(
			m_dirty.rows.begin() + (push_begin - m_view.row_range.x),
			m_dirty.rows.begin() + m_view.row_count,
			Vec2{-1, -1}
		);
	}

	if (ContentAction::erase == action) {
		// Erasure is bound (row_end is bound to m_row_count)
		set_row_count(m_row_count - (row_end - row_begin));
		// TODO: Shift view up if it contains no rows
	} else {
		// Insertion is unbound
		set_row_count(m_row_count + count);
	}
	if (not_disjoint) {
		update_view(
			m_view.row_range.x,
			m_view.row_range.x + m_view.fit_count,
			m_view.col_range.x,
			m_view.col_range.y,
			true
		);
	}
}

void
ProtoGrid::queue_header_render(
	ui::index_type col_begin,
	ui::index_type col_end
) noexcept {
	if (is_header_enabled()) {
		if (-1 == col_begin) {
			col_end = -1;
			m_dirty.header = {-1, -1};
		} else {
			col_begin = value_clamp(col_begin, m_view.col_range);
			col_end = value_clamp(col_end, col_begin, m_view.col_range.y);
			if (col_begin != col_end) {
				expand_range(m_dirty.header, col_begin, col_end);
			}
		}
	}
}

void
ProtoGrid::queue_cell_render(
	ui::index_type row_begin,
	ui::index_type row_end,
	ui::index_type col_begin,
	ui::index_type col_end
) noexcept {
	// NB: row_begin will be clamped to the view range, which is
	// already clamped to the number of rows. Also, row_begin needn't
	// be clamped to m_row_count because row_end is clamped to that,
	// and the bound comparison is row_end > row.
	if (-1 == col_begin) {
		col_end = -1;
	} else {
		col_begin = value_clamp(col_begin, m_view.col_range);
		col_end = value_clamp(col_end, col_begin, m_view.col_range.y);
		if (col_begin == col_end) {
			return;
		}
	}

	// Make row range relative to view
	row_begin = value_clamp(row_begin - m_view.row_range.x, 0, m_row_count);
	row_end = min_ce(
		max_ce(
			index_type{0},
			min_ce(row_end, m_row_count) - m_view.row_range.x
		),
		m_view.row_count
	);

	for (
		auto row = row_begin;
		row_end > row;
		++row
	) {
		expand_range(m_dirty.rows[row], col_begin, col_end);
	}
}

void
ProtoGrid::reflow_view(
	Rect const& view_frame
) noexcept {
	if (
		m_header_enabled == m_header_enabled_next &&
		view_frame == m_view.frame
	) {
		return;
	}

	m_header_enabled = m_header_enabled_next;

	m_view.frame = view_frame;
	vec2_clamp_min(m_view.frame.size, Vec2{0, 0});
	m_view.content_frame = m_view.frame;
	if (is_header_enabled()) {
		// Clamps here are to avoid sneaking outside the view frame
		m_view.header_frame = m_view.frame;
		m_view.header_frame.size.height = min_ce(
			1,
			m_view.frame.size.height
		);

		m_view.content_frame.pos.y = min_ce(
			m_view.content_frame.pos.y + 1,
			m_view.frame.pos.y + m_view.frame.size.height
		);
		m_view.content_frame.size.height = max_ce(
			0,
			m_view.content_frame.size.height - 1
		);
	} else {
		m_view.header_frame = {{0, 0}, {0, 0}};
	}

	// We want to retain the row range at its beginning, so
	// instead of calculating the intersection with the previous
	// content frame, we're just queuing the entire range
	m_view.fit_count = m_view.content_frame.size.height;
	m_dirty.rows.resize(m_view.fit_count);
	update_view(
		m_view.row_range.x,
		m_view.row_range.x + m_view.fit_count,
		m_view.col_range.x,
		m_view.col_range.y,
		false
	);

	if (is_header_enabled()) {
		m_dirty.header = {-1, -1};
	}
	std::fill(
		m_dirty.rows.begin(),
		m_dirty.rows.end(),
		Vec2{-1, -1}
	);
}

void
ProtoGrid::render_view(
	ui::GridRenderData& grid_rd,
	bool all
) noexcept {
	Vec2 cr = m_view.col_range;
	if (is_header_enabled()) {
		if (!all) {
			cr = m_dirty.header;
			if (-1 == cr.x) {
				cr = m_view.col_range;
			} else {
				cr.x = value_clamp(cr.x, m_view.col_range);
				cr.y = value_clamp(cr.y, cr.x, m_view.col_range.y);
			}
		}
		if (cr.x < cr.y) {
			render_header(grid_rd, cr.x, cr.y, m_view.header_frame);
		}
	}

	signed row = 0;
	signed row_end = 0;
	Rect frame = m_view.content_frame;
	if (0 >= range_length(m_view.col_range)) {
		// Do nothing
	} else if (all) {
		frame.size.height = m_view.row_count;
		render_content(
			grid_rd,
			m_view.row_range.x, m_view.row_range.y,
			cr.x, cr.y,
			frame
		);
		row = m_view.row_count;
	} else {
		Vec2 cr_queued;
		while (m_view.row_count > row) {
			if (m_view.row_count == row_end || 0 == m_dirty.rows[row_end].y) {
				if (row == row_end) {
					row = ++row_end;
					continue;
				}
			} else {
				if (!all) {
					cr_queued = m_dirty.rows[row_end];
					if (-1 == cr_queued.x) {
						cr = m_view.col_range;
						all = true;
					} else {
						cr.x = value_clamp(min_ce(cr.x, cr_queued.x), m_view.col_range);
						cr.y = value_clamp(max_ce(cr.y, cr_queued.y), cr.x, m_view.col_range.y);
						if (range_length(cr) == range_length(m_view.col_range)) {
							all = true;
						}
					}
				}
				++row_end;
				continue;
			}
			frame.pos.y = m_view.content_frame.pos.y + row;
			frame.size.height = row_end - row;
			render_content(
				grid_rd,
				m_view.row_range.x + row,
				m_view.row_range.x + row_end,
				cr.x, cr.y,
				frame
			);
			std::fill(
				m_dirty.rows.begin() + row,
				m_dirty.rows.begin() + row_end,
				Vec2{0, 0}
			);
			row = ++row_end;
		}
	}

	if (m_view.fit_count > row) {
		// FIXME: Destructive clear
		frame.pos.y = m_view.content_frame.pos.y + m_view.row_count;
		frame.size.height = m_view.fit_count - m_view.row_count;
		grid_rd.rd.terminal.clear_back(frame);
	}
}

} // namespace ui
} // namespace Beard
