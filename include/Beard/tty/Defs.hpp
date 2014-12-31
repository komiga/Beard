/**
@file tty/Defs.hpp
@brief TTY definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/keys.hpp>
#include <Beard/geometry.hpp>
#include <Beard/txt/Defs.hpp>

namespace Beard {
namespace tty {

// Forward declarations
struct Cell;

enum class EventType : unsigned;
struct Event;

/**
	@addtogroup tty
	@{
*/

/**
	File descriptor type.
*/
using fd_type = signed;

enum : tty::fd_type {
	/**
		Invalid file descriptor.
	*/
	FD_INVALID = tty::fd_type{-1}
};

/**
	Color + attribute type.
*/
using attr_type = std::uint16_t;

/**
	%Attributes.
*/
namespace Attr {
enum enum_type : tty::attr_type {
	none		= 0,
	bold		= 0x0100,
	underline	= 0x0200,
	inverted	= 0x0400,
	blink		= 0x0800,
	mask		= 0x0F00,
};
}

/**
	%Colors.
*/
namespace Color {
enum enum_type : tty::attr_type {
	// NB: Unless c == term_default, c - 1 for actual digit in
	// terminal color (3<c>, 4<c>)

	/** %Terminal default color. */
	term_default	= 0x00,
	black			= 0x01,
	red				= 0x02,
	green			= 0x03,
	yellow			= 0x04,
	blue			= 0x05,
	magenta			= 0x06,
	cyan			= 0x07,
	white			= 0x08,
	// TODO: What is the canonical 38/48?
};
}

/**
	%Terminal cell.

	@sa tty::Terminal,
		txt::UTF8Block,
		tty::Attr,
		tty::Color
*/
struct Cell final {
/** @name Properties */ /// @{
	/** UTF-8 code unit block. */
	txt::UTF8Block u8block;

	/** Foreground color and attributes. */
	tty::attr_type attr_fg;
	/** Background color and attributes. */
	tty::attr_type attr_bg;
/// @}
};

/**
	Construct a cell.

	@param u8block Code unit block.
	@param attr_fg Foreground attributes.
	@param attr_bg Background attributes.
*/
inline constexpr tty::Cell
make_cell(
	txt::UTF8Block&& u8block,
	tty::attr_type attr_fg = tty::Color::term_default,
	tty::attr_type attr_bg = tty::Color::term_default
) noexcept {
	return tty::Cell{
		u8block,
		attr_fg,
		attr_bg
	};
}

/** @name Pre-defined cells and frames */ /// @{

static constexpr Cell const
	/**
		Default buffer cell.
	*/
	s_cell_default = tty::make_cell(
		' ',
		tty::Color::term_default,
		tty::Color::term_default
	)
;

static txt::UTF8Block const
	/**
		Single-lined frame.
	*/
	s_frame_single[]{
		{U'┌'},
		{U'─'},
		{U'┐'},
		{U'│'},
		{U'┘'},
		{U'─'},
		{U'└'},
		{U'│'}
	},
	/**
		Heavy single-lined frame.
	*/
	s_frame_single_heavy[]{
		{U'┏'},
		{U'━'},
		{U'┓'},
		{U'┃'},
		{U'┛'},
		{U'━'},
		{U'┗'},
		{U'┃'}
	},
	/**
		Double-lined frame.
	*/
	s_frame_double[]{
		{U'╔'},
		{U'═'},
		{U'╗'},
		{U'║'},
		{U'╝'},
		{U'═'},
		{U'╚'},
		{U'║'}
	}
;

/// @}

/**
	%Event types.

	@sa tty::Event
*/
enum class EventType : unsigned {
	none = 0u,
	resize,
	key_input,
};

/**
	%Event.

	@sa tty::EventType
*/
struct Event final {
	/**
		Type.
	*/
	tty::EventType type{tty::EventType::none};

	/**
		%Event data for tty::EventType::resize.

		@sa tty::Terminal::get_size()
	*/
	struct {
		/** Old size of terminal. */
		Vec2 old_size{0, 0};
	} resize{};

	/**
		%Event data for tty::EventType::key_input.
	*/
	KeyInputData key_input{};
};

/** @} */ // end of doc-group tty

} // namespace tty
} // namespace Beard
