
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
#include <Beard/ui/Field.hpp>
#include <Beard/ui/packing.hpp>

#include <cassert>
#include <iostream>

#include "../common/common.hpp"

using namespace Beard;
using Beard::tty::make_cell;

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
		return 1;
	}

	auto root = ui::Root::make(ctx, Axis::vertical);
	ctx.set_root(root);

	auto hcont1 = ui::Container::make(root, Axis::horizontal);
	{
		auto field = ui::Field::make(
			root,
			""
		);
		field->set_text("X_111_222_333_444_555_666_777_888_999_Y");
		field->get_geometry().set_sizing(Axis::both, Axis::horizontal);
		hcont1->push_back(std::move(field));

		hcont1->push_back(ui::Label::make(root, "abacabadabacaba"));

		auto button = ui::Button::make(root, "xyzzyzzyx");
		button->get_geometry().set_sizing(Axis::both, Axis::both);
		button->signal_pressed.bind([](
			aux::shared_ptr<ui::Button> b
		) {
			if ('x' == b->get_text()[0u]) {
				b->set_text("blblblblblblblblblbl");
			} else {
				b->set_text("xyzzyzzyx");
			}
		});
		hcont1->push_back(std::move(button));
	}

	auto hcont2 = ui::Container::make(root, Axis::horizontal);
	hcont2->get_geometry().set_request_size(Vec2{0, 3});
	hcont2->get_geometry().set_static(true);
	hcont2->get_geometry().set_sizing(Axis::x, Axis::x);
	{
		hcont2->push_back(ui::Field::make(root, "hi I am a field"));
		hcont2->push_back(nullptr);
		hcont2->push_back(ui::Field::make(
			root,
			"that is not a field, I am a field!"
		));
		for (auto& s : hcont2->get_slots()) {
			if (s.widget) {
				s.widget->get_geometry().set_sizing(
					Axis::both, Axis::horizontal
				);
			}
		}
	}

	auto hcont3 = ui::Container::make(root, Axis::horizontal);
	hcont3->get_geometry().set_request_size(Vec2{0, 3});
	hcont3->get_geometry().set_static(true);
	hcont3->get_geometry().set_sizing(Axis::x, Axis::x);
	{
		hcont3->push_back(ui::Button::make(root, "aaa"));
		hcont3->push_back(ui::Button::make(root, "bbb"));
		hcont3->push_back(nullptr);
		hcont3->push_back(ui::Button::make(root, "ccc"));
		hcont3->push_back(ui::Button::make(root, "ddd"));
		for (auto& s : hcont3->get_slots()) {
			if (s.widget) {
				s.widget->get_geometry().set_sizing(Axis::both, Axis::none);
			}
		}
	}

	root->push_back(hcont1);
	root->push_back(hcont2);
	root->push_back(hcont3);
	ctx.render(true);

	std::cout << "root: " << root->get_geometry() << '\n';
	std::cout.flush();
	for (auto const& s1 : root->get_slots()) {
		std::cout << "top-level child: " << s1.widget->get_geometry() << '\n';
		for (
			auto const& s2
			: std::static_pointer_cast<ui::Container>(s1.widget)->get_slots()
		) {
			if (s2.widget) {
				std::cout << "inner child: " << s2.widget->get_geometry() << '\n';
			}
		}
	}
	std::cout.flush();

	while (context_update(ctx)) {}
	ctx.close();
	return 0;
}
