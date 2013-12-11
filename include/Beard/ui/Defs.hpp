/**
@file ui/Defs.hpp
@brief UI definitions.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_DEFS_HPP_
#define BEARD_UI_DEFS_HPP_

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/utility.hpp>
#include <Beard/keys.hpp>
#include <Beard/geometry.hpp>
#include <Beard/tty/Defs.hpp>

#include <memory>

namespace Beard {
namespace ui {

// Forward declarations
class Widget; // external
enum class WidgetType : unsigned;
struct Slot;
enum class EventType : unsigned;
struct Event;

/**
	@addtogroup ui
	@{
*/

/**
	%Widget shared pointer.
*/
using WidgetSPtr = std::shared_ptr<ui::Widget>;

/**
	%Widget weak pointer.
*/
using WidgetWPtr = std::weak_ptr<ui::Widget>;

// TODO: Userspace
/**
	%Widget type.
*/
enum class WidgetType : unsigned {
	/** ui::Container. */
	Container = 0u,
	/** ui::Button. */
	Button,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	%Widget slot.
*/
struct Slot final {
/** @name Properties */ /// @{
	/** %Widget. */
	ui::WidgetSPtr widget;

	/** Calculated area. */
	Rect area;
/// @}
};

/**
	%Slot vector.
*/
using slot_vector_type = aux::vector<ui::Slot>;

/**
	%Event type.
*/
enum class EventType : unsigned {
	none = 0u,
	key_input,
};

/**
	%Event.

	@sa ui::EventType
*/
struct Event final {
/** @name Properties */ /// @{
	/**
		Type.
	*/
	ui::EventType type;

	/**
		%Event data for ui::EventType::key_input.

		This is triggered by a tty::EventType::key_input event.
	*/
	KeyInputData key_input;
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_DEFS_HPP_
