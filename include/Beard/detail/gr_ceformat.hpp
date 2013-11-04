/**
@file detail/gr_ceformat.hpp
@brief GR ceformat definitions.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_DETAIL_GR_CEFORMAT_HPP_
#define BEARD_DETAIL_GR_CEFORMAT_HPP_

#include <Beard/config.hpp>
#include <Beard/detail/gr_core.hpp>

#include <ceformat/Format.hpp>
#include <ceformat/print.hpp>

#include <duct/GR/ceformat.hpp>

#include <cerrno>
#include <cstring>

#define BEARD_DEF_FMT(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT(ident_, fmt_)

#define BEARD_DEF_FMT_CLASS(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT_CLASS(ident_, fmt_)

#define BEARD_DEF_FMT_FUNC(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT_FUNC(ident_, fmt_)

#define BEARD_DEF_FMT_FQN(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT_FQN(ident_, fmt_)

#define BEARD_THROW_FMT(ec_, cefmt_, ...) \
	DUCT_GR_THROW_CEFMT(ec_, cefmt_, __VA_ARGS__)

#define BEARD_DEF_FMT_FQN_CERR(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT_FQN(ident_, fmt_ "; errno: %d, reason: %s")

#ifndef BEARD_DETAIL_GR_CEFORMAT_NS_GUARD_
#define BEARD_DETAIL_GR_CEFORMAT_NS_GUARD_
namespace Beard {
namespace detail {
	BEARD_DEF_FMT_FQN_CERR(
		s_cerr_generic,
		"%s"
	);
}}
#endif // BEARD_DETAIL_GR_CEFORMAT_NS_GUARD_

#define BEARD_THROW_FMT_CERR(ec_, cefmt_, err_, ...) \
	DUCT_GR_THROW_CEFMT(ec_, cefmt_, __VA_ARGS__, err_, std::strerror(err_))

#define BEARD_THROW_CERR(ec_, err_, msg_)		\
	DUCT_GR_THROW_CEFMT(						\
		ec_, ::Beard::detail::s_cerr_generic,	\
		msg_, err_, std::strerror(err_)			\
	)

#endif // BEARD_DETAIL_GR_CEFORMAT_HPP_
