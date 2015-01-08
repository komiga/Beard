/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Terminal operations.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/String.hpp>

namespace Beard {
namespace tty {

// Forward declarations

/**
	@addtogroup tty
	@{
*/

/**
	Get the pathname to the process's controlling terminal.
*/
String
this_path() noexcept;

/** @} */ // end of doc-group tty

} // namespace tty
} // namespace Beard
