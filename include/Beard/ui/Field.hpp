/**
@file ui/Field.hpp
@brief %Field widget.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
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
		Input filter function type.

		Returns @c true if the code point is accepted.

		Parameters:

		-# Input code point.
	*/
	using filter_type = aux::function<bool(char32 const)>;

	/**
		Signal for the <em>control changed</em> event.

		Parameters:

		-# The affected field.
		-# Whether the field has gained or lost input control.
	*/
	ui::Signal<void(
		aux::shared_ptr<ui::Field> /*field*/,
		bool /*have_control*/
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
	ui::Widget::type_info const&
	get_type_info_impl() const noexcept override;

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
		ui::RootWPtr&& root,
		String&& text,
		filter_type&& filter,
		ui::group_hash_type const group,
		ui::Widget::WPtr&& parent
	) noexcept
		: base_type(
			std::move(root),
			enum_combine(
				ui::Widget::Flags::visible
			),
			group,
			{{2, 1}, false, Axis::none, Axis::none},
			std::move(parent)
		)
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
		@param focus_index Focus index.
		@param parent Parent.
	*/
	static aux::shared_ptr<ui::Field>
	make(
		ui::RootWPtr root,
		String text,
		filter_type filter = nullptr,
		ui::group_hash_type const group = ui::group_field,
		ui::focus_index_type const focus_index = ui::focus_index_lazy,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		auto p = aux::make_shared<ui::Field>(
			ctor_priv{},
			std::move(root),
			std::move(text),
			std::move(filter),
			group,
			std::move(parent)
		);
		p->set_focus_index(focus_index);
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

