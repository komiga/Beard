/**
@file String.hpp
@brief String type.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_STRING_HPP_
#define BEARD_STRING_HPP_

#include <Beard/config.hpp>
#include <Beard/aux.hpp>

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

/** @} */ // end of doc-group string
/** @} */ // end of doc-group etc

} // namespace Beard

#endif // BEARD_STRING_HPP_
