/**
@file
@brief %Error codes.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

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
		Read or write operation failed during serialization.
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

	/**
		Child insertion of or assignment to a null widget.
	*/
	ui_container_null_widget,
/// @}

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/** @} */ // end of doc-group error

} // namespace Beard
