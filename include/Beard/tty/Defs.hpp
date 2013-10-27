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

namespace Beard {
namespace tty {

// Forward declarations

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

/** @} */ // end of doc-group tty

} // namespace tty
} // namespace Beard

#endif // BEARD_TTY_DEFS_HPP_
