/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/keys.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Root.hpp>

namespace Beard {
namespace ui {

// class Root implementation

Root::~Root() noexcept = default;

static KeyInputMatch const
s_kim_root[]{
	{KeyMod::shift, KeyCode::none, '\t', false},
	{KeyMod::none , KeyCode::none, '\t', false},
	{KeyMod::none , KeyCode::up   , codepoint_none, false},
	{KeyMod::none , KeyCode::left , codepoint_none, false},
	{KeyMod::none , KeyCode::down , codepoint_none, false},
	{KeyMod::none , KeyCode::right, codepoint_none, false},
};

bool
Root::handle_event_impl(
	ui::Event const& event
) noexcept {
	switch (event.type) {
	case ui::EventType::key_input: {
		auto const kim = key_input_match(event.key_input, s_kim_root);
		if (kim) {
			if (
				kim->code == KeyCode::up || kim->code == KeyCode::left ||
				(kim->cp == '\t' && KeyMod::shift == event.key_input.mod)
			) {
				focus_dir(ui::FocusDir::prev);
			} else {
				focus_dir(ui::FocusDir::next);
			}
			return has_focus();
		}
	}	break;

	default:
		break;
	}
	return false;
}

// focus

namespace {
	enum : signed {
		INDEX_SWIM = ~0,
	};
	enum : unsigned {
		FLAG_DIR_PREV = bit(0u),
		FLAG_CYCLED = bit(1u),
		FLAG_USE_INDEX = bit(2u),
	};
} // anonymous namespace

ui::Widget::SPtr
deepest_rightmost_widget(
	ui::Widget::SPtr widget
) {
	while (widget->has_children()) {
		widget = widget->get_last_child();
	}
	return widget;
}

ui::Widget::SPtr
Root::focus_dir(
	ui::Widget::SPtr from,
	ui::FocusDir const dir
) {
	if (!from) {
		from = shared_from_this();
	}
	unsigned flags = (ui::FocusDir::prev == dir) ? FLAG_DIR_PREV : 0;
	signed const dir_value = (FLAG_DIR_PREV & flags) ? -1 : 1;
	ui::Widget::SPtr widget{from};
	signed child_index = 0;
	if ((FLAG_DIR_PREV & flags) && this == widget.get()) {
		widget = deepest_rightmost_widget(widget);
	}
	child_index = widget->get_index();
	while (true) {
		if (
			INDEX_SWIM != child_index &&
			widget->is_visible() &&
			widget->has_children()
		) {
			widget = widget->get_child(
				/*(FLAG_USE_INDEX & flags)
				? */child_index
				// : ((FLAG_DIR_PREV & flags) ? widget->get_last_child_index() : 0)
			);
			child_index
				= (FLAG_DIR_PREV & flags)
				? widget->get_last_child_index()
				: 0
			;
			flags &= ~FLAG_USE_INDEX;
		} else if (widget->has_parent()) {
			child_index = widget->get_index() + dir_value;
			widget = widget->get_parent();
			if (
				0 > child_index ||
				widget->num_children() <= child_index
			) {
				child_index = INDEX_SWIM;
			}
			flags |= FLAG_USE_INDEX;
		} else {
			// Swam all the way to the root (assuming invariants)
			if (ui::FocusDir::prev == dir) {
				widget = deepest_rightmost_widget(shared_from_this());
				child_index = widget->get_index();
			} else {
				widget = shared_from_this();
				child_index = 0;
			}
			flags |= FLAG_CYCLED;
		}
		if ((FLAG_CYCLED & flags) && widget == from) {
			return widget->is_focusable(true) ? widget : ui::Widget::SPtr();
		} else if (widget->is_focusable(true)) {
			return widget;
		}
	}
}

void
Root::set_focus(
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
		widget->set_focused(true);
		m_focus = widget;
	} else {
		m_focus.reset();
	}
}

} // namespace ui
} // namespace Beard
