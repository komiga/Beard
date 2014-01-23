/**
@file ui/Container.hpp
@brief %Container widget.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_CONTAINER_HPP_
#define BEARD_UI_CONTAINER_HPP_

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>
#include <Beard/ui/ProtoSlotContainer.hpp>

#include <utility>
#include <memory>

namespace Beard {
namespace ui {

// Forward declarations
class Container;

/**
	@addtogroup ui
	@{
*/

/**
	%Widget container.
*/
class Container final
	: public ui::ProtoSlotContainer
{
private:
	using base_type = ui::ProtoSlotContainer;

private:
	enum class ctor_priv {};

	Container() noexcept = delete;
	Container(Container const&) = delete;
	Container& operator=(Container const&) = delete;

	ui::Widget::type_info const&
	get_type_info_impl() const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Container() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Container(
		ctor_priv const,
		ui::RootWPtr&& root,
		Axis const orientation,
		std::size_t const slot_count,
		ui::Widget::WPtr&& parent
	) noexcept
		: base_type(
			std::move(root),
			enum_combine(ui::Widget::Flags::visible),
			ui::group_null,
			{{0, 0}, true, Axis::both, Axis::both},
			std::move(parent),
			orientation,
			slot_count
		)
	{}
	/** @endcond */ // INTERNAL

	/**
		Construct container.

		@throws std::bad_alloc
		If allocation fails.

		@param root %Root.
		@param orientation Orientation.
		@param slot_count Number of slots to reserve.
		@param parent Parent.
	*/
	static aux::shared_ptr<ui::Container>
	make(
		ui::RootWPtr root,
		Axis const orientation,
		std::size_t const slot_count = 0u,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		return aux::make_shared<ui::Container>(
			ctor_priv{},
			std::move(root),
			orientation,
			slot_count,
			std::move(parent)
		);
	}

	/** Move constructor. */
	Container(Container&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Container& operator=(Container&&) = default;
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_CONTAINER_HPP_
