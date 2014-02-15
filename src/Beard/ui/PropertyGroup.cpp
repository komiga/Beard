
#include <Beard/tty/Defs.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/PropertyGroup.hpp>

namespace Beard {
namespace ui {

// class PropertyGroup implementation

ui::PropertyGroup
s_default_group{{
// primary
	{ui::property_primary_fg_inactive, tty::attr_type{tty::Color::term_default}},
	{ui::property_primary_bg_inactive, tty::attr_type{tty::Color::term_default}},
	{ui::property_primary_fg_active, tty::attr_type{tty::Color::term_default}},
	{ui::property_primary_bg_active, tty::attr_type{tty::Color::term_default | tty::Attr::inverted}},

// content
	{ui::property_content_fg_inactive, tty::attr_type{tty::Color::term_default}},
	{ui::property_content_bg_inactive, tty::attr_type{tty::Color::term_default}},
	{ui::property_content_fg_active, tty::attr_type{tty::Color::term_default}},
	{ui::property_content_bg_active, tty::attr_type{tty::Color::term_default}},
	{ui::property_content_fg_selected, tty::attr_type{tty::Color::term_default | tty::Attr::bold}},
	{ui::property_content_bg_selected, tty::attr_type{tty::Color::magenta}},

// frame
	{ui::property_frame_enabled, false},
#ifdef NDEBUG
	{ui::property_frame_debug_enabled, false},
#else
	{ui::property_frame_debug_enabled, true},
#endif
	{ui::property_frame_fg_inactive, tty::attr_type{tty::Color::blue}},
	{ui::property_frame_bg_inactive, tty::attr_type{tty::Color::term_default}},
	{ui::property_frame_fg_active, tty::attr_type{tty::Color::blue}},
	{ui::property_frame_bg_active, tty::attr_type{tty::Color::term_default | tty::Attr::inverted}},

// field
	{ui::property_field_content_underline, true},
}};

} // namespace ui
} // namespace Beard
