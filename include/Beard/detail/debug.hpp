/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Debug macros.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/detail/gr_core.hpp>

#include <duct/debug.hpp>

#include <cstring>
#include <cerrno>

#define BEARD_DEBUG_CERR_FQN(err_, m_)				\
	DUCT_DEBUGF(									\
		BEARD_SCOPE_FQN_STR ": " m_					\
		"; errno: %d, reason: %s",					\
		err_,										\
		std::strerror(err_)							\
	)
//

#define BEARD_DEBUG_MSG_FQN(m_)						\
	DUCT_DEBUG(										\
		BEARD_SCOPE_FQN_STR ": " m_					\
	)
//

#define BEARD_DEBUG_MSG_FQN_F(m_, ...)				\
	DUCT_DEBUGF(									\
		BEARD_SCOPE_FQN_STR ": " m_,				\
		__VA_ARGS__									\
	)
//
