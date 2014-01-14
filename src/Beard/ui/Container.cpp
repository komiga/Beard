
#include <Beard/utility.hpp>
#include <Beard/ui/packing.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Container.hpp>

namespace Beard {
namespace ui {

// class Container implementation

namespace {
static ui::Widget::type_info const
s_type_info{
	ui::Widget::Type::Container,
	enum_combine(
		ui::Widget::TypeFlags::container
	)
};
} // anonymous namespace

Container::~Container() noexcept = default;

ui::Widget::type_info const&
Container::get_type_info_impl() const noexcept {
	return s_type_info;
}

} // namespace ui
} // namespace Beard
