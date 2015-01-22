/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Button widget.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/String.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Signal.hpp>
#include <Beard/ui/Widget/Base.hpp>

#include <utility>
#include <memory>

namespace Beard {
namespace ui {

// Forward declarations
class Button;

/**
	@addtogroup ui
	@{
*/

/**
	%Button widget.
*/
class Button final
	: public ui::Widget::Base
{
private:
	using base_type = ui::Widget::Base;

public:
	/**
		Shared pointer.
	*/
	using SPtr = aux::shared_ptr<ui::Button>;

	/**
		Signal for the <em>pressed</em> event.

		Parameters:

		-# The actuated button.
	*/
	ui::Signal<void(
		ui::Button::SPtr button
	)> signal_pressed;

private:
	enum class ctor_priv {};

	Vec2 m_gc_pos{0, 0};
	String m_text;

	Button() noexcept = delete;
	Button(Button const&) = delete;
	Button& operator=(Button const&) = delete;

// implementation
	void
	cache_geometry_impl() noexcept override;

	void
	reflow_impl() noexcept override;

	bool
	handle_event_impl(
		ui::Event const& event
	) noexcept override;

	void
	render_impl(
		ui::Widget::RenderData& rd
	) noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Button() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Button(
		ctor_priv const,
		ui::group_hash_type const group,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent,
		String&& text
	) noexcept
		: base_type(
			ui::Widget::Type::Button,
				ui::Widget::Flags::trait_focusable |
				ui::Widget::Flags::visible
			,
			group,
			{{1, 1}, false, Axis::none, Axis::none},
			std::move(root),
			std::move(parent)
		)
		, signal_pressed()
		, m_text(std::move(text))
	{}
	/** @endcond */ // INTERNAL

	/**
		Construct button.

		@throws std::bad_alloc
		If allocation fails.

		@param root %Root.
		@param text Text.
		@param group %Property group.
		@param parent Parent.
	*/
	static ui::Button::SPtr
	make(
		ui::RootWPtr root,
		String text,
		ui::group_hash_type const group = ui::group_button,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		auto p = aux::make_shared<ui::Button>(
			ctor_priv{},
			group,
			std::move(root),
			std::move(parent),
			std::move(text)
		);
		return p;
	}

	/** Move constructor. */
	Button(Button&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Button& operator=(Button&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set text.

		@param text Text.
	*/
	void
	set_text(
		String text
	);

	/**
		Get text.
	*/
	String const&
	get_text() const noexcept {
		return m_text;
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
