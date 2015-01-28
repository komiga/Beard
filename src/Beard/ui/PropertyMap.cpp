/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/ui/PropertyMap.hpp>

#include <Beard/detail/gr_core.hpp>
#include <Beard/detail/gr_ceformat.hpp>

namespace Beard {
namespace ui {

// class PropertyMap implementation

#define BEARD_SCOPE_CLASS ui::PropertyMap

PropertyValue const*
PropertyMap::property(
	ui::property_hash_type const name,
	const_iterator group,
	const_iterator fallback
) const {
	if (ui::property_null == name) {
		return nullptr;
	}

	if (cend() == group) {
		group = fallback;
		fallback = cend();
	}

	if (cend() == group) {
		return nullptr;
	} else {
		auto const pv = group->second.property(name);
		return (!pv && cend() != fallback)
			? fallback->second.property(name)
			: pv
		;
	}
}

// values

namespace {
BEARD_DEF_FMT_FQN(
	s_err_property_not_found,
	"cannot find property: %-#08x"
);
} // anonymous namespace

#define BEARD_SCOPE_FUNC number
ui::property_number_type
PropertyMap::number(
	ui::property_hash_type const name,
	const_iterator group,
	const_iterator fallback
) const {
	auto const pv = property(name, group, fallback);
	if (pv && pv->is_type(ui::PropertyType::number)) {
		return pv->number();
	} else {
		BEARD_THROW_FMT(
			ErrorCode::ui_property_not_found,
			s_err_property_not_found,
			name
		);
	}
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC attr
ui::property_attr_type
PropertyMap::attr(
	ui::property_hash_type const name,
	const_iterator group,
	const_iterator fallback
) const {
	auto const pv = property(name, group, fallback);
	if (pv && pv->is_type(ui::PropertyType::attr)) {
		return pv->attr();
	} else {
		BEARD_THROW_FMT(
			ErrorCode::ui_property_not_found,
			s_err_property_not_found,
			name
		);
	}
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC boolean
ui::property_boolean_type
PropertyMap::boolean(
	ui::property_hash_type const name,
	const_iterator group,
	const_iterator fallback
) const {
	auto const pv = property(name, group, fallback);
	if (pv && pv->is_type(ui::PropertyType::boolean)) {
		return pv->boolean();
	} else {
		BEARD_THROW_FMT(
			ErrorCode::ui_property_not_found,
			s_err_property_not_found,
			name
		);
	}
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC string
ui::property_string_type const&
PropertyMap::string(
	ui::property_hash_type const name,
	const_iterator group,
	const_iterator fallback
) const {
	auto const pv = property(name, group, fallback);
	if (pv && pv->is_type(ui::PropertyType::string)) {
		return pv->string();
	} else {
		BEARD_THROW_FMT(
			ErrorCode::ui_property_not_found,
			s_err_property_not_found,
			name
		);
	}
}
#undef BEARD_SCOPE_FUNC

#undef BEARD_SCOPE_CLASS // ui::PropertyMap

} // namespace ui
} // namespace Beard
