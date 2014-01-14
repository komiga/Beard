/**
@file ui/Label.hpp
@brief %Label widget.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_LABEL_HPP_
#define BEARD_UI_LABEL_HPP_

#include <Beard/config.hpp>
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
class Label;

/**
	@addtogroup ui
	@{
*/

/**
	%Label widget.
*/
class Label final
	: public ui::Widget::Base
{
private:
	using base_type = ui::Widget::Base;

private:
	enum class ctor_priv {};

	Vec2 m_gc_pos{0, 0};
	String m_text;

	Label() noexcept = delete;
	Label(Label const&) = delete;
	Label& operator=(Label const&) = delete;

	ui::Widget::type_info const&
	get_type_info_impl() const noexcept override;

	void
	cache_geometry_impl() noexcept override;

	void
	reflow_impl(
		Rect const& area,
		bool const cache
	) noexcept override;

	void
	render_impl(
		tty::Terminal& terminal
	) noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Label() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Label(
		ctor_priv const,
		ui::RootWPtr&& root,
		String text,
		ui::Widget::WPtr&& parent
	) noexcept
		: base_type(
			std::move(root),
			enum_combine(
				ui::Widget::Flags::visible
			),
			{{1, 1}, false, Axis::none, Axis::none},
			std::move(parent)
		)
		, m_text(std::move(text))
	{}
	/** @endcond */ // INTERNAL

	/**
		Construct label.

		@throws std::bad_alloc
		If allocation fails.

		@param root %Root.
		@param text Text.
		@param parent Parent.
	*/
	static std::shared_ptr<ui::Label>
	make(
		ui::RootWPtr root,
		String text,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		auto p = std::make_shared<ui::Label>(
			ctor_priv{},
			std::move(root), std::move(text), std::move(parent)
		);
		return p;
	}

	/** Move constructor. */
	Label(Label&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Label& operator=(Label&&) = default;
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

#endif // BEARD_UI_LABEL_HPP_
