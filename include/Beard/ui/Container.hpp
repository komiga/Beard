/**
@file ui/Container.hpp
@brief %Container widget.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

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

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Container() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Container(
		ctor_priv const,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent,
		Axis const orientation
	) noexcept
		: base_type(
			ui::Widget::Type::Container,
			enum_combine(
				ui::Widget::Flags::visible
			),
			ui::group_null,
			{{0, 0}, true, Axis::both, Axis::both},
			std::move(root),
			std::move(parent),
			orientation
		)
	{}
	/** @endcond */ // INTERNAL

	/**
		Construct container.

		@throws std::bad_alloc
		If allocation fails.

		@param root %Root.
		@param orientation Orientation.
		@param parent Parent.
	*/
	static aux::shared_ptr<ui::Container>
	make(
		ui::RootWPtr root,
		Axis const orientation,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		return aux::make_shared<ui::Container>(
			ctor_priv{},
			std::move(root),
			std::move(parent),
			orientation
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
