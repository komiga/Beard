/**
@file tty/Ops.hpp
@brief %Terminal operations.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
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
