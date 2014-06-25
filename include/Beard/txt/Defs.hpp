/**
@file txt/Defs.hpp
@brief Text definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/String.hpp>
#include <Beard/utility.hpp>

#include <duct/char.hpp>
#include <duct/EncodingUtils.hpp>

#include <cstring>

namespace Beard {
namespace txt {

// Forward declarations
enum class Extent : unsigned;
struct Sequence;
struct UTF8Block;

/**
	@addtogroup txt
	@{
*/

/**
	Encoding utilities.
*/
using EncUtils = duct::UTF8Utils;

/**
	%Extents.
*/
enum class Extent : unsigned {
	head = 0u,
	tail
};

/**
	%Sequence of code unit sequences.

	@note All data in the sequence is expected to be UTF-8 encoded.
*/
struct Sequence final {
/** @name Properties */ /// @{
	/** Data. */
	txt::EncUtils::char_type const* const data;
	/** Size. */
	std::size_t const size;
/// @}

/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Sequence() noexcept = default;

	/** Default constructor. */
	Sequence() noexcept = default;
	/** Copy constructor. */
	Sequence(Sequence const&) noexcept = default;
	/** Move constructor. */
	Sequence(Sequence&&) noexcept = default;

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
	constexpr
	Sequence(
		txt::EncUtils::char_type const (&data)[N]
	) noexcept
		: data(data)
		, size(N - 1)
	{}

	/**
		Constructor with arbitrary code unit sequence.

		@param data Code unit sequence.
		@param size Number of units in sequence.
	*/
	constexpr
	Sequence(
		txt::EncUtils::char_type const* const data,
		std::size_t const size
	) noexcept
		: data(data)
		, size(size)
	{}

	/**
		Constructor with string.

		@warning The sequence will be invalid if @a str changes.
		If this is given to a terminal, it will consume the units,
		so this is generally safe to use as long as @a str doesn't
		change beforehand.

		@param str %String.
		@param pos Position in string. Clamped to
		<code>str.size()</code>.
		@param size Size of sequence. Clamped to
		<code>str.size() - pos</code>.
	*/
	constexpr
	Sequence(
		String const& str,
		String::size_type const pos = 0u,
		String::size_type const size = String::npos
	) noexcept
		: data(str.data() + min_ce(pos, str.size()))
		, size(min_ce(size, str.size() - min_ce(pos, str.size())))
	{}
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	Sequence& operator=(Sequence const&) noexcept = default;
	/** Move assignment operator. */
	Sequence& operator=(Sequence&&) noexcept = default;
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
	txt::EncUtils::char_type units[txt::EncUtils::max_units];

	/**
		Get the number of units in the block.
	*/
	std::size_t
	size() const noexcept {
		return txt::EncUtils::required_first_whole(units[0u]);
	}
/// @}

/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~UTF8Block() noexcept = default;

	/** Default constructor. */
	UTF8Block() noexcept = default;
	/** Copy constructor. */
	UTF8Block(UTF8Block const&) noexcept = default;
	/** Move constructor. */
	UTF8Block(UTF8Block&&) noexcept = default;

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
		char32 const cp
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
		txt::EncUtils::char_type const* const data,
		std::size_t const size
	) noexcept
		: units{}
	{
		assign(data, size);
	}
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	UTF8Block& operator=(UTF8Block const&) noexcept = default;
	/** Move assignment operator. */
	UTF8Block& operator=(UTF8Block&&) noexcept = default;

	/**
		Assign to ASCII character.

		@param c ASCII character.
	*/
	UTF8Block&
	operator=(
		char const c
	) noexcept {
		units[0u] = c;
		return *this;
	}

	/**
		Assign to (decoded) code point.

		@param cp Code point to decode.
	*/
	UTF8Block&
	operator=(
		char32 const cp
	) noexcept {
		assign(cp);
		return *this;
	}
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
		char32 const cp
	) noexcept {
		txt::EncUtils::encode(
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
		txt::EncUtils::char_type const* const data,
		std::size_t const size
	) noexcept {
		std::memcpy(
			units,
			data,
			(txt::EncUtils::max_units < size)
				? txt::EncUtils::max_units
				: size
		);
	}
/// @}
};

/** @} */ // end of doc-group txt

} // namespace txt
} // namespace Beard

