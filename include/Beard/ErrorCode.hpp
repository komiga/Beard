/**
@file ErrorCode.hpp
@brief %Error codes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_ERRORCODE_HPP_
#define BEARD_ERRORCODE_HPP_

#include <Beard/config.hpp>

namespace Beard {

// Forward declarations
enum class ErrorCode : unsigned;

/**
	@addtogroup error
	@{
*/

// FIXME: Doxygen borks all over itself
// when name-groups are used inside enums.

/**
	%Error codes.

	@sa Error
*/
enum class ErrorCode : unsigned {
/** @name General */ /// @{
	/**
		Unknown/unspecified.
	*/
	unknown = 0u,
/// @}

/** @name Serialization */ /// @{
	/**
		Write or read operation failed during deserialization or
		serialization.
	*/
	serialization_io_failed,
	/**
		Encountered malformed data during deserialization.
	*/
	serialization_data_malformed,
/// @}

/** @name TTY */ /// @{
	/**
		Terminal is already open.
	*/
	tty_terminal_already_open,
	/**
		Terminal information is uninitialized.
	*/
	tty_terminal_info_uninitialized,
	/**
		Another terminal already owns the @c SIGWINCH handler.
	*/
	tty_sigwinch_handler_already_active,
	/**
		Failed to fetch or modify the @c SIGWINCH handler.
	*/
	tty_sigaction_failed,
	/**
		Failed to open a terminal device file.
	*/
	tty_device_open_failed,
	/**
		File descriptor does not refer to a terminal or is otherwise
		invalid.
	*/
	tty_invalid_fd,

	/**
		Failed to setup terminal.
	*/
	tty_init_failed,
/// @}

/** @name UI */ /// @{
	/**
		%Property name is invalid.
	*/
	ui_invalid_property,
	/**
		Group name is invalid.
	*/
	ui_invalid_group,
	/**
		%Property name does not exist.
	*/
	ui_property_not_found,

	/**
		Context is already open.
	*/
	ui_context_already_open,
/// @}

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/** @} */ // end of doc-group error

} // namespace Beard

#endif // BEARD_ERRORCODE_HPP_
