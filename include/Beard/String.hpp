/**
@file String.hpp
@brief String type.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>

#include <duct/char.hpp>

namespace Beard {

/**
	@addtogroup etc
	@{
*/
/**
	@addtogroup string
	@{
*/

/**
	Turn input into a UTF-8 encoded string literal.

	@param x_ C-string literal.
*/
#define BEARD_STR_LIT(x_) u8 ## x_

/**
	String type.

	@note Contents must be UTF-8.
*/
using String = aux::basic_string<char>;

/**
	8-bit character (UTF-8 code unit).
*/
using char8 = duct::char8;

/**
	32-bit character type (Unicode code point).
*/
using char32 = duct::char32;

/** @} */ // end of doc-group string
/** @} */ // end of doc-group etc

} // namespace Beard

