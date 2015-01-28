
#include <Beard/utility.hpp>
#include <Beard/Error.hpp>
#include <Beard/keys.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Ops.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/ProtoSlotContainer.hpp>
#include <Beard/ui/Container.hpp>
#include <Beard/ui/Button.hpp>

#include <cassert>
#include <iostream>

#include "../common/common.hpp"

using namespace Beard;

void
add_button(
	ui::RootSPtr const& root,
	ui::ProtoSlotContainer::SPtr container,
	Axis const orientation,
	String&& label,
	bool const no_container
) {
	if (root->has_focus()) {
		container = std::static_pointer_cast<ui::ProtoSlotContainer>(
			root->focused_widget()->parent()
		);
	}
	auto button = ui::Button::make(root, std::move(label));
	button->geometry().set_sizing(Axis::both, Axis::both);
	if (no_container) {
		container->push_back(std::move(button));
	} else {
		auto const button_container = ui::Container::make(root, orientation);
		button_container->geometry().set_sizing(Axis::both, Axis::both);
		button_container->push_back(std::move(button));
		container->push_back(std::move(button_container));
	}
}

void
remove_widget(
	ui::RootSPtr const& root
) {
	if (root->has_focus()) {
		auto const focus = root->focused_widget();
		auto const container = std::static_pointer_cast<ui::ProtoSlotContainer>(
			focus->parent()
		);
		container->remove(focus->index());
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
			"usage: dynamic_focus terminfo-file-path [tty-path]\n"
		;
		return -1;
	}

	ui::Context ctx;
	tty::Terminal& term = ctx.terminal();

	char const* const info_path = argv[1];
	if (!load_term_info(term.info(), info_path)) {
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

	auto root = ui::Root::make(ctx, Axis::horizontal);
	ctx.set_root(root);

	auto const hcont = ui::Container::make(root, Axis::horizontal);
	hcont->geometry().set_sizing(Axis::both, Axis::both);
	root->push_back(hcont);

	ctx.render(true);
	unsigned count = 0;
	while (true) {
		if (ctx.update(15u)) {
			continue;
		}
		auto const& event = ctx.last_event();
		if (ui::EventType::key_input != event.type) {
			continue;
		} else if (key_input_match(event.key_input, s_kim_c)) {
			break;
		}
		switch (event.key_input.cp) {
		case 'a':
			add_button(
				root,
				hcont,
				Axis::horizontal,
				String{"A-"} + std::to_string(count++),
				true
			);
			break;

		case 'h':
			add_button(
				root,
				hcont,
				Axis::horizontal,
				String{"H-"} + std::to_string(count++),
				false
			);
			break;

		case 'v':
			add_button(
				root,
				hcont,
				Axis::vertical,
				String{"V-"} + std::to_string(count++),
				false
			);
			break;

		case 'r':
			remove_widget(root);
			break;

		case 'c':
			hcont->clear();
			count = 0;
			break;
		}
	}
	ctx.close();
	return 0;
}
