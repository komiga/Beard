
#include <Beard/keys.hpp>
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

static KeyInputMatch const
s_kim_root[]{
	{KeyMod::shift, KeyCode::none, '\t', false},
	{KeyMod::none , KeyCode::none, '\t', false},
};

bool
Root::handle_event_impl(
	ui::Event const& event
) noexcept {
	switch (event.type) {
	case ui::EventType::key_input: {
		auto const kim = key_input_match(event.key_input, s_kim_root);
		if (kim) {
			switch (kim->cp) {
			case '\t':
				focus_dir(
					(KeyMod::shift == event.key_input.mod)
					? ui::FocusDir::prev
					: ui::FocusDir::next
				);
				return has_focus();
			}
		}
	}	break;

	default:
		break;
	}
	return false;
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
		if (
			m_focus_map.cend() == iter ||
			m_focus_map.cend_bound() == iter
		) {
			iter = m_focus_map.clast_bound();
		} else if (!m_focus_map.is_index_in_bound(
			iter->second.lock()->get_focus_index()
		)) {
			iter = m_focus_map.cbegin_bound();
		}
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

void
Root::set_focus(
	ui::Widget::SPtr const& widget
) {
	if (has_focus() && widget->is_focused()) {
		return;
	}
	set_focus(
		static_cast<bool>(widget)
		? m_focus_map.find(widget, true)
		: m_focus_map.cend(),
		widget
	);
}

void
Root::set_bound_index(
	ui::focus_index_type const bound_index
) {
	if (
		ui::focus_index_none != bound_index &&
		has_focus()
	) {
		auto focus_widget = get_focus();
		if (focus_widget->get_focus_index() != bound_index) {
			focus_widget->set_focused(false);
			m_focus.iter = m_focus_map.cend();
			m_focus.widget.reset();
		}
	}
	m_focus_map.set_bound_index(bound_index);
}

void
Root::focus_dir(
	ui::FocusDir const dir
) {
	ui::FocusMap::const_iterator iter;
	if (has_focus()) {
		iter = (ui::FocusDir::prev == dir)
			? m_focus_map.prev_bound(m_focus.iter)
			: m_focus_map.next_bound(m_focus.iter)
		;
	} else {
		iter = (ui::FocusDir::prev == dir)
			? m_focus_map.clast_bound()
			: m_focus_map.cbegin_bound()
		;
	}
	set_focus(
		iter,
		(m_focus_map.cend() == iter)
		? Widget::SPtr()
		: iter->second.lock()
	);
}

} // namespace ui
} // namespace Beard
