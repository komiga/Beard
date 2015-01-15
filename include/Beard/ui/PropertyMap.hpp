/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Property map class.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/PropertyValue.hpp>
#include <Beard/ui/PropertyGroup.hpp>

#include <initializer_list>
#include <utility>

#include <Beard/detail/gr_core.hpp>

namespace Beard {
namespace ui {

// Forward declarations
class PropertyMap;

/**
	@addtogroup ui
	@{
*/

/**
	%Property group collection.
*/
class PropertyMap final {
public:
	/** %Property group map type. */
	using map_type = aux::unordered_map<
		ui::group_hash_type,
		ui::PropertyGroup
	>;

	/** Key-value pair type. */
	using pair_type = map_type::value_type;

	/** Group iterator. */
	using iterator = map_type::iterator;
	/** Const group iterator. */
	using const_iterator = map_type::const_iterator;

private:
	map_type m_groups;

	PropertyMap(PropertyMap const&) = delete;
	PropertyMap& operator=(PropertyMap const&) = delete;

	PropertyValue const*
	get_property(
		ui::property_hash_type const name,
		const_iterator group,
		const_iterator fallback
	) const;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~PropertyMap() = default;

	/**
		Default constructor.

		@note Equivalent to <code>ui::PropertyMap map{true}</code>.
	*/
	PropertyMap()
		: PropertyMap(true)
	{}

	/**
		Constructor with optional default group.

		@param emplace_default Whether to emplace the default group
		as @c ui::group_default.
	*/
	PropertyMap(
		bool const emplace_default
	)
		: m_groups()
	{
		if (emplace_default) {
			emplace(ui::group_default, ui::PropertyGroup::get_default());
		}
	}

	/**
		Constructor with initializer list.

		@param ilist Initializer list.
		@param emplace_default Whether to emplace the default group
		as @c ui::group_default.
	*/
	PropertyMap(
		std::initializer_list<pair_type> ilist,
		bool const emplace_default = true
	)
		: m_groups(std::move(ilist))
	{
		if (emplace_default) {
			emplace(ui::group_default, ui::PropertyGroup::get_default());
		}
	}

	/** Move constructor. */
	PropertyMap(PropertyMap&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	PropertyMap& operator=(PropertyMap&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get number of groups.
	*/
	std::size_t
	size() const noexcept {
		return m_groups.size();
	}

	/**
		Check if the group is empty.
	*/
	bool
	empty() const noexcept {
		return m_groups.empty();
	}

	/**
		Get iterator to beginning.
	*/
	iterator
	begin() noexcept {
		return m_groups.begin();
	}
	/**
		Get iterator to beginning.
	*/
	const_iterator
	begin() const noexcept {
		return m_groups.begin();
	}
	/**
		Get iterator to beginning.
	*/
	const_iterator
	cbegin() const noexcept {
		return m_groups.cbegin();
	}

	/**
		Get iterator to end.
	*/
	iterator
	end() noexcept {
		return m_groups.end();
	}
	/**
		Get iterator to end.
	*/
	const_iterator
	end() const noexcept {
		return m_groups.end();
	}
	/**
		Get iterator to end.
	*/
	const_iterator
	cend() const noexcept {
		return m_groups.cend();
	}
/// @}

/** @name Lookup */ /// @{
	/**
		Check if map contains a group.

		@param name Group name.
	*/
	bool
	contains(
		ui::group_hash_type const name
	) const noexcept {
		return (ui::group_null == name)
			? false
			: m_groups.count(name)
		;
	}

	/**
		Find group by name.

		@returns Iterator to @a name or @a fallback if either exist;
		end() otherwise.
		@param name Group name.
		@param fallback Fallback group name; unused if equal
		to @c ui::group_null.
	*/
	iterator
	find(
		ui::group_hash_type name,
		ui::group_hash_type fallback = ui::group_default
	) noexcept {
		if (ui::group_null == name) {
			name = fallback;
			fallback = ui::group_null;
		} else if (name == fallback) {
			fallback = ui::group_null;
		}

		if (ui::group_null == name) {
			return end();
		} else {
			auto const it = m_groups.find(name);
			return (end() == it && ui::group_null != fallback)
				? m_groups.find(fallback)
				: it
			;
		}
	}

	/**
		Find group by name.

		@returns Iterator to @a name or @a fallback if either exist;
		cend() otherwise.
		@param name Group name.
		@param fallback Fallback group name; unused if equal
		to @c ui::group_null.
	*/
	const_iterator
	find(
		ui::group_hash_type name,
		ui::group_hash_type fallback = ui::group_default
	) const noexcept {
		if (ui::group_null == name) {
			name = fallback;
			fallback = ui::group_null;
		} else if (name == fallback) {
			fallback = ui::group_null;
		}

		if (ui::group_null == name) {
			return cend();
		} else {
			auto const it = m_groups.find(name);
			return (cend() == it && ui::group_null != fallback)
				? m_groups.find(fallback)
				: it
			;
		}
	}
/// @}

/** @name Operations */ /// @{
	/**
		Clear the map.

		@post All iterators and group references are invalidated.
	*/
	void
	clear() noexcept {
		m_groups.clear();
	}

	/**
		Emplace group.

		@throws Error{ErrorCode::ui_invalid_group}
		If <code>name == ui::group_null</code>.

		@post All iterators are invalidated iff rehashing occurs.

		@returns Iterator for the emplaced group and @c true
		if @a name did not already exist; iterator for existing group
		and @c false otherwise.
		@param name Group name.
		@param args Constructor arguments.
	*/
	template<class... Args>
	std::pair<iterator, bool>
	emplace(
		ui::group_hash_type const name,
		Args&&... args
	) {
		if (ui::group_null == name) {
			// ... Doxygen ¬_¬
			/** @cond INTERNAL */
			#define BEARD_SCOPE_CLASS ui::PropertyMap
			#define BEARD_SCOPE_FUNC emplace
			/** @endcond */
			BEARD_THROW_FQN(
				ErrorCode::ui_invalid_group,
				"cannot emplace null group"
			);
			/** @cond INTERNAL */
			#undef BEARD_SCOPE_CLASS
			#undef BEARD_SCOPE_FUNC
			/** @endcond */
		}

		return m_groups.emplace(name, std::forward<Args>(args)...);
	}

	/**
		Erase group by name.

		@post All iterators and group references to the removed group
		are invalidated.

		@returns The number of groups removed (@c 1 or @c 0).
		@param name Group name to remove.
	*/
	map_type::size_type
	erase(
		ui::group_hash_type const name
	) noexcept {
		if (ui::group_null == name) {
			return 0u;
		} else {
			return m_groups.erase(name);
		}
	}

	/**
		Erase group by iterator.

		@note Unlike @c std::unordered_map, erasing the end iterator
		is a no-op instead of UB.

		@post All iterators and group references to the removed group
		are invalidated.

		@returns Iterator following the removed group.
		@param pos Iterator to remove.
	*/
	iterator
	erase(
		const_iterator const pos
	) noexcept {
		if (cend() == pos) {
			return end();
		} else {
			return m_groups.erase(pos);
		}
	}
/// @}

/** @name Values */ /// @{
	/**
		Get number value by name.

		@throws Error{ErrorCode::ui_property_not_found}
		If neither @a group nor @a fallback contain @a name.

		@returns The value of @a name in @a group or @a fallback.
		@param name Value name.
		@param group Iterator to group.
		@param fallback Iterator to fallback group.
	*/
	ui::property_number_type
	get_number(
		ui::property_hash_type const name,
		const_iterator group,
		const_iterator fallback
	) const;

	/**
		Get attr value by name.

		@throws Error{ErrorCode::ui_property_not_found}
		If neither @a group nor @a fallback contain @a name.

		@returns The value of @a name in @a group or @a fallback.
		@param name Value name.
		@param group Iterator to group.
		@param fallback Iterator to fallback group.
	*/
	ui::property_attr_type
	get_attr(
		ui::property_hash_type const name,
		const_iterator group,
		const_iterator fallback
	) const;

	/**
		Get boolean value by name.

		@throws Error{ErrorCode::ui_property_not_found}
		If neither @a group nor @a fallback contain @a name.

		@returns The value of @a name in @a group or @a fallback.
		@param name Value name.
		@param group Iterator to group.
		@param fallback Iterator to fallback group.
	*/
	ui::property_boolean_type
	get_boolean(
		ui::property_hash_type const name,
		const_iterator group,
		const_iterator fallback
	) const;

	/**
		Get string value by name.

		@throws Error{ErrorCode::ui_property_not_found}
		If neither @a group nor @a fallback contain @a name.

		@returns The value of @a name in @a group or @a fallback.
		@param name Value name.
		@param group Iterator to group.
		@param fallback Iterator to fallback group.
	*/
	ui::property_string_type const&
	get_string(
		ui::property_hash_type const name,
		const_iterator group,
		const_iterator fallback
	) const;
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#include <Beard/detail/gr_unconfigure.hpp>
