/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Prototype grid widget.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/geometry.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>

#include <utility>

namespace Beard {
namespace ui {

// Forward declarations
struct GridView;
struct GridRenderData;
class ProtoGrid;

/**
	@addtogroup ui
	@{
*/

/**
	Grid view.
*/
struct GridView final {
	/** Entire frame of the view. */
	Rect frame{{0, 0}, {0, 0}};
	/** Header frame. */
	Rect header_frame{{0, 0}, {0, 0}};
	/** Content frame (not including header). */
	Rect content_frame{{0, 0}, {0, 0}};
	/** Number of rows that fit in the content frame. */
	ui::index_type fit_count{0};
	/** Number of columns in the column range. */
	ui::index_type col_count{0};
	/** Number of rows in the row range. */
	ui::index_type row_count{0};
	/** Column range. */
	Vec2 col_range{0, 0};
	/** Row range (bound to fit_count). */
	Vec2 row_range{0, 0};
};

/**
	Grid render data.
*/
struct GridRenderData final {
	/** %Base render data. */
	ui::Widget::RenderData& rd;

	/**
		Cached properties.
		@{
	*/
	tty::attr_type
		primary_fg{tty::Color::term_default},
		primary_bg{tty::Color::term_default},
		content_fg{tty::Color::term_default},
		content_bg{tty::Color::term_default},
		selected_fg{tty::Color::term_default},
		selected_bg{tty::Color::term_default}
	;
	/** @} */

	/**
		Constructor with render data and cache.

		@returns Return value.
		@param rd %Base render data.
		@param primary_active Whether to fetch active or inactive
		primary attributes.
		@param content_active Whether to fetch active or inactive
		content attributes.
	*/
	GridRenderData(
		ui::Widget::RenderData& rd,
		bool const primary_active,
		bool const content_active
	) noexcept
		: rd(rd)
		, primary_fg(rd.get_attr(
			primary_active
			? ui::property_primary_fg_active
			: ui::property_primary_fg_inactive
		))
		, primary_bg(rd.get_attr(
			primary_active
			? ui::property_primary_bg_active
			: ui::property_primary_bg_inactive
		))
		, content_fg(rd.get_attr(
			content_active
			? ui::property_content_fg_active
			: ui::property_content_fg_inactive
		))
		, content_bg(rd.get_attr(
			content_active
			? ui::property_content_bg_active
			: ui::property_content_bg_inactive
		))
		, selected_fg(rd.get_attr(ui::property_content_fg_selected))
		, selected_bg(rd.get_attr(ui::property_content_bg_selected))
	{}
};

/**
	Prototype grid widget.
*/
class ProtoGrid
	: public ui::Widget::Base
{
private:
	using base_type = ui::Widget::Base;

public:
	/**
		Shared pointer.
	*/
	using SPtr = aux::shared_ptr<ui::ProtoGrid>;

	/**
		Content actions.
	*/
	enum class ContentAction : unsigned {
		select = 0u,
		unselect,
		select_toggle,
		insert_before,
		insert_after,
		erase,
		erase_selected
	};

protected:
	/** Vector of ranges. */
	using range_vector_type = aux::vector<Vec2>;

private:
	bool m_header_enabled{true};
	bool m_header_enabled_next{true};
	ui::index_type m_col_count{0};
	ui::index_type m_row_count{0};

	struct {
		Vec2 header{0, 0};
		range_vector_type rows{};
	} m_dirty{};

	ui::GridView m_view{};

	ProtoGrid() noexcept = delete;
	ProtoGrid(ProtoGrid const&) = delete;
	ProtoGrid& operator=(ProtoGrid const&) = delete;

protected:
/** @name Internals */ /// @{
	/**
		Set number of columns.

		@param col_count New count.
	*/
	void
	set_col_count(
		ui::index_type const col_count
	) noexcept {
		m_col_count = max_ce(0, col_count);
	}

	/**
		Set number of rows.

		@param row_count New count.
	*/
	void
	set_row_count(
		ui::index_type const row_count
	) noexcept {
		m_row_count = max_ce(0, row_count);
	}

	/**
		Get view (mutable).
	*/
	GridView&
	get_view() noexcept {
		return m_view;
	}

	/**
		Enable or disable header.

		@note is_header_enabled() will not report the new value until
		reflow_view() is called.

		@param header_enabled Whether to enable or disable the header.
	*/
	void
	set_header_enabled(
		bool const header_enabled
	) noexcept {
		if (header_enabled != m_header_enabled) {
			m_header_enabled_next = header_enabled;
		}
	}

	/**
		Update the view range.

		@note If @a retain_intersection is @c false, the cell render
		queue will not be modified.

		@param row_begin Start of row range.
		@param row_end End of row range (non-inclusive).
		@param col_begin Start of column range.
		@param col_end End of column range (non-inclusive).
		@param retain_intersection Whether to retain the queued
		intersection with the previous range.
	*/
	void
	update_view(
		ui::index_type row_begin,
		ui::index_type row_end,
		ui::index_type col_begin,
		ui::index_type col_end,
		bool const retain_intersection
	) noexcept;

	/**
		Perform internal content action.

		This will queue cell renders and shift the view if necessary.

		This only responds to the @c insert_before, @c insert_after,
		and @c erase actions. @c erase_selected should be decomposed
		to @c erase actions by content_action(). The row count will
		be updated.

		@note The first three parameters are the same as for
		content_action(). 

		@param action Content action.
		@param row_begin Start of row range.
		@param count Number of rows.
	*/
	void
	content_action_internal(
		ui::ProtoGrid::ContentAction const action,
		ui::index_type row_begin,
		ui::index_type count
	) noexcept;

	/**
		Queue header render.

		@note If @a col_begin is @c -1, the column range <em>at point
		of render</em> will be rendered.

		@param col_begin Start of column range.
		@param col_end End of column range (non-inclusive).
	*/
	void
	queue_header_render(
		ui::index_type col_begin = -1,
		ui::index_type col_end = -1
	) noexcept;

	/**
		Queue cell render.

		@note If @a col_begin is @c -1, the column range <em>at point
		of render</em> will be rendered.

		@param row_begin Start of row range.
		@param row_end End of row range (non-inclusive).
		@param col_begin Start of column range.
		@param col_end End of column range (non-inclusive).
	*/
	void
	queue_cell_render(
		ui::index_type row_begin,
		ui::index_type row_end,
		ui::index_type col_begin = -1,
		ui::index_type col_end = -1
	) noexcept;

	/**
		Reflow view.

		@note This queues a cell render for the entire view range.

		@param view_frame Area available to the grid view (header +
		content).
	*/
	void
	reflow_view(
		Rect const& view_frame
	) noexcept;

	/**
		Render the view.

		@note This calls render_content(). If the header is enabled,
		render_header() is also called.

		@param grid_rd Render data.
		@param all Whether to render all cells or to only render
		queued cells.
	*/
	void
	render_view(
		ui::GridRenderData& grid_rd,
		bool all
	) noexcept;
/// @}

/** @name Implementation */ /// @{
	/**
		Perform content action.

		@note @a row_end will be a count if an insertion action is
		requested.

		@param action Content action.
		@param row_begin Start of row range.
		@param count Number of rows.
	*/
	virtual void
	content_action(
		ui::ProtoGrid::ContentAction action,
		ui::index_type row_begin,
		ui::index_type count
	) noexcept = 0;

	/**
		Render header cells.

		@note Indices and ranges will always be within the bounds of
		the current dimensions.

		@param grid_rd Render data.
		@param col_begin Start of column range.
		@param col_end End of column range (non-inclusive).
		@param frame Frame of the entire header row.
	*/
	virtual void
	render_header(
		ui::GridRenderData& grid_rd,
		ui::index_type const col_begin,
		ui::index_type const col_end,
		Rect const& frame
	) noexcept = 0;

	/**
		Render content cells.

		@note Indices and ranges will always be within the bounds of
		the current dimensions.

		@param grid_rd Render data.
		@param row_begin Start of row range.
		@param row_end End of row range (non-inclusive).
		@param col_begin Start of column range.
		@param col_end End of column range (non-inclusive).
		@param frame Frame of the entire visible row.
	*/
	virtual void
	render_content(
		ui::GridRenderData& grid_rd,
		ui::index_type const row_begin,
		ui::index_type const row_end,
		ui::index_type const col_begin,
		ui::index_type const col_end,
		Rect const& frame
	) noexcept = 0;
/// @}

protected:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	virtual
	~ProtoGrid() noexcept override = 0;

	/**
		Constructor.

		@param type %Widget type.
		@param flags Flags.
		@param group Property group name.
		@param geometry Geometry.
		@param root %Root.
		@param parent Parent.
		@param col_count Number of columns.
		@param row_count Number of rows.

	*/
	ProtoGrid(
		ui::Widget::Type const type,
		ui::Widget::Flags const flags,
		ui::group_hash_type const group,
		ui::Geom&& geometry,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent,
		ui::index_type const col_count,
		ui::index_type const row_count
	) noexcept
		: base_type(
			type,
			flags,
			group,
			std::move(geometry),
			std::move(root),
			std::move(parent)
		)
		, m_col_count(max_ce(ui::index_type{0}, col_count))
		, m_row_count(max_ce(ui::index_type{0}, row_count))
	{}

	/** Move constructor. */
	ProtoGrid(ProtoGrid&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	ProtoGrid& operator=(ProtoGrid&&) = default;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Check if the header is enabled.

		@sa set_header_enabled()
	*/
	bool
	is_header_enabled() const noexcept {
		return m_header_enabled;
	}

	/**
		Get the number of columns.
	*/
	ui::index_type
	get_col_count() const noexcept {
		return m_col_count;
	}

	/**
		Get the number of rows.
	*/
	ui::index_type
	get_row_count() const noexcept {
		return m_row_count;
	}

	/**
		Get view.
	*/
	GridView const&
	get_view() const noexcept {
		return m_view;
	}
/// @}

/** @name Content */ /// @{
	/**
		Unselect all rows.
	*/
	void
	select_none() noexcept {
		content_action(ContentAction::unselect, 0, get_row_count());
	}

	/**
		Select all rows.
	*/
	void
	select_all() noexcept {
		content_action(ContentAction::select, 0, get_row_count());
	}

	/**
		Select or unselect a row range.

		@param select Whether to select or unselect the range.
		@param row_begin Start of range.
		@param count Number of rows to select.
	*/
	void
	select(
		bool const select,
		ui::index_type row_begin,
		ui::index_type const count
	) noexcept {
		row_begin = value_clamp(row_begin, 0, get_row_count());
		content_action(
			select
				? ContentAction::select
				: ContentAction::unselect
			,
			row_begin,
			max_ce(0, count)
		);
	}

	/**
		Flip the selection of a row range.

		@param row_begin Start of range.
		@param count Number of rows to flip.
	*/
	void
	select_toggle(
		ui::index_type row_begin,
		ui::index_type const count
	) noexcept {
		row_begin = value_clamp(row_begin, 0, get_row_count());
		content_action(ContentAction::select_toggle, row_begin, max_ce(0, count));
	}

	/**
		Insert rows before a position.

		@note Inserts @a count rows at @a row_begin.

		@param row_begin Position to insert at.
		@param count Number of rows to insert.
	*/
	void
	insert_before(
		ui::index_type row_begin,
		ui::index_type const count
	) noexcept {
		row_begin = value_clamp(row_begin, 0, get_row_count());
		content_action(ContentAction::insert_before, row_begin, max_ce(0, count));
	}

	/**
		Insert rows after a position.

		@note Inserts @a count rows at @a row_begin.

		@param row_begin Position to insert at.
		@param count Number of rows to insert.
	*/
	void
	insert_after(
		ui::index_type row_begin,
		ui::index_type const count
	) noexcept {
		row_begin = value_clamp(row_begin, 0, get_row_count());
		content_action(ContentAction::insert_after, row_begin, max_ce(0, count));
	}

	/**
		Erase a row range.

		@param row_begin Start of range.
		@param count Number of rows to erase.
	*/
	void
	erase(
		ui::index_type row_begin,
		ui::index_type const count
	) noexcept {
		row_begin = value_clamp(row_begin, 0, get_row_count());
		content_action(ContentAction::erase, row_begin, max_ce(0, count));
	}

	/**
		Erase all selected rows.
	*/
	void
	erase_selected() noexcept {
		content_action(ContentAction::erase_selected, 0, 0);
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
