/**
@file
@brief Auxiliary stdlib specializations.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

namespace Beard {
namespace aux {

/**
	@addtogroup etc
	@{
*/
/**
	@addtogroup aux
	@{
*/

/**
	@c std::shared_ptr<T>.
*/
template<
	typename T
>
using shared_ptr = std::shared_ptr<T>;

/**
	@c std::weak_ptr<T>.
*/
template<
	typename T
>
using weak_ptr = std::weak_ptr<T>;

/** Alias for @c std::make_shared(). */
using std::make_shared;

/** Alias for @c std::owner_less<T>. */
using std::owner_less;

/** Alias for @c std::enable_shared_from_this<T>. */
using std::enable_shared_from_this;

/** Alias for @c std::function<R(Args...)>. */
using std::function;

/**
	@c std::basic_string<CharT, Traits>.
*/
template<
	typename CharT,
	class Traits = std::char_traits<CharT>
>
using basic_string
= std::basic_string<
	CharT, Traits,
	BEARD_AUX_ALLOCATOR<CharT>
>;

/**
	@c std::vector<T>.
*/
template<
	typename T
>
using vector
= std::vector<
	T,
	BEARD_AUX_ALLOCATOR<T>
>;

/**
	@c std::forward_list<T>.
*/
template<
	typename T
>
using forward_list
= std::forward_list<
	T,
	BEARD_AUX_ALLOCATOR<T>
>;

/**
	@c std::multimap<Key, T, Compare>.
*/
template<
	typename Key,
	typename T,
	class Compare = std::less<Key>
>
using multimap
= std::multimap<
	Key, T, Compare,
	BEARD_AUX_ALLOCATOR<std::pair<Key const, T> >
>;

/**
	@c std::unordered_map<Key, T, Hash, KeyEqual>.
*/
template<
	typename Key,
	typename T,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>
>
using unordered_map
= std::unordered_map<
	Key, T, Hash, KeyEqual,
	BEARD_AUX_ALLOCATOR<std::pair<Key const, T> >
>;

/**
	@c std::unordered_set<Key, Hash, KeyEqual>.
*/
template<
	typename Key,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>
>
using unordered_set
= std::unordered_set<
	Key, Hash, KeyEqual,
	BEARD_AUX_ALLOCATOR<Key>
>;

/**
	@c std::set<Key, Compare>.
*/
template<
	typename Key,
	class Compare = std::equal_to<Key>
>
using set
= std::set<
	Key, Compare,
	BEARD_AUX_ALLOCATOR<Key>
>;

/** @} */ // end of doc-group aux
/** @} */ // end of doc-group etc

} // namespace aux
} // namespace Beard
