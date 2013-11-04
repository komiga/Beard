/**
@file tty/Defs.hpp
@brief TTY definitions.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_TTY_DEFS_HPP_
#define BEARD_TTY_DEFS_HPP_

#include <Beard/config.hpp>
#include <Beard/String.hpp>

#include <duct/char.hpp>
#include <duct/EncodingUtils.hpp>

#include <cstring>

namespace Beard {
namespace tty {

// Forward declarations
struct Sequence;
struct UTF8Block;
struct Cell;

enum class EventType : unsigned;
enum class KeyMod : unsigned;
enum class KeyCode : unsigned;
union Event;

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
	%Attributes.
*/
namespace Attr {
enum enum_type : unsigned {
	bold		= 0x0100,
	underline	= 0x0200,
	inverted	= 0x0400,
	blink		= 0x0800
};
}

/**
	%Colors.
*/
namespace Color {
enum enum_type : unsigned {
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
	white			= 0x08
	// TODO: What is the canonical 38/48?
};
}

/**
	%Terminal encoding utilities.
*/
using EncUtils = duct::UTF8Utils;

/**
	%Sequence of code unit sequences.

	@note All data in the sequence is expected to be UTF-8 encoded.

	@sa tty::Cell
*/
struct Sequence final {
/** @name Properties */ /// @{
	/** Data. */
	tty::EncUtils::char_type const* const data;
	/** Size. */
	std::size_t const size;
/// @}

/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Sequence() = default;

	/** Default constructor. */
	Sequence() = default;
	/** Copy constructor. */
	Sequence(Sequence const&) = default;
	/** Move constructor (deleted). */
	Sequence(Sequence&&) = delete;

	/**
		Constructor with n-ary C string.

		@note The expected input is NUL-terminated, so the result
		sequence has @c size==N-1.

		@tparam N Number of elements in @a data; inferred.
		@param data Sequence data.
	*/
	template<
		std::size_t const N
	>
	Sequence(
		tty::EncUtils::char_type const (&data)[N]
	) noexcept
		: data(data)
		, size(N - 1)
	{}

	/**
		Constructor with arbitrary code unit sequence.

		@param data Code unit sequence.
		@param size Number of units in sequence.
	*/
	Sequence(
		tty::EncUtils::char_type const* const data,
		std::size_t const size
	) noexcept
		: data(data)
		, size(size)
	{}

	/**
		Constructor with string.

		@warning The sequence will be invalid if @a str changes
		the terminal consumes it.

		@param str %String.
	*/
	Sequence(
		String const& str
	) noexcept
		: data(str.data())
		, size(str.size())
	{}
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	Sequence& operator=(Sequence const&) = default;
	/** Move assignment operator (deleted). */
	Sequence& operator=(Sequence&&) = default;
/// @}
};

/**
	UTF-8 block of code units.

	@note This is used to store an entire UTF-8 code unit sequence
	representing a single code point.
*/
struct UTF8Block final {
/** @name Properties */ /// @{
	/** UTF-8 code units. */
	tty::EncUtils::char_type units[tty::EncUtils::max_units];

	/**
		Get the number of units in the block.
	*/
	std::size_t
	size() const noexcept {
		return tty::EncUtils::required_first_whole(units[0u]);
	}
/// @}

/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~UTF8Block() = default;

	/** Default constructor. */
	UTF8Block() = default;
	/** Copy constructor. */
	UTF8Block(UTF8Block const&) = default;
	/** Move constructor. */
	UTF8Block(UTF8Block&&) = default;

	/**
		Constructor with ASCII character.

		@param c ASCII character.
	*/
	constexpr
	UTF8Block(
		char const c
	) noexcept
		: units{c}
	{}

	/**
		Constructor with (decoded) code point.

		@param cp Code point to decode.
	*/
	UTF8Block(
		duct::char32 const cp
	) noexcept
		: units{}
	{
		assign(cp);
	}

	/**
		Constructor with arbitrary code unit sequence.

		@param data Code unit sequence.
		@param size Number of units in sequence.
	*/
	UTF8Block(
		tty::EncUtils::char_type const* const data,
		std::size_t const size
	) noexcept
		: units{}
	{
		assign(data, size);
	}
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	UTF8Block& operator=(UTF8Block const&) = default;
	/** Move assignment operator. */
	UTF8Block& operator=(UTF8Block&&) = default;
/// @}

/** @name Operations */ /// @{
	/**
		Assign to ASCII character.

		@param c ASCII character.
	*/
	inline void
	assign(
		char const c
	) noexcept {
		units[0u] = c;
	}

	/**
		Assign to code point.

		@a cp is decoded to UTF-8 code points and stored
		in @c this->units.

		@note If @a cp is invalid, @c this->units holds U+FFFD in
		UTF-8.

		@param cp Code point to decode.
	*/
	inline void
	assign(
		duct::char32 const cp
	) noexcept {
		tty::EncUtils::encode(
			cp,
			std::begin(units),
			duct::CHAR_REPLACEMENT
		);
	}

	/**
		Assign to arbitrary code unit sequence.

		@warning @a size will be clamped to the maximum number of
		units in a UTF-8 code unit sequence.

		@warning @a data is not checked for erroneous encoding.

		@param data Code unit sequence.
		@param size Number of units to copy.
	*/
	inline void
	assign(
		tty::EncUtils::char_type const* const data,
		std::size_t const size
	) noexcept {
		std::memcpy(
			units,
			data,
			(tty::EncUtils::max_units < size)
				? tty::EncUtils::max_units
				: size
		);
	}
/// @}
};

/**
	%Terminal cell.

	@sa tty::Terminal,
		tty::UTF8Block,
		tty::Attr,
		tty::Color
*/
struct Cell final {
/** @name Properties */ /// @{
	/** UTF-8 code unit block. */
	tty::UTF8Block u8block;

	/** Foreground color and attributes. */
	uint16_t attr_fg;
	/** Background color and attributes. */
	uint16_t attr_bg;
/// @}
};

/**
	Create a cell with default colors.

	@returns The created cell.
	@param u8block Code unit block.
	@param attr_fg Foreground attributes.
	@param attr_bg Background attributes.
*/
constexpr inline tty::Cell
make_cell(
	tty::UTF8Block&& u8block,
	uint16_t attr_fg = tty::Color::term_default,
	uint16_t attr_bg = tty::Color::term_default
) noexcept {
	return tty::Cell{
		u8block,
		attr_fg,
		attr_bg
	};
}

/**
	%Event types.

	@sa tty::Event
*/
enum class EventType : unsigned {
	none = 0u,
	resize,
	key_input,
};

// TODO
enum class KeyMod : unsigned {};
enum class KeyCode : unsigned {};

/**
	%Event.

	@sa tty::EventType
*/
union Event final {
	/**
		%Event data for tty::EventType::resize.

		@sa tty::Terminal::get_width(),
			tty::Terminal::get_height()
	*/
	struct {
		/** Old width of terminal. */
		unsigned old_width;
		/** Old height of terminal. */
		unsigned old_height;
	} resize;

	/**
		%Event data for tty::EventType::key_input.
	*/
	struct {
		/** Code point. */
		duct::char32 cp;
		/** Key modifier. */
		KeyMod mod;
		/** Key code. */
		KeyCode code;
	} key_input;
};

/** @} */ // end of doc-group tty

} // namespace tty
} // namespace Beard

#endif // BEARD_TTY_DEFS_HPP_
