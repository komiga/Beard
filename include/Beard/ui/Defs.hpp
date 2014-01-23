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
#include <Beard/String.hpp>
#include <Beard/geometry.hpp>
#include <Beard/tty/Defs.hpp>

#include <am/hash/common.hpp>
#include <am/hash/fnv.hpp>

#include <memory>

namespace Beard {
namespace ui {

// Forward declarations
class Root; // external
enum class PropertyType : unsigned;
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
	Common hash type.
*/
using hash_type = am::hash::common_hash_type<am::hash::HL32>;

/**
	%Property hash type.
*/
using property_hash_type = ui::hash_type;

/**
	Group hash type.
*/
using group_hash_type = ui::hash_type;

/**
	Hash values.
*/
enum : ui::hash_type {
	/** Null hash value. */
	hash_null = ui::hash_type(0)
};

/**
	Hash string literal (constexpr).

	@tparam N Size of @a str; inferred from @a str.

	@returns Hash of @a str if <code>N > 0u</code>; @c ui::hash_null
	otherwise.
	@param str String to hash.
*/
template<
	std::size_t const N
>
constexpr ui::hash_type
hash(
	const char (&str)[N]
) noexcept {
	return (0u == N)
		? ui::hash_null
		: am::hash::fnv1a_c<am::hash::HL32>(str, N)
	;
}

/**
	Hash string.

	@tparam StringT String type; inferred from @a str. This type must
	have the same interface as @c std::basic_string<char>.

	@returns Hash of @a str if <code>N > 0u</code>; @c ui::hash_null
	otherwise.
	@param str String to hash.
*/
template<
	class StringT
>
ui::hash_type
hash(
	StringT const& str
) noexcept {
	return (str.empty())
		? ui::hash_null
		: am::hash::fnv1a_str<am::hash::HL32>(str)
	;
}

/**
	%Property type.
*/
enum class PropertyType : unsigned {
	number		= 1 << 0u,
	attr		= 1 << 1u,
	boolean		= 1 << 2u,
	string		= 1 << 3u
};

/** @name %Property value types */ /// @{
/** %Property number value type. */
using property_number_type = signed;

/** %Property attribute value type. */
using property_attr_type = tty::attr_type;

/** %Property boolean value type. */
using property_boolean_type = bool;

/** %Property string value type. */
using property_string_type = String;
/// @}

/** @cond INTERNAL */
#define BEARD_DPROP_(name) \
	property_ ## name = ui::hash(DUCT_STRINGIFY(name))
/** @endcond */

/**
	Pre-defined property names.
*/
enum : property_hash_type {
	/** Null property. */
	property_null = ui::hash_null,

/** @name Primary properties */ /// @{
	BEARD_DPROP_(primary_fg_inactive),
	BEARD_DPROP_(primary_bg_inactive),
	BEARD_DPROP_(primary_fg_active),
	BEARD_DPROP_(primary_bg_active),
/// @}

/** @name Content properties */ /// @{
	BEARD_DPROP_(content_fg_inactive),
	BEARD_DPROP_(content_bg_inactive),
	BEARD_DPROP_(content_fg_active),
	BEARD_DPROP_(content_bg_active),
/// @}

/** @name Frame properties */ /// @{
	BEARD_DPROP_(frame_enabled),
	BEARD_DPROP_(frame_debug_enabled),
	BEARD_DPROP_(frame_fg_inactive),
	BEARD_DPROP_(frame_bg_inactive),
	BEARD_DPROP_(frame_fg_active),
	BEARD_DPROP_(frame_bg_active),
/// @}

/** @name Field properties */ /// @{
	BEARD_DPROP_(field_content_underline),
/// @}
};

/** @cond INTERNAL */
#undef BEARD_DPROP_
/** @endcond */

/** @cond INTERNAL */
#define BEARD_DGROUP_(name) \
	group_ ## name = ui::hash(DUCT_STRINGIFY(name))
/** @endcond */

/**
	Pre-defined property group names.
*/
enum : group_hash_type {
	/** Null group. */
	group_null = ui::hash_null,

	/** Default group. */
	BEARD_DGROUP_(default),
	/** ui::Label group. */
	BEARD_DGROUP_(label),
	/** ui::Button group. */
	BEARD_DGROUP_(button),
	/** ui::Field group. */
	BEARD_DGROUP_(field),
};

/** @cond INTERNAL */
#undef BEARD_DGROUP_
/** @endcond */

/**
	Focus index.

	@note Only @c ui::focus_index_none will be ignored in focus maps.
*/
using focus_index_type = signed;

/**
	Special focus indices.
*/
enum : ui::focus_index_type {
	/** Non-participating focus index. */
	focus_index_none = ui::focus_index_type(-1),
	/** Default lazy index for focusable widgets. */
	focus_index_lazy = ui::focus_index_type(0)
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
