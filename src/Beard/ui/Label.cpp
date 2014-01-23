
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/txt/Defs.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Label.hpp>

#include <iostream>

namespace Beard {
namespace ui {

// class Label implementation

namespace {
static ui::Widget::type_info const
s_type_info{
	ui::Widget::Type::Label,
	enum_combine(
		ui::Widget::TypeFlags::none
	)
};
} // anonymous namespace

Label::~Label() noexcept = default;

// implementation

ui::Widget::type_info const&
Label::get_type_info_impl() const noexcept {
	return s_type_info;
}

void
Label::cache_geometry_impl() noexcept {
	if (get_geometry().is_static()) {
		return;
	}

	auto& rs = get_geometry().get_request_size();
	rs.width = max_ce(1, 2 + static_cast<signed>(m_text.size()));
	rs.height = 1;
}

void
Label::reflow_impl(
	Rect const& area,
	bool const cache
) noexcept {
	base_type::reflow_impl(area, cache);

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

void
Label::render_impl(
	ui::Widget::RenderData& rd
) noexcept {
	auto const& frame = get_geometry().get_frame();
	rd.terminal.put_sequence(
		m_gc_pos.x + (signed_cast(m_text.size()) < frame.size.width),
		m_gc_pos.y,
		txt::Sequence{m_text, 0u, m_text.size()},
		unsigned_cast(max_ce(0, frame.size.width)),
		rd.get_attr(ui::property_content_fg_inactive),
		rd.get_attr(ui::property_content_bg_inactive)
	);
}

// properties

void
Label::set_text(
	String text
) {
	m_text.assign(std::move(text));
	queue_actions(enum_combine(
		ui::UpdateActions::flag_parent,
		ui::UpdateActions::reflow,
		ui::UpdateActions::render
	));
}

} // namespace ui
} // namespace Beard
