
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
#include <Beard/ui/Button.hpp>

#include <cassert>
#include <iostream>

#include "../common/common.hpp"

using namespace Beard;
using Beard::tty::make_cell;

static constexpr ui::focus_index_type const focus_index_1 = 10;
static constexpr ui::focus_index_type const focus_index_2 = 20;

void
button_pressed_toggle(
	aux::shared_ptr<ui::Button> button
) {
	auto const root = button->get_root();
	if (focus_index_1 == button->get_focus_index()) {
		root->set_bound_index(focus_index_2);
	} else {
		root->set_bound_index(focus_index_1);
	}
	root->focus_dir(ui::FocusDir::next);
}

void
button_pressed_disable(
	aux::shared_ptr<ui::Button> button
) {
	auto const root = button->get_root();
	root->set_bound_index(ui::focus_index_none);
}

signed
main(
	signed argc,
	char* argv[]
) {
	if (2 > argc || 3 < argc) {
		std::cerr <<
			"invalid arguments\n"
			"usage: bound_focus terminfo-file-path [tty-path]\n"
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
		return 1;
	}

	auto root = ui::Root::make(ctx, Axis::vertical);
	ctx.set_root(root);

	auto hcont1 = ui::Container::make(root, Axis::horizontal);
	hcont1->get_geometry().set_sizing(Axis::both, Axis::both);
	{
		auto button = ui::Button::make(
			root, "1-toggle", ui::group_button, focus_index_1
		);
		button->get_geometry().set_sizing(Axis::both, Axis::both);
		button->signal_pressed.bind(button_pressed_toggle);
		hcont1->push_back(std::move(button));

		button = ui::Button::make(
			root, "1-disable", ui::group_button, focus_index_1
		);
		button->get_geometry().set_sizing(Axis::both, Axis::both);
		button->signal_pressed.bind(button_pressed_disable);
		hcont1->push_back(std::move(button));
	}

	auto hcont2 = ui::Container::make(root, Axis::horizontal);
	hcont2->get_geometry().set_sizing(Axis::both, Axis::both);
	{
		auto button = ui::Button::make(
			root, "2-toggle", ui::group_button, focus_index_2
		);
		button->get_geometry().set_sizing(Axis::both, Axis::both);
		button->signal_pressed.bind(button_pressed_toggle);
		hcont2->push_back(std::move(button));

		button = ui::Button::make(
			root, "2-disable", ui::group_button, focus_index_2
		);
		button->get_geometry().set_sizing(Axis::both, Axis::both);
		button->signal_pressed.bind(button_pressed_disable);
		hcont2->push_back(std::move(button));
	}

	root->push_back(hcont1);
	root->push_back(hcont2);
	ctx.render(true);

	while (context_update(ctx)) {}
	ctx.close();
	return 0;
}
