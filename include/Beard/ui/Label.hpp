/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Label widget.
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

public:
	/**
		Shared pointer.
	*/
	using SPtr = aux::shared_ptr<ui::Label>;

private:
	enum class ctor_priv {};

	Vec2 m_gc_pos{0, 0};
	String m_text;

	Label() noexcept = delete;
	Label(Label const&) = delete;
	Label& operator=(Label const&) = delete;

	void
	cache_geometry_impl() noexcept override;

	void
	reflow_impl() noexcept override;

	void
	render_impl(
		ui::Widget::RenderData& rd
	) noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Label() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Label(
		ctor_priv const,
		ui::group_hash_type const group,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent,
		String&& text
	) noexcept
		: base_type(
			ui::Widget::Type::Label,
			ui::Widget::Flags::visible,
			group,
			{{1, 1}, false, Axis::none, Axis::none},
			std::move(root),
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
		@param group %Property group.
		@param parent Parent.
	*/
	static ui::Label::SPtr
	make(
		ui::RootWPtr root,
		String text,
		ui::group_hash_type const group = ui::group_label,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		auto p = aux::make_shared<ui::Label>(
			ctor_priv{},
			group,
			std::move(root),
			std::move(parent),
			std::move(text)
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
