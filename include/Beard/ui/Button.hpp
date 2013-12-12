/**
@file ui/Button.hpp
@brief %Button widget.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_BUTTON_HPP_
#define BEARD_UI_BUTTON_HPP_

#include <Beard/config.hpp>
#include <Beard/String.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Signal.hpp>
#include <Beard/ui/Widget.hpp>

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
	%Widget button.
*/
class Button final
	: public ui::Widget
{
public:
	/**
		Signal for the button "pressed" event.
	*/
	ui::Signal<void()> signal_pressed;

private:
	Vec2 m_gc_pos{0, 0};
	String m_text;

	Button() noexcept = delete;
	Button(Button const&) = delete;
	Button& operator=(Button const&) = delete;

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
	render_impl() noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Button() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Button(
		ui::Context& context,
		String text,
		ui::WidgetWPtr&& parent
	) noexcept
		: Widget(
			ui::WidgetType::Button,
			enum_combine(ui::Widget::Flags::visible),
			context,
			std::move(parent),
			{{1, 1}, false, Axis::none, Axis::none}
		)
		, signal_pressed()
		, m_text(std::move(text))
	{}
	/** @endcond */ // INTERNAL

	/**
		Construct button.

		@throws std::bad_alloc
		If allocation fails.

		@param context Context.
		@param parent Parent.
		@param text Text.
	*/
	static std::shared_ptr<ui::Button>
	make(
		ui::Context& context,
		String text,
		ui::WidgetWPtr parent = ui::WidgetWPtr()
	) {
		return std::make_shared<ui::Button>(
			context, std::move(text), std::move(parent)
		);
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
	) noexcept {
		// TODO: Trigger/queue re-render
		m_text.assign(std::move(text));
	}

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

#endif // BEARD_UI_BUTTON_HPP_
