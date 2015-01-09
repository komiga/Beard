/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Field widget.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/String.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>
#include <Beard/txt/Tree.hpp>
#include <Beard/txt/Cursor.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Signal.hpp>
#include <Beard/ui/Widget/Base.hpp>

#include <utility>
#include <memory>

namespace Beard {
namespace ui {

// Forward declarations
class Field;

/**
	@addtogroup ui
	@{
*/

/**
	%Field widget.
*/
class Field final
	: public ui::Widget::Base
{
private:
	using base_type = ui::Widget::Base;

public:
	/**
		Shared pointer.
	*/
	using SPtr = aux::shared_ptr<ui::Field>;

	/**
		Input filter function type.

		Returns @c true if the code point is accepted.

		Parameters:

		-# Input code point.
	*/
	using filter_type = aux::function<bool(char32 const)>;

	/**
		Signal for the <em>user modified</em> event.

		@note The field text does not revert to its previous value if
		<code>accept == true</code>.

		Parameters:

		-# The affected field.
		-# Whether the user entered a new value.
	*/
	ui::Signal<void(
		ui::Field::SPtr field,
		bool accept
	)> signal_user_modified;

	/**
		Signal for the <em>control changed</em> event.

		@note This occurs after @c signal_user_modified.

		Parameters:

		-# The affected field.
		-# Whether the field has gained or lost input control.
	*/
	ui::Signal<void(
		ui::Field::SPtr field,
		bool have_control
	)> signal_control_changed;

private:
	enum class ctor_priv {};

	txt::Tree m_tree;
	filter_type m_filter;
	txt::Cursor m_cursor;
	txt::Cursor m_view;

	Field() noexcept = delete;
	Field(Field const&) = delete;
	Field& operator=(Field const&) = delete;

	void
	update_view() noexcept;

// implementation
	void
	set_input_control_impl(
		bool const enabled
	) noexcept override;

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
	~Field() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Field(
		ctor_priv const,
		ui::group_hash_type const group,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent,
		String&& text,
		filter_type&& filter
	) noexcept
		: base_type(
			ui::Widget::Type::Field,
			enum_combine(
				ui::Widget::Flags::trait_focusable,
				ui::Widget::Flags::visible
			),
			group,
			{{2, 1}, false, Axis::none, Axis::none},
			std::move(root),
			std::move(parent)
		)
		, signal_user_modified()
		, signal_control_changed()
		, m_tree(std::move(text))
		, m_filter(std::move(filter))
		, m_cursor(m_tree)
		, m_view(m_tree)
	{
		DUCT_ASSERTE(1u == m_tree.lines());
		if (1u == m_tree.lines()) {
			m_cursor.col_extent(txt::Extent::tail);
		}
	}
	/** @endcond */ // INTERNAL

	/**
		Construct button.

		@throws std::bad_alloc
		If allocation fails.

		@param root %Root.
		@param text Text.
		@param filter Input filter.
		@param group %Property group.
		@param parent Parent.
	*/
	static ui::Field::SPtr
	make(
		ui::RootWPtr root,
		String text,
		filter_type filter = nullptr,
		ui::group_hash_type const group = ui::group_field,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		auto p = aux::make_shared<ui::Field>(
			ctor_priv{},
			group,
			std::move(root),
			std::move(parent),
			std::move(text),
			std::move(filter)
		);
		return p;
	}

	/** Move constructor. */
	Field(Field&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Field& operator=(Field&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set text.

		@param text Text.
	*/
	void
	set_text(
		String const& text
	);

	/**
		Get text.

		@note This is an uncached operation.
		See txt::Tree::to_string().
	*/
	String
	get_text() const noexcept {
		return m_tree.to_string();
	}

	/**
		Get text tree.
	*/
	txt::Tree const&
	get_tree() const noexcept {
		return m_tree;
	}

	/**
		Set input filter.

		@note The input filter is called when a valid input
		code point is received on a %Field with input control. The
		code point is only accepted if the filter returns @c true.

		@param filter New input filter.
	*/
	void
	set_filter(
		filter_type filter
	) noexcept {
		m_filter = std::move(filter);
	}

	/**
		Get input filter.
	*/
	filter_type const&
	get_filter() const noexcept {
		return m_filter;
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
