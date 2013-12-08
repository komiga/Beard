
#ifndef BEARD_TEST_COMMON_COMMON_HPP_
#define BEARD_TEST_COMMON_COMMON_HPP_

#include <Beard/String.hpp>
#include <Beard/Error.hpp>
#include <Beard/tty/TerminalInfo.hpp>

#include <iostream>
#include <fstream>

void
report_error(
	Beard::Error const& e
) {
	std::cerr
		<< '[' << Beard::get_error_name(e.get_code()) << ']'
		<< '\n' << e.get_message()
		<< '\n'
	<< std::endl;
}

bool
load_term_info(
	Beard::tty::TerminalInfo& term_info,
	Beard::String const& path
) {
	std::ifstream stream{path};
	if (stream.fail() || !stream.is_open()) {
		std::cerr
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

#endif // BEARD_TEST_COMMON_COMMON_HPP_
