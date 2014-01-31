
// usage: hello terminfo-file-path [tty-path]
// Redirect stdout and stderr to a file.

#include <Beard/config.hpp>
#include <Beard/String.hpp>
#include <Beard/utility.hpp>
#include <Beard/Error.hpp>
#include <Beard/txt/Defs.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Ops.hpp>
#include <Beard/tty/TerminalInfo.hpp>
#include <Beard/tty/Terminal.hpp>

#include <duct/char.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <thread>

#include "../common/common.hpp"

using Beard::enum_cast;
using Beard::tty::make_cell;

static bool s_close = false;

void
render(
	Beard::tty::Terminal& term
) {
	term.put_cell(
		0u, 0u,
		make_cell(
			{'R'},
			Beard::tty::Color::red | Beard::tty::Attr::underline,
			Beard::tty::Color::cyan
		)
	);
	term.put_sequence(
		2u, 0u,
		{BEARD_STR_LIT("bold")},
		4u,
		Beard::tty::Attr::bold
	);
	term.put_sequence(
		7u, 0u,
		{BEARD_STR_LIT("blink")},
		5u,
		Beard::tty::Color::white,
		Beard::tty::Color::green | Beard::tty::Attr::blink
	);

	term.put_sequence(
		4u, 4u,
		{BEARD_STR_LIT("Hello, terminal overlord! 元気ですか？")},
		32u
	);
	term.present();
}

signed
main(
	signed argc,
	char* argv[]
) {
	if (2 > argc || 3 < argc) {
		std::cerr <<
			"invalid arguments\n"
			"usage: hello terminfo-file-path [tty-path]\n"
		;
		return -1;
	}

	std::cout
		<< "sizeof Terminal: "
		<< sizeof(Beard::tty::Terminal)
		<< '\n'
	;

	Beard::tty::Terminal term{};

	char const* const info_path = argv[1];
	if (!load_term_info(term.get_info(), info_path)) {
		return -2;
	}

	term.update_cache();

	bool use_sigwinch = false;
	Beard::String tty_path{};
	if (2 < argc) {
		tty_path.assign(argv[2]);
	} else {
		tty_path.assign(Beard::tty::this_path());
		use_sigwinch = true;
	}

	std::cout
		<< "opening tty: " << tty_path << '\n'
		<< "use_sigwinch = " << std::boolalpha << use_sigwinch
		<< std::endl
	;
	//std::this_thread::sleep_for(std::chrono::seconds{1});
	std::cout << "opening\n";

	try {
		term.open(tty_path, use_sigwinch);
	} catch (Beard::Error const& ex) {
		report_error(ex);
	}

	std::cout
		<< "size: "
		<< term.get_width()
		<< " x "
		<< term.get_height()
		<< '\n'
	;

	term.set_caret_pos(0u, 2u);
	term.set_caret_visible(false);
	render(term);

	Beard::txt::UTF8Block cpblock{};
	Beard::tty::Event ev{};
	while (!s_close) {
		switch (term.poll(ev, 5u)) {
		case Beard::tty::EventType::resize:
			std::cout
				<< std::dec
				<< "resized from "
				<< ev.resize.old_size.width << " x "
				<< ev.resize.old_size.height
				<< " to "
				<< term.get_width() << " x "
				<< term.get_height()
				<< std::endl
			;
			render(term);
			break;

		case Beard::tty::EventType::key_input:
			std::cout
				<< std::hex
				<< "key_input: "
				<< "mod = " << enum_cast(ev.key_input.mod) << "  "
				<< "code = " << enum_cast(ev.key_input.code) << "  "
				<< "cp = " << ev.key_input.cp
			;
			if (duct::CHAR_SENTINEL != ev.key_input.cp) {
				cpblock.assign(ev.key_input.cp);
				std::cout << " \'";
				std::cout.write(cpblock.units, cpblock.size());
				std::cout << '\'';
			}
			std::cout
				<< std::endl
			;
			if (Beard::KeyMod::ctrl == ev.key_input.mod
			 && 'c' == ev.key_input.cp
			) {
				s_close = true;
			}
			break;

		case Beard::tty::EventType::none:
		default:
			break;
		}
	}

	std::cout << "closing\n";
	term.close();

	return 0;
}
