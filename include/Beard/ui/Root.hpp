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
	ui::Widget::WPtr m_focus;

	Root() = delete;
	Root(Root const&) = delete;
	Root& operator=(Root const&) = delete;

	bool
	handle_event_impl(
		ui::Event const& event
	) noexcept override;

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
		, m_focus()
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
	Root& operator=(Root&&) = default;
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
/// @}

/** @name Focus */ /// @{
private:
	ui::Widget::SPtr
	focus_dir(
		ui::Widget::SPtr from,
		ui::FocusDir const dir
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
		Clear focus.
	*/
	void
	clear_focus() {
		set_focus(nullptr);
	}

	/**
		Get focused widget.

		@note If @c !has_focus(), the return pointer will be null.
	*/
	ui::Widget::SPtr
	get_focus() noexcept {
		return m_focus.lock();
	}

	/**
		Check if there is a focused widget.
	*/
	bool
	has_focus() const noexcept {
		return !m_focus.expired();
	}

	/**
		Set focus to previous/next widget.
	*/
	void
	focus_dir(
		ui::FocusDir const dir
	) {
		set_focus(focus_dir(get_focus(), dir));
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
