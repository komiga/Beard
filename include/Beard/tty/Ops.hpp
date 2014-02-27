/**
@file tty/Ops.hpp
@brief %Terminal operations.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_TTY_OPS_HPP_
#define BEARD_TTY_OPS_HPP_

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

#endif // BEARD_TTY_OPS_HPP_
