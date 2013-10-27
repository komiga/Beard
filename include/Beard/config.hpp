/**
@file config.hpp
@brief Configuration.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_CONFIG_HPP_
#define BEARD_CONFIG_HPP_

#include <cstddef>
#include <cstdint>

#include <duct/config.hpp>

namespace Beard {

/**
	@addtogroup config
	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI
	/**
		Allocator for auxiliary specializations.

		@note Defaults to @c std::allocator.
	*/
	#define BEARD_AUX_ALLOCATOR
#else // -
	#ifndef BEARD_AUX_ALLOCATOR
		#define BEARD_AUX_ALLOCATOR std::allocator
	#endif
#endif

/** @} */ // end of doc-group config

} // namespace Beard

#endif // BEARD_CONFIG_HPP_
