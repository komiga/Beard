/**
@file keys.hpp
@brief Key constants.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_KEYS_HPP_
#define BEARD_KEYS_HPP_

#include <Beard/config.hpp>
#include <Beard/String.hpp>

#include <duct/char.hpp>

namespace Beard {

// Forward declarations
enum class KeyMod : unsigned;
enum class KeyCode : unsigned;
struct KeyInputData;
struct KeyInputMatch;

/**
	@addtogroup keys
	@{
*/

/**
	Modifier keys.
*/
enum class KeyMod : unsigned {
	/** Absence of modifier. */
	none = 0u,
	/** Escape or alt. */
	esc   = 1u << 0,
	/** Control. */
	ctrl  = 1u << 1,
	/**
		Shift.

		@note This will only ever be used for KeyCodes -- not
		code points.
	*/
	shift = 1u << 2
};

/**
	Mostly-unprintable key codes.
*/
enum class KeyCode : unsigned {
	none = 0u,

	esc,
	backspace,
	enter,

	insert,
	del,
	home,
	end,
	pgup,
	pgdn,

	up,
	down,
	left,
	right,

	tab,
	f1,
	f2,
	f3,
	f4,
	f5,
	f6,
	f7,
	f8,
	f9,
	f10,
	f11,
	f12
};

enum : char32 {
	/**
		Non-Unicode value used to represent a "none" state.
	*/
	codepoint_none = duct::CHAR_SENTINEL
};

/**
	Key input event data.

	This is used to represent event data.
*/
struct KeyInputData final {
	/** Key modifier. */
	KeyMod mod;
	/** Key code. */
	KeyCode code;
	/** Code point. */
	char32 cp;
};

/**
	Key input match.
*/
struct KeyInputMatch final {
	/** Key modifier. */
	KeyMod mod;
	/** Key code. */
	KeyCode code;
	/** Code point. */
	char32 cp;

	/**
		Whether to match any modifiers or only the specified
		modifiers.
	*/
	bool mod_any;
};

/**
	Match key input data.

	@param data Key input data to compare against.
	@param match Key input match.
*/
inline constexpr bool
key_input_match(
	KeyInputData const& data,
	KeyInputMatch const& match
) noexcept {
	return
		(
			match.mod_any ||
			data.mod == match.mod
		) && (
			data.code == match.code &&
			data.cp == match.cp
		)
	;
}

/**
	Match key input data from an array.

	@returns Pointer to matching data, or @c nullptr if none match.
	@param data Key input data to compare against.
	@param matches List of key input matches.
*/
template<
	std::size_t N
>
KeyInputMatch const*
key_input_match(
	KeyInputData const& data,
	KeyInputMatch const (&matches)[N]
) noexcept {
	for (auto const& match : matches) {
		if (key_input_match(data, match)) {
			return &match;
		}
	}
	return nullptr;
}

/** @} */ // end of doc-group keys

} // namespace Beard

#endif // BEARD_KEYS_HPP_
