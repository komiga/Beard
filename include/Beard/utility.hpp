/**
@file utility.hpp
@brief Utilities.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UTILITY_HPP_
#define BEARD_UTILITY_HPP_

#include <Beard/config.hpp>

#include <duct/utility.hpp>

namespace Beard {

// Forward declarations

/**
	@addtogroup etc
	@{
*/

using duct::make_const;
using duct::signed_cast;
using duct::unsigned_cast;
using duct::enum_cast;
using duct::enum_bitor;
using duct::enum_bitand;
using duct::enum_combine;
using duct::min_ce;
using duct::max_ce;

/** @} */ // end of doc-group etc

} // namespace Beard

#endif // BEARD_UTILITY_HPP_
