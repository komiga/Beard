
#ifndef BEARD_TEST_COMMON_COMMON_HPP_
#define BEARD_TEST_COMMON_COMMON_HPP_

#include <Beard/Error.hpp>

#include <iostream>

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

#endif // BEARD_TEST_COMMON_COMMON_HPP_
