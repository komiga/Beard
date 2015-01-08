/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Spacer widget.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/String.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>

#include <utility>
#include <memory>

namespace Beard {
namespace ui {

// Forward declarations
class Spacer;

/**
	@addtogroup ui
	@{
*/

/**
	%Spacer widget.
*/
class Spacer final
	: public ui::Widget::Base
{
private:
	using base_type = ui::Widget::Base;

public:
	/**
		Shared pointer.
	*/
	using SPtr = aux::shared_ptr<ui::Spacer>;

private:
	enum class ctor_priv {};

	Spacer() noexcept = delete;
	Spacer(Spacer const&) = delete;
	Spacer& operator=(Spacer const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Spacer() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Spacer(
		ctor_priv const,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent,
		Axis const expand_and_fill = Axis::both
	) noexcept
		: base_type(
			ui::Widget::Type::Spacer,
			enum_combine(
				ui::Widget::Flags::visible
			),
			ui::group_null,
			{{1, 1}, false, expand_and_fill, expand_and_fill},
			std::move(root),
			std::move(parent)
		)
	{}
	/** @endcond */ // INTERNAL

	/**
		Construct spacer.

		@throws std::bad_alloc
		If allocation fails.

		@param root %Root.
		@param expand_and_fill Expand and fill axes.
		@param parent Parent.
	*/
	static ui::Spacer::SPtr
	make(
		ui::RootWPtr root,
		Axis const expand_and_fill = Axis::both,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		auto p = aux::make_shared<ui::Spacer>(
			ctor_priv{},
			std::move(root),
			std::move(parent),
			expand_and_fill
		);
		return p;
	}

	/** Move constructor. */
	Spacer(Spacer&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Spacer& operator=(Spacer&&) = default;
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
