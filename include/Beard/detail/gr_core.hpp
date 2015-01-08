/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief GR core configuration.
*/

#ifndef BEARD_DETAIL_GR_CORE_HPP_
#define BEARD_DETAIL_GR_CORE_HPP_

#include <Beard/config.hpp>
#include <Beard/ErrorCode.hpp>
#include <Beard/String.hpp>
#include <Beard/Error.hpp>

#ifndef BEARD_DETAIL_GR_CORE_NS_GUARD_
#define BEARD_DETAIL_GR_CORE_NS_GUARD_

namespace Beard {
namespace detail {
namespace GR {
	using Error = ::Beard::Error;
}}}

#endif // BEARD_DETAIL_GR_CORE_NS_GUARD_

#define DUCT_GR_ROOT_NAMESPACE \
	::Beard::detail::GR

#define DUCT_GR_STR_LIT(s_) \
	BEARD_STR_LIT(s_)

#define DUCT_GR_MSG_IMPL_(s_, m_) \
	"{" s_ "} " m_

#define DUCT_GR_SCOPE_PREFIX \
	Beard::

#define DUCT_GR_SCOPE_CLASS \
	BEARD_SCOPE_CLASS

#define DUCT_GR_SCOPE_FUNC \
	BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FQN_STR \
	DUCT_GR_SCOPE_FQN_STR

#define BEARD_SCOPE_FQN_STR_LIT \
	DUCT_GR_SCOPE_FQN_STR_LIT

#include <duct/GR/common.hpp>
#include <duct/GR/throw.hpp>

#define BEARD_THROW(ec_, m_) \
	DUCT_GR_THROW(ec_, m_)

#define BEARD_THROW_CLASS(ec_, m_) \
	DUCT_GR_THROW_CLASS(ec_, m_)

#define BEARD_THROW_FUNC(ec_, m_) \
	DUCT_GR_THROW_FUNC(ec_, m_)

#define BEARD_THROW_FQN(ec_, m_) \
	DUCT_GR_THROW_FQN(ec_, m_)

#endif // BEARD_DETAIL_GR_CORE_HPP_
