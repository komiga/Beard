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

#endif // BEARD_DETAIL_GR_CEFORMAT_HPP_
