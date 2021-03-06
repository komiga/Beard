
#pragma once

#include <Beard/geometry.hpp>
#include <Beard/String.hpp>
#include <Beard/Error.hpp>
#include <Beard/tty/TerminalInfo.hpp>
#include <Beard/keys.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Context.hpp>
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
		<< "  area  = " << geom.area() << ",\n"
		<< "  frame = " << geom.frame() << ",\n"
		<< "  request_size = " << geom.request_size() << ",\n"
		<< std::boolalpha
		<< "  expand = " << geom.expand() << ",\n"
		<< "  fill   = " << geom.fill()
		<< "\n}"
	;
}

void
report_error(
	Beard::Error const& e
) {
	std::cerr
		<< '[' << Beard::get_error_name(e.code()) << ']'
		<< '\n' << e.message()
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

namespace {

static Beard::KeyInputMatch const
s_kim_c{Beard::KeyMod::ctrl , Beard::KeyCode::none,  'c', false};

} // anonymous namespace

// Returns false if the program should terminate
bool
context_update(
	Beard::ui::Context& context,
	unsigned const timeout_ms = 10u
) {
	if (!context.update(timeout_ms)) {
		auto const& event = context.last_event();
		if (Beard::ui::EventType::key_input == event.type) {
			if (Beard::key_input_match(event.key_input, s_kim_c)) {
				return false;
			}
		}
	}
	return true;
}
