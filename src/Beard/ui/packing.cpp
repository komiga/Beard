
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Geom.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>
#include <Beard/ui/packing.hpp>

namespace Beard {
namespace ui {

namespace {

inline void
center_axis(
	geom_value_type const ap,
	geom_value_type const as,
	geom_value_type& wp,
	geom_value_type& ws,
	geom_value_type const rs
) {
	ws = min_ce(rs, as);
	wp = ap + (as / 2) - (ws / 2);
}

void
expand_rect(
	Rect const& area,
	Rect& rect,
	Vec2 const& request_size,
	Axis axes
) noexcept {
	axes = axis_transpose(axes);
	rect = area;
	if (enum_bitand(axes, Axis::x)) {
		center_axis(
			area.pos.x, area.size.width,
			rect.pos.x, rect.size.width,
			request_size.width
		);
	}
	if (enum_bitand(axes, Axis::y)) {
		center_axis(
			area.pos.y, area.size.height,
			rect.pos.y, rect.size.height,
			request_size.height
		);
	}
}

} // anonymous namespace

void
reflow(
	Rect const& area,
	ui::Geom& geom
) noexcept {
	Rect& w_area = geom.get_area();
	Rect& w_frame = geom.get_frame();

	Axis const axes_e = geom.get_expand();
	expand_rect(area, w_area, geom.get_request_size(), axes_e);

	Axis const axes_f = geom.get_fill();
	if (axes_f == axes_e) {
		w_frame = w_area;
	} else {
		expand_rect(w_area, w_frame, geom.get_request_size(), axes_f);
	}
}

void
reflow_slots(
	Rect const& area,
	ui::Widget::slot_vector_type& slots,
	Axis const axis,
	bool const cache_geometry
) noexcept {
	if (slots.empty()) {
		return;
	}

	// TODO: Reserve request sizes before calculating initial areas
	// to prevent index-ordered overflow (favor preserving the size
	// of smaller widgets)

	// NB: Slot area are initially used axis-first and
	// properly transposed before reflow()
	Vec2 const area_aligned = vec2_axis_first(area.size, axis);
	geom_value_type unused = area_aligned.x;
	unsigned expand_count = 0u;
	signed remaining = static_cast<signed>(slots.size());

	// Calculate initial area
	for (auto& s : slots) {
		if (s.widget) {
			if (cache_geometry) {
				s.widget->cache_geometry();
			}
			auto const& geom = s.widget->get_geometry();
			geom_value_type const
			asize = min_ce(
				max_ce(0, unused - remaining),
				max_ce(1, vec2_axis_value(geom.get_request_size(), axis))
			);
			unused -= asize;
			s.area.size.x = asize;
			expand_count += static_cast<unsigned>(geom.expands(axis));
		} else {
			// Empty slot acts as a both-expand widget of size {0, 0}
			s.area.size.x = 0;
			++expand_count;
		}
		s.area.size.y = area_aligned.y;
		--remaining;
	}

	// Distribute any remaining space (favoring axis-expand widgets,
	// if any) and reflow
	bool const expand_only = (0u < expand_count);
	unsigned const dist_count
		= (expand_only)
		? expand_count
		: slots.size()
	;
	geom_value_type const dist = unused / dist_count;
	geom_value_type each = unused % dist_count;
	Vec2 pos = area.pos;
	geom_value_type& apos = vec2_axis_ref(pos, axis);
	for (auto& s : slots) {
		if (
			0 < unused && (
				!expand_only ||
				!s.widget ||
				s.widget->get_geometry().expands(axis)
			)
		) {
			s.area.size.x += dist + (0 < each--);
		}
		s.area.pos = pos;
		apos += s.area.size.x;
		// Return slot size to proper axis order
		if (Axis::x != axis) {
			s.area.size = vec2_transpose(s.area.size);
		}
		if (s.widget) {
			s.widget->reflow(s.area, false);
		}
	}
}

} // namespace ui
} // namespace Beard
