/**
@file ui/Button.hpp
@brief %Button widget.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
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
		Signal for the <em>pressed</em> event.

		Parameters:

		-# The actuated button.
	*/
	ui::Signal<void(aux::shared_ptr<ui::Button>)> signal_pressed;

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
	reflow_impl(
		Rect const& area,
		bool const cache
	) noexcept override;

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
			enum_combine(
				ui::Widget::Flags::trait_focusable,
				ui::Widget::Flags::visible
			),
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
		@param focus_index Focus index.
		@param parent Parent.
	*/
	static aux::shared_ptr<ui::Button>
	make(
		ui::RootWPtr root,
		String text,
		ui::group_hash_type const group = ui::group_button,
		ui::focus_index_type const focus_index = ui::focus_index_lazy,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		auto p = aux::make_shared<ui::Button>(
			ctor_priv{},
			group,
			std::move(root),
			std::move(parent),
			std::move(text)
		);
		p->set_focus_index(focus_index);
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
