
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Root.hpp>

namespace Beard {
namespace ui {

// class Root implementation

namespace {
static ui::Widget::type_info const
s_type_info{
	ui::Widget::Type::Root,
	enum_combine(
		ui::Widget::TypeFlags::container
	)
};
} // anonymous namespace

Root::~Root() noexcept = default;

ui::Widget::type_info const&
Root::get_type_info_impl() const noexcept {
	return s_type_info;
}

void
Root::notify_focus_index_changing(
	ui::Widget::SPtr const& widget,
	ui::focus_index_type const old_index,
	ui::focus_index_type const new_index
) {
	ui::FocusMap::const_iterator iter = m_focus_map.cend();
	if (ui::focus_index_none != old_index) {
		iter = m_focus_map.erase(widget).first;
	}
	if (ui::focus_index_none != new_index) {
		iter = m_focus_map.emplace(new_index, widget);
		if (widget->is_focused() && has_focus()) {
			m_focus.iter = iter;
		}
	} else if (widget->is_focused() && has_focus()) {
		// TODO: Retain focus?
		iter = (m_focus_map.cend() == iter)
			? m_focus_map.prev(iter)
			: iter
		;
		set_focus(
			iter,
			(m_focus_map.cend() == iter)
				? ui::Widget::SPtr()
				: iter->second.lock()
		);
	}
}

// focus

void
Root::set_focus(
	ui::FocusMap::const_iterator const iter,
	ui::Widget::SPtr const& widget
) {
	ui::Widget::SPtr const current = get_focus();
	if (current == widget) {
		return;
	}
	// TODO: Queue these actions
	if (current) {
		current->set_focused(false);
	}
	if (widget) {
		m_focus.iter = iter;
		m_focus.widget = widget;
		widget->set_focused(true);
	} else {
		m_focus.iter = m_focus_map.cend();
		m_focus.widget.reset();
	}
}

} // namespace ui
} // namespace Beard
