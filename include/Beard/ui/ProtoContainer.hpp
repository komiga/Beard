/**
@file ui/ProtoContainer.hpp
@brief Prototype container widget.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>

#include <utility>

namespace Beard {
namespace ui {

// Forward declarations
class ProtoContainer;

/**
	@addtogroup ui
	@{
*/

/**
	Prototype container widget.
*/
class ProtoContainer
	: public ui::Widget::Base
{
private:
	using base_type = ui::Widget::Base;

private:
	ProtoContainer() noexcept = delete;
	ProtoContainer(ProtoContainer const&) = delete;
	ProtoContainer& operator=(ProtoContainer const&) = delete;

protected:
/** @name Implementation */ /// @{
	/**
		size() implementation.
	*/
	virtual std::size_t
	size_impl() const noexcept = 0;

	/**
		clear() implementation.
	*/
	virtual void
	clear_impl() = 0;

	/**
		set_widget() implementation.
	*/
	virtual void
	set_widget_impl(
		std::size_t const idx,
		ui::Widget::SPtr widget
	) = 0;

	/**
		get_widget() implementation.
	*/
	virtual ui::Widget::SPtr
	get_widget_impl(
		std::size_t const idx
	) const = 0;

	/**
		push_back() implementation.
	*/
	virtual void
	push_back_impl(
		ui::Widget::SPtr widget
	) = 0;
/// @}

protected:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	virtual
	~ProtoContainer() noexcept override = 0;

	/**
		Constructor.

		@param root %Root.
		@param flags Flags.
		@param group %Property group.
		@param geometry Geometry.
		@param parent Parent.
	*/
	ProtoContainer(
		ui::RootWPtr&& root,
		ui::Widget::Flags const flags,
		ui::group_hash_type const group,
		ui::Geom&& geometry,
		ui::Widget::WPtr&& parent
	) noexcept
		: base_type(
			std::move(root),
			flags,
			group,
			std::move(geometry),
			std::move(parent)
		)
	{}

	/** Move constructor. */
	ProtoContainer(ProtoContainer&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	ProtoContainer& operator=(ProtoContainer&&) = default;
/// @}

public:
/** @name Collection */ /// @{
	/**
		Get the number of widgets in the container.
	*/
	std::size_t
	size() const noexcept {
		return size_impl();
	}

	/**
		Remove all widgets.
	*/
	void
	clear() {
		clear_impl();
	}

	/**
		Set widget by index.

		@throws std::out_of_range
		If @a idx is out of bounds.

		@param idx Index.
		@param widget %Widget.
	*/
	void
	set_widget(
		std::size_t const idx,
		ui::Widget::SPtr widget
	) {
		set_widget_impl(idx, std::move(widget));
	}

	/**
		Get widget by index.

		@throws std::out_of_range
		If @a idx is out of bounds.

		@param idx %Widget index.
	*/
	ui::Widget::SPtr
	get_widget(
		std::size_t const idx
	) const {
		return get_widget_impl(idx);
	}

	/**
		Add a widget to the end of the container.

		@param widget %Widget to add.
	*/
	void
	push_back(
		ui::Widget::SPtr widget
	) {
		push_back_impl(std::move(widget));
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

