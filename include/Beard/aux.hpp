/**
@file aux.hpp
@brief Auxiliary stdlib specializations.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_AUX_HPP_
#define BEARD_AUX_HPP_

#include <Beard/config.hpp>

#include <string>
#include <vector>
#include <unordered_map>

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

/** @} */ // end of doc-group aux
/** @} */ // end of doc-group etc

} // namespace aux
} // namespace Beard

#endif // BEARD_AUX_HPP_
