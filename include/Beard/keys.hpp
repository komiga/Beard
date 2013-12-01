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

namespace Beard {

// Forward declarations
enum class KeyMod : unsigned;
enum class KeyCode : unsigned;

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
	shift = 1u << 2,
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

/** @} */ // end of doc-group keys

} // namespace Beard

#endif // BEARD_KEYS_HPP_
