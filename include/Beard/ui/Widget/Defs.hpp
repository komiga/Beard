/**
@file ui/Widget/Defs.hpp
@brief %Widget definitions.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_WIDGET_DEFS_HPP_
#define BEARD_UI_WIDGET_DEFS_HPP_

#include <Beard/config.hpp>
#include <Beard/aux.hpp>

#include <duct/StateStore.hpp>

#include <memory>

namespace Beard {
namespace ui {
namespace Widget {

// Forward declarations
class Base; // external
enum class Type : unsigned;
enum class TypeFlags : unsigned;
struct type_info;
enum class Flags : unsigned;
struct Slot;

/**
	@addtogroup ui
	@{
*/

/**
	Shared widget pointer.
*/
using SPtr = aux::shared_ptr<ui::Widget::Base>;

/**
	Weak widget pointer.
*/
using WPtr = aux::weak_ptr<ui::Widget::Base>;

/**
	%Widget type.

	@remarks The range <code>[0x01, 0xFF]</code> is reserved for 
	standard types. Userspace is permitted the range
	<code>[0x00000100, 0xFFFFFFFF]</code>.
*/
enum class Type : unsigned {
/** @name Standard range */ /// @{
	/** Base standard type. */
	STANDARD_BASE = 0x01,
	/** Maximum standard type. */
	STANDARD_LIMIT = 0xFF,
	/** Number of (possible) standard types. */
	STANDARD_COUNT = STANDARD_LIMIT - STANDARD_BASE,
/// @}

/** @name Userspace range */ /// @{
	/** Base userspace type. */
	USERSPACE_BASE = 0x00000100,
	/** Maximum userspace type. */
	USERSPACE_LIMIT = 0xFFFFFFFF,
/// @}

/**
	Standard types.
	@{
*/
	/** ui::Root. */
	Root = STANDARD_BASE,
	/** ui::Container. */
	Container,
	/** ui::Label. */
	Label,
	/** ui::Button. */
	Button,
/** @} */

/** @cond INTERNAL */
	// One-past-end
	STANDARD_END,
	STANDARD_COUNT_DEFINED = STANDARD_END - STANDARD_BASE
/** @endcond */
};

/**
	%Widget type flags.
*/
enum class TypeFlags : unsigned {
	/**
		No type flags.
	*/
	none			= 0u,
	/**
		Type is focusable.
	*/
	focusable		= 1u << 0,
	/**
		Type is a container.
	*/
	container		= 1u << 1,
};

/**
	%Widget type information.
*/
struct type_info final {
	/** Widget type. */
	ui::Widget::Type const type;

	/** Type flags. */
	duct::StateStore<ui::Widget::TypeFlags> const type_flags;
};

/**
	%Widget flags.
*/
enum class Flags : unsigned {
	// NB: If flags are added/changed, remember to update shift_ua
	// in ui::Widget::Base

	/**
		No flags.
	*/
	none			= 0x00,
	/**
		%Widget is enabled.
	*/
	enabled			= 1u << 0,
	/**
		%Widget is visible.
	*/
	visible			= 1u << 1,
	/**
		%Widget is focused.
	*/
	focused			= 1u << 2,
	/**
		%Widget is in input control mode.
	*/
	input_control	= 1u << 3,
	/**
		One or more queued update actions.
	*/
	queued_actions	= 1u << 4,
};

/**
	%Widget slot.
*/
struct Slot final {
/** @name Properties */ /// @{
	/** %Widget. */
	ui::Widget::SPtr widget;

	/** Calculated area. */
	Rect area;
/// @}
};

/**
	%Slot vector.
*/
using slot_vector_type = aux::vector<ui::Widget::Slot>;

/** @} */ // end of doc-group ui

} // namespace Widget
} // namespace ui
} // namespace Beard

#endif // BEARD_UI_WIDGET_DEFS_HPP_
