
#include <Beard/ui/PropertyMap.hpp>

#include <Beard/detail/gr_core.hpp>
#include <Beard/detail/gr_ceformat.hpp>

namespace Beard {
namespace ui {

// class PropertyMap implementation

#define BEARD_SCOPE_CLASS ui::PropertyMap

PropertyValue const*
PropertyMap::get_property(
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
		auto const pv = group->second.get_property(name);
		return (!pv && cend() != fallback)
			? fallback->second.get_property(name)
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

#define BEARD_SCOPE_FUNC get_number
ui::property_number_type
PropertyMap::get_number(
	ui::property_hash_type const name,
	const_iterator group,
	const_iterator fallback
) const {
	auto const pv = get_property(name, group, fallback);
	if (pv && pv->is_type(ui::PropertyType::number)) {
		return pv->get_number();
	} else {
		BEARD_THROW_FMT(
			ErrorCode::ui_property_not_found,
			s_err_property_not_found,
			name
		);
	}
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC get_attr
ui::property_attr_type
PropertyMap::get_attr(
	ui::property_hash_type const name,
	const_iterator group,
	const_iterator fallback
) const {
	auto const pv = get_property(name, group, fallback);
	if (pv && pv->is_type(ui::PropertyType::attr)) {
		return pv->get_attr();
	} else {
		BEARD_THROW_FMT(
			ErrorCode::ui_property_not_found,
			s_err_property_not_found,
			name
		);
	}
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC get_boolean
ui::property_boolean_type
PropertyMap::get_boolean(
	ui::property_hash_type const name,
	const_iterator group,
	const_iterator fallback
) const {
	auto const pv = get_property(name, group, fallback);
	if (pv && pv->is_type(ui::PropertyType::boolean)) {
		return pv->get_boolean();
	} else {
		BEARD_THROW_FMT(
			ErrorCode::ui_property_not_found,
			s_err_property_not_found,
			name
		);
	}
}
#undef BEARD_SCOPE_FUNC

#define BEARD_SCOPE_FUNC get_string
ui::property_string_type const&
PropertyMap::get_string(
	ui::property_hash_type const name,
	const_iterator group,
	const_iterator fallback
) const {
	auto const pv = get_property(name, group, fallback);
	if (pv && pv->is_type(ui::PropertyType::string)) {
		return pv->get_string();
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
