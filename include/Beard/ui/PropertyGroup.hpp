/**
@file ui/PropertyGroup.hpp
@brief %Property group class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_PROPERTYGROUP_HPP_
#define BEARD_UI_PROPERTYGROUP_HPP_

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/PropertyValue.hpp>

#include <initializer_list>
#include <utility>

namespace Beard {
namespace ui {

// Forward declarations
class PropertyGroup;
class PropertyMap; // external

/**
	@addtogroup ui
	@{
*/

/**
	%Property value group.
*/
class PropertyGroup final {
	friend class ui::PropertyMap;

public:
	/** %Property value map type. */
	using map_type = aux::unordered_map<
		ui::property_hash_type,
		ui::PropertyValue
	>;

	/** Key-value pair type. */
	using pair_type = map_type::value_type;

	/** Map iterator type. */
	using iterator = map_type::iterator;
	/** Const map iterator type. */
	using const_iterator = map_type::const_iterator;

	/**
		Get the default property group.
	*/
	static ui::PropertyGroup&
	get_default() noexcept {
		extern ui::PropertyGroup s_default_group;
		return s_default_group;
	}

private:
	// FIXME: Defect in libc++ (HEAD) / Clang ~3.5: adding a {} here
	// blows up claiming that 'value' is not a member of
	// std::is_void<T>. Side note: it doesn't asplode with an NSDMI
	// on PropertyMap::m_groups.
	map_type m_values;

	PropertyValue*
	get_property(
		ui::property_hash_type const name
	) {
		if (ui::property_null == name) {
			return nullptr;
		} else {
			auto const it = find(name);
			return (end() == it)
				? nullptr
				: &it->second
			;
		}
	}

	PropertyValue const*
	get_property(
		ui::property_hash_type const name
	) const {
		if (ui::property_null == name) {
			return nullptr;
		} else {
			auto const it = find(name);
			return (cend() == it)
				? nullptr
				: &it->second
			;
		}
	}

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~PropertyGroup() = default;

	/** Default constructor. */
	PropertyGroup() = default;

	/**
		Constructor with initializer list.

		@param ilist Initializer list.
	*/
	PropertyGroup(
		std::initializer_list<pair_type> ilist
	)
		: m_values(std::move(ilist))
	{}

	/** Copy constructor. */
	PropertyGroup(PropertyGroup const&) = default;
	/** Move constructor. */
	PropertyGroup(PropertyGroup&&) noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	PropertyGroup& operator=(PropertyGroup const&) = default;
	/** Move assignment operator. */
	PropertyGroup& operator=(PropertyGroup&&) noexcept = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get number of values.
	*/
	std::size_t
	size() const noexcept {
		return m_values.size();
	}

	/**
		Check if the group is empty.
	*/
	bool
	empty() const noexcept {
		return m_values.empty();
	}

	/**
		Get iterator to beginning.
	*/
	iterator
	begin() noexcept {
		return m_values.begin();
	}
	/**
		Get iterator to beginning.
	*/
	const_iterator
	begin() const noexcept {
		return m_values.begin();
	}
	/**
		Get iterator to beginning.
	*/
	const_iterator
	cbegin() const noexcept {
		return m_values.cbegin();
	}

	/**
		Get iterator to end.
	*/
	iterator
	end() noexcept {
		return m_values.end();
	}
	/**
		Get iterator to end.
	*/
	const_iterator
	end() const noexcept {
		return m_values.end();
	}
	/**
		Get iterator to end.
	*/
	const_iterator
	cend() const noexcept {
		return m_values.cend();
	}
/// @}

/** @name Lookup */ /// @{
	/**
		Check if map contains a value.

		@param name Value name.
	*/
	bool
	contains(
		ui::property_hash_type const name
	) const noexcept {
		return (ui::property_null == name)
			? false
			: m_values.count(name)
		;
	}

	/**
		Find value by name.

		@returns Iterator to @a name or @a fallback if either exist;
		end() otherwise.
		@param name Value name.
	*/
	iterator
	find(
		ui::property_hash_type const name
	) noexcept {
		return (ui::group_null == name)
			? end()
			: m_values.find(name)
		;
	}

	/**
		Find value by name.

		@returns Iterator to @a name or @a fallback if either exist;
		cend() otherwise.
		@param name Value name.
	*/
	const_iterator
	find(
		ui::property_hash_type const name
	) const noexcept {
		return (ui::group_null == name)
			? cend()
			: m_values.find(name)
		;
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_PROPERTYGROUP_HPP_
