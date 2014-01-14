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
class Root; // external
enum class FocusDir : unsigned;
enum class UpdateActions : unsigned;
enum class EventType : unsigned;
struct Event;

/**
	@addtogroup ui
	@{
*/

/**
	Shared root pointer.
*/
using RootSPtr = aux::shared_ptr<ui::Root>;

/**
	Weak root pointer.
*/
using RootWPtr = aux::weak_ptr<ui::Root>;

/**
	Focus index.

	@note Only @c focus_index_none will be ignored in focus maps.
*/
using focus_index_type = signed;

enum : focus_index_type {
	/** Non-participating focus index. */
	focus_index_none = focus_index_type(-1),
	/** Default lazy index for focusable widgets. */
	focus_index_lazy = focus_index_type(0)
};

/**
	Focus direction.
*/
enum class FocusDir : unsigned {
	/** Previous widget. */
	prev,
	/** Next widget. */
	next
};

/**
	%Widget context-update actions.

	%Widget actions to perform on a context update.
*/
enum class UpdateActions : unsigned {
	/**
		No actions.
	*/
	none			= 0x00,
	/**
		Whether to perform actions on the parent.
	*/
	flag_parent		= 1u << 0,
	/**
		Reflow.
	*/
	reflow			= 1u << 1,
	/**
		Render (after reflowing, if set).
	*/
	render			= 1u << 2,

	/**
		Mask with all actions.
	*/
	mask_all
		= reflow
		| render
	,
};

/**
	%Event type.

	@sa ui::Event
*/
enum class EventType : unsigned {
	/** No-event type. */
	none = 0u,
	/** Key input event type. */
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
