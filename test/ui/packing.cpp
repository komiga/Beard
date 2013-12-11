
#include <Beard/utility.hpp>
#include <Beard/Error.hpp>
#include <Beard/geometry.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Ops.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Geom.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Container.hpp>
#include <Beard/ui/Button.hpp>
#include <Beard/ui/packing.hpp>

#include <cassert>
#include <iostream>
#include <iomanip>

#include "../common/common.hpp"

using namespace Beard;
using Beard::tty::make_cell;

std::ostream&
operator<<(
	std::ostream& stream,
	Axis const axis
) {
	static char const* const
	axis_names[]{"none", "x", "y", "both"};

	return stream
		<< axis_names[enum_cast(axis)]
	;
}

std::ostream&
operator<<(
	std::ostream& stream,
	Vec2 const& v
) {
	return stream
		<< '{' << std::dec
		<< std::setw(4u) << v.x << ','
		<< std::setw(4u) << v.y
		<< '}'
	;
}

std::ostream&
operator<<(
	std::ostream& stream,
	Rect const& rect
) {
	return stream
		<< '{'
		<< rect.pos << ", "
		<< rect.size
		<< '}'
	;
}

std::ostream&
operator<<(
	std::ostream& stream,
	ui::Geom const& geom
) {
	return stream
		<< "{\n"
		<< "  area  = " << geom.get_area() << ",\n"
		<< "  frame = " << geom.get_frame() << ",\n"
		<< "  nsize = " << geom.get_request_size() << ",\n"
		<< std::boolalpha
		<< "  expand = " << geom.get_expand() << ",\n"
		<< "  fill   = " << geom.get_fill()
		<< "\n}"
	;
}

signed
main(
	int argc,
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

	auto root = ui::Container::make(ctx, Axis::vertical);
	ctx.set_root(root);

	auto hcont1 = ui::Container::make(ctx, Axis::horizontal);
	{
		auto btn = ui::Button::make(ctx, "button text");
		btn->get_geometry().set_expand(Axis::both);
		hcont1->push_back(std::move(btn));

		btn = ui::Button::make(ctx, "abacabadabacaba");
		hcont1->push_back(std::move(btn));

		btn = ui::Button::make(ctx, "xyzzyzzyx");
		btn->get_geometry().set_sizing(Axis::both, Axis::both);
		hcont1->push_back(std::move(btn));
	}

	auto hcont2 = ui::Container::make(ctx, Axis::horizontal);
	hcont2->get_geometry().set_request_size(Vec2{0, 3});
	hcont2->get_geometry().set_static(true);
	hcont2->get_geometry().set_sizing(Axis::x, Axis::x);
	{
		hcont2->push_back(ui::Button::make(ctx, "[a]"));
		hcont2->push_back(ui::Button::make(ctx, "[b]"));
		hcont2->push_back(nullptr);
		hcont2->push_back(ui::Button::make(ctx, "[c]"));
		hcont2->push_back(ui::Button::make(ctx, "[d]"));
		for (auto& s : hcont2->get_slots()) {
			if (s.widget) {
				s.widget->get_geometry().set_sizing(Axis::both, Axis::none);
			}
		}
	}

	root->push_back(hcont1);
	root->push_back(hcont2);
	ctx.render(true);

	std::cout
		<< "root: " << root->get_geometry() << '\n'
	;
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

	while (!ctx.update(10u)) {}
	ctx.close();
	return 0;
}
