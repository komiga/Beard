/**
@file ui/Root.hpp
@brief UI root class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/FocusMap.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/ProtoSlotContainer.hpp>

namespace Beard {
namespace ui {

// Forward declarations
class Root;

/**
	@addtogroup ui
	@{
*/

/**
	%Root class.
*/
class Root final
	: public ui::ProtoSlotContainer
{
private:
	using base_type = ui::ProtoSlotContainer;

public:
	friend class Widget::Base;

private:
	enum class ctor_priv {};

	std::reference_wrapper<ui::Context> m_context;
	ui::FocusMap m_focus_map;

	struct {
		ui::FocusMap::const_iterator iter;
		ui::Widget::WPtr widget;
	} m_focus;

	Root() = delete;
	Root(Root const&) = delete;
	Root& operator=(Root const&) = delete;

	bool
	handle_event_impl(
		ui::Event const& event
	) noexcept override;

	void
	notify_focus_index_changing(
		ui::Widget::SPtr const& widget,
		ui::focus_index_type const old_index,
		ui::focus_index_type const new_index
	);

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Root() noexcept override;

	/** @cond INTERNAL */
	/* Required for visibility in make_shared; do not use directly. */
	Root(
		ctor_priv const,
		ui::Context& context,
		Axis const orientation,
		std::size_t const slot_count
	)
		: base_type(
			ui::Widget::Type::Root,
			enum_combine(
				ui::Widget::Flags::visible
			),
			ui::group_null,
			{{0, 0}, true, Axis::both, Axis::both},
			{},
			{},
			orientation,
			slot_count
		)
		, m_context(context)
		, m_focus_map()
		, m_focus({m_focus_map.cend(), ui::Widget::WPtr()})
	{}
	/** @endcond */

	/**
		Construct root.

		@throws std::bad_alloc
		If allocation fails.

		@param context Context.
		@param orientation Orientation.
		@param slot_count Number of slots to reserve.
	*/
	static ui::RootSPtr
	make(
		ui::Context& context,
		Axis const orientation,
		std::size_t const slot_count = 0u
	) {
		auto p = aux::make_shared<ui::Root>(
			ctor_priv{},
			context,
			orientation,
			slot_count
		);
		p->m_root = ui::RootWPtr(p);
		return std::move(p);
	}

	/** Move constructor. */
	Root(Root&&) noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Root& operator=(Root&&) noexcept = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get context (mutable).
	*/
	ui::Context&
	get_context() noexcept {
		return m_context;
	}

	/**
		Get context.
	*/
	ui::Context const&
	get_context() const noexcept {
		return m_context;
	}

	/**
		Get terminal (mutable).
	*/
	tty::Terminal&
	get_terminal() noexcept {
		return get_context().get_terminal();
	}

	/**
		Get terminal.
	*/
	tty::Terminal const&
	get_terminal() const noexcept {
		return get_context().get_terminal();
	}

	/**
		Get focus map (mutable).
	*/
	ui::FocusMap&
	get_focus_map() noexcept {
		return m_focus_map;
	}

	/**
		Get focus map.
	*/
	ui::FocusMap const&
	get_focus_map() const noexcept {
		return m_focus_map;
	}
/// @}

/** @name Focus */ /// @{
private:
	void
	set_focus(
		ui::FocusMap::const_iterator const iter,
		ui::Widget::SPtr const& widget
	);

public:
	/**
		Set focused widget.

		@param widget %Widget.
	*/
	void
	set_focus(
		ui::Widget::SPtr const& widget
	);

	/**
		Set bound focus index.

		If this is not equal to @c ui::focus_index_none, any widgets
		that do not have @a bound_index as their focus index will not
		be focusable by focus_dir().
	*/
	void
	set_bound_index(
		ui::focus_index_type const bound_index
	);

	/**
		Clear focus.
	*/
	void
	clear_focus() {
		set_focus(m_focus_map.cend(), nullptr);
	}

	/**
		Get focused widget.

		@note If @c !has_focus(), the return pointer will be null.
	*/
	ui::Widget::SPtr
	get_focus() noexcept {
		return m_focus.widget.lock();
	}

	/**
		Check if there is a focused widget.
	*/
	bool
	has_focus() const noexcept {
		return !m_focus.widget.expired();
	}

	/**
		Set focus to previous/next widget.
	*/
	void
	focus_dir(
		ui::FocusDir const dir
	);
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
