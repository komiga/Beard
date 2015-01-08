/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Error class.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/ErrorCode.hpp>
#include <Beard/String.hpp>

#include <duct/GR/Error.hpp>

namespace Beard {

// Forward declarations

/**
	@addtogroup error
	@{
*/

/**
	%Error.

	See @c duct::GR::Error for documentation.
*/
using Error
= duct::GR::Error<
	Beard::ErrorCode,
	Beard::String
>;

/**
	Get the name of an error.

	@returns C-string containing the name of @a error_code or
	"INVALID" if somehow @a error_code is not actually an ErrorCode.
	@param error_code ErrorCode.
*/
char const*
get_error_name(
	ErrorCode const error_code
) noexcept;

/** @} */ // end of doc-group error

} // namespace Beard
