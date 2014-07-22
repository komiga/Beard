/**
@file ui/Widget/Defs.hpp
@brief %Widget definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/utility.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/PropertyMap.hpp>

#include <duct/StateStore.hpp>

#include <memory>

namespace Beard {
namespace ui {

// Forward declarations
class Context; // external

namespace Widget {

// Forward declarations
class Base; // external
enum class Type : unsigned;
struct type_info;
enum class Flags : unsigned;
struct Slot;
struct RenderData;

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
	/** %Base standard type. */
	STANDARD_BASE = 0x01,
	/** Maximum standard type. */
	STANDARD_LIMIT = 0xFF,
	/** Number of (possible) standard types. */
	STANDARD_COUNT = STANDARD_LIMIT - STANDARD_BASE,
/// @}

/** @name Userspace range */ /// @{
	/** %Base userspace type. */
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
	/** ui::Field. */
	Field,
/** @} */

/** @cond INTERNAL */
	// One-past-end
	STANDARD_END,
	STANDARD_COUNT_DEFINED = STANDARD_END - STANDARD_BASE
/** @endcond */
};

/**
	%Widget flags.
*/
enum class Flags : unsigned {
	/**
		No flags.
	*/
	none			= 0x00,

	/**
		%Widget is focusable.
	*/
	trait_focusable	= bit(0u),
	/**
		%Widget is a container.

		Containers must implement the following functions:

		- ui::Widget::Base::num_children_impl()
		- ui::Widget::Base::get_child_impl()
	*/
	trait_container	= bit(1u),

	/**
		%Widget is enabled.
	*/
	enabled			= bit(2u),
	/**
		%Widget is visible.
	*/
	visible			= bit(3u),
	/**
		%Widget is focused.
	*/
	focused			= bit(4u),
	/**
		%Widget is in input control mode.
	*/
	input_control	= bit(5u),
	/**
		One or more queued update actions.
	*/
	queued_actions	= bit(6u),

	/**
		Mask for trait flags.
	*/
	trait_mask
		= trait_focusable
		| trait_container
	,

/** @cond INTERNAL */
	COUNT = 7u
/** @endcond */
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

/**
	%Widget render data.
*/
struct RenderData final {
	/** Context. */
	ui::Context& context;
	/** %Terminal. */
	tty::Terminal& terminal;
	/** %Property map. */
	ui::PropertyMap& property_map;

	/** Name of current group. */
	ui::group_hash_type group_name;
	/** Iterator to property group. */
	ui::PropertyMap::const_iterator it_group;
	/** Iterator to fallback property group. */
	ui::PropertyMap::const_iterator it_fallback;

	/**
		Update group.

		This will update the current group iterator iff @a name
		differs from the current group name.

		@param name Group to use.
	*/
	void
	update_group(
		ui::group_hash_type const name
	) {
		if (name != this->group_name) {
			this->it_group = property_map.find(name, ui::group_null);
			this->group_name = name;
		}
	}

	/**
		See ui::PropertyMap::get_number().
	*/
	ui::property_number_type
	get_number(
		ui::property_hash_type const name
	) const {
		return this->property_map.get_number(
			name,
			this->it_group,
			this->it_fallback
		);
	}

	/**
		See ui::PropertyMap::get_attr().
	*/
	ui::property_attr_type
	get_attr(
		ui::property_hash_type const name
	) const {
		return this->property_map.get_attr(
			name,
			this->it_group,
			this->it_fallback
		);
	}

	/**
		See ui::PropertyMap::get_boolean().
	*/
	ui::property_boolean_type
	get_boolean(
		ui::property_hash_type const name
	) const {
		return this->property_map.get_boolean(
			name,
			this->it_group,
			this->it_fallback
		);
	}

	/**
		See ui::PropertyMap::get_boolean().
	*/
	ui::property_string_type const&
	get_string(
		ui::property_hash_type const name
	) const {
		return this->property_map.get_string(
			name,
			this->it_group,
			this->it_fallback
		);
	}
};

/** @} */ // end of doc-group ui

} // namespace Widget
} // namespace ui
} // namespace Beard
