
// usage: hello <terminfo-file-path> [tty-path]

#include <Beard/config.hpp>
#include <Beard/Error.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Ops.hpp>
#include <Beard/tty/TerminalInfo.hpp>
#include <Beard/tty/Terminal.hpp>

#include <signal.h>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>

#include "../common/common.hpp"

using Beard::tty::make_cell;

static bool s_close = false;

void
sigint_handler(int) noexcept {
	s_close = true;
}

bool
load_term_info(
	Beard::tty::TerminalInfo& term_info,
	Beard::String const& path
) {
	std::ifstream stream{path};
	if (stream.fail() || !stream.is_open()) {
		std::cout
			<< "failed to open terminfo path for reading: '"
			<< path
			<< "'\n"
		;
		return false;
	}

	try {
		term_info.deserialize(stream);
	} catch (...) {
		// Just rethrow and let the program explode
		throw;
	}
	stream.close();
	return true;
}

signed
main(
	int argc,
	char* argv[]
) {
	if (2 > argc || 3 < argc) {
		std::cerr <<
			"invalid arguments\n"
			"usage: hello <terminfo-file-path> [tty-path]\n"
		;
		return -1;
	}

	std::cout
		<< "sizeof Terminal: "
		<< sizeof(Beard::tty::Terminal)
		<< '\n'
	;

	struct ::sigaction sig;
	sig.sa_handler = sigint_handler;
	sig.sa_flags = 0;
	if (0 != ::sigaction(SIGINT, &sig, nullptr)) {
		std::cerr
			<< "failed to set SIGINT handler\n"
		;
		return -3;
	}

	Beard::tty::Terminal term{};

	char const* const info_path = argv[1];
	if (!load_term_info(term.get_info(), info_path)) {
		return -2;
	}

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
	std::this_thread::sleep_for(std::chrono::seconds{1});
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
		Beard::tty::Attr::bold
	);
	term.put_sequence(
		7u, 0u,
		{BEARD_STR_LIT("blink")},
		Beard::tty::Color::white,
		Beard::tty::Color::green | Beard::tty::Attr::blink
	);

	term.put_sequence(
		4u, 4u,
		{BEARD_STR_LIT("Hello, terminal overlord! 元気ですか？")}
	);

	term.set_caret_pos(0u, 1u);
	term.set_caret_visible(true);

	Beard::tty::Event ev;
	term.render();
	while (!s_close) {
		// std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::milliseconds{1u});
		switch (term.poll(ev)) {
		case Beard::tty::EventType::resize:
			std::cout
				<< "resized from "
				<< ev.resize.old_width << " x " << ev.resize.old_width
				<< " to "
				<< term.get_width() << " x " << term.get_height()
				<< std::endl
			;
			term.render();
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
