
#ifndef BEARD_TEST_COMMON_COMMON_HPP_
#define BEARD_TEST_COMMON_COMMON_HPP_

#include <Beard/geometry.hpp>
#include <Beard/String.hpp>
#include <Beard/Error.hpp>
#include <Beard/tty/TerminalInfo.hpp>
#include <Beard/ui/Geom.hpp>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>

#include "../common/common.hpp"

std::ostream&
operator<<(
	std::ostream& stream,
	Beard::Axis const axis
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
	Beard::Vec2 const& v
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
	Beard::Rect const& rect
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
	Beard::ui::Geom const& geom
) {
	return stream
		<< "{\n"
		<< "  area  = " << geom.get_area() << ",\n"
		<< "  frame = " << geom.get_frame() << ",\n"
		<< "  request_size = " << geom.get_request_size() << ",\n"
		<< std::boolalpha
		<< "  expand = " << geom.get_expand() << ",\n"
		<< "  fill   = " << geom.get_fill()
		<< "\n}"
	;
}

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
