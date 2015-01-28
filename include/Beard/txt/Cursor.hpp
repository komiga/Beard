/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Text cursor.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/String.hpp>
#include <Beard/utility.hpp>
#include <Beard/txt/Defs.hpp>
#include <Beard/txt/Tree.hpp>
#include <Beard/txt/Node.hpp>

#include <limits>
#include <functional>

namespace Beard {
namespace txt {

// Forward declarations
class Cursor;

/**
	@addtogroup txt
	@{
*/

/**
	Text cursor.
*/
class Cursor final {
public:
	/** Position type. */
	using position_type = txt::Node::buffer_type::difference_type;

	/** Position difference type. */
	using difference_type = txt::Node::buffer_type::difference_type;

private:
	std::reference_wrapper<txt::Tree> m_tree;
	position_type m_row;
	position_type m_col;
	position_type m_index;

	Cursor() = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Cursor() = default;

	/**
		Constructor with tree.

		@param tree Text tree.
	*/
	Cursor(
		txt::Tree& tree
	) noexcept
		: m_tree(tree)
		, m_row(0u)
		, m_col(0u)
		, m_index(0u)
	{}

	/** Copy constructor. */
	Cursor(Cursor const&) = default;
	/** Move constructor. */
	Cursor(Cursor&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	Cursor& operator=(Cursor const&) = default;
	/** Move assignment operator. */
	Cursor& operator=(Cursor&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get tree (mutable).
	*/
	txt::Tree&
	tree() noexcept {
		return m_tree;
	}

	/**
		Get tree.
	*/
	txt::Tree const&
	tree() const noexcept {
		return m_tree;
	}

	/**
		Get node.
	*/
	txt::Node&
	node() {
		return tree().nodes().at(m_row);
	}

	/**
		Get row.
	*/
	position_type
	row() const noexcept {
		return m_row;
	}

	/**
		Get column.
	*/
	position_type
	col() const noexcept {
		return m_col;
	}

	/**
		Get code unit index.
	*/
	position_type
	index() const noexcept {
		return m_index;
	}
/// @}

/** @name Positioning */ /// @{
	/**
		Ensure row is within bounds.

		@note If the row is modified, col_recalc() is also called.

		@returns @c true if the row was modified.
	*/
	bool
	row_bound() noexcept;

	/**
		Move to specific row.

		@param row New row.
	*/
	void
	row_abs(
		position_type row
	) noexcept;

	/**
		Move to row extent.

		@param extent %Extent.
	*/
	void
	row_extent(
		txt::Extent const extent
	) noexcept {
		switch (extent) {
		case txt::Extent::head:
			row_abs(0u);
			break;

		case txt::Extent::tail:
			row_abs(std::numeric_limits<position_type>::max());
			break;
		}
	}

	/**
		Move to row by offset.

		@param n Number of rows to step.
	*/
	void
	row_step(
		difference_type const n
	) noexcept {
		row_abs(min_ce(position_type{0}, m_row + n));
	}

	/**
		Move to previous row.
	*/
	void
	row_prev() noexcept {
		row_step(-1);
	}

	/**
		Move to next row.
	*/
	void
	row_next() noexcept {
		row_step(+1);
	}

	/**
		Recalculate column unit index.
	*/
	void
	col_recalc() noexcept;

	/**
		Move to specific column.

		@param col New column.
	*/
	void
	col_abs(
		position_type const col
	) noexcept {
		col_step(col - m_col);
	}

	/**
		Move to column extent.

		@param extent %Extent.
	*/
	void
	col_extent(
		txt::Extent const extent
	) noexcept {
		switch (extent) {
		case txt::Extent::head:
			m_col = 0;
			m_index = 0;
			break;

		case txt::Extent::tail:
			auto const& node = this->node();
			m_col = signed_cast(node.points());
			m_index = signed_cast(node.units());
			break;
		}
	}

	/**
		Move to column by offset.

		@param n Number of columns to step.
	*/
	void
	col_step(
		difference_type const n
	) noexcept;

	/**
		Move to previous column.
	*/
	void
	col_prev() noexcept {
		col_step(-1);
	}

	/**
		Move to next column.
	*/
	void
	col_next() noexcept {
		col_step(+1);
	}
/// @}

/** @name Operations */ /// @{
	/**
		Clear the current row.
	*/
	void
	clear();

	/**
		Assign current row to string.

		@param str %String.
		@param size Size of string.
	*/
	void
	assign(
		char8 const* const str,
		unsigned const size
	);

	/**
		Assign current row to string.

		@param str %String.
	*/
	void
	assign(
		String const& str
	) {
		assign(str.data(), str.size());
	}

	/**
		Insert a code point.

		@returns The number of code units inserted.
		@param cp Code point to insert.
	*/
	std::size_t
	insert(
		char32 const cp
	);

	/**
		Insert a code point and step the cursor forward.

		@returns The number of code units inserted.
		@param cp Code point to insert.
	*/
	std::size_t
	insert_step(
		char32 const cp
	);

	/**
		Erase the code point at the cursor.

		@note This operation corresponds to the delete key.

		@returns The number of code units erased.
	*/
	std::size_t
	erase();

	/**
		Erase the code point before the cursor.

		@note This operation corresponds to the backspace key.

		@returns The number of code units erased.
	*/
	std::size_t
	erase_before();
/// @}
};

/** @} */ // end of doc-group txt

} // namespace txt
} // namespace Beard
