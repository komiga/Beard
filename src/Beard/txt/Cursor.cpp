/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/utility.hpp>
#include <Beard/txt/Cursor.hpp>

#include <duct/char.hpp>

#include <cmath>
#include <iterator>

namespace Beard {
namespace txt {

// class Cursor implementation

// positioning

bool
Cursor::row_bound() noexcept {
	if (signed_cast(get_tree().lines()) <= m_row) {
		row_extent(txt::Extent::tail);
		return true;
	}
	return false;
}

void
Cursor::row_abs(
	position_type row
) noexcept {
	row = max_ce(
		row,
		max_ce(
			signed_cast(std::size_t{0u}),
			signed_cast(get_tree().lines()) - 1
		)
	);
	if (row != m_row) {
		m_row = row;
		col_recalc();
	}
}

void
Cursor::col_recalc() noexcept {
	auto const& node = get_node();
	if (0 >= m_col) {
		m_col = 0;
		m_index = 0;
	} else if (node.points() <= unsigned_cast(m_col)) {
		m_col = signed_cast(node.points());
		m_index = signed_cast(node.units());
	} else if (node.singular()) {
		m_index = m_col;
	} else {
		position_type col = 0;
		auto const end = node.cend();
		auto step = node.cbegin(), from = step;
		while (
			from < (step = txt::EncUtils::next(from, end)) &&
			col > m_col
		) {
			from = step;
			++col;
		}
		m_col = col;
		m_index = std::distance(node.cbegin(), from);
	}
}

void
Cursor::col_step(
	difference_type const n
) noexcept {
	if (0 == n) {
		return;
	}

	auto const& node = get_node();
	difference_type const dest = m_col + n;
	/*DUCT_DEBUGF(
		"col_step: m_col = %zd, m_index = %zd, dest = %ld, n = %ld, "
		"diff = %zd, abs = %zd",
		m_col, m_index, dest, n,
		dest - m_col,
		std::abs(dest - m_col)
	);*/

	// Recalculate (i.e., count from the beginning) or step
	// depending on the distance from the current column
	if (0 >= dest) {
		m_col = 0;
		m_index = 0;
	} else if (node.points() <= unsigned_cast(dest)) {
		m_col = signed_cast(node.points());
		m_index = signed_cast(node.units());
	} else if (node.singular()) {
		m_col = dest;
		m_index = dest;
	} else if (dest < std::abs(dest - m_col)) {
		m_col = dest;
		col_recalc();
	} else {
		auto const begin = node.cbegin(), end = node.cend();
		auto step = begin + m_index, from = step;
		if (0 > n) {
			// Step backward
			while (
				from > (step = txt::EncUtils::prev(from, begin)) &&
				dest < m_col
			) {
				from = step;
				--m_col;
			}
		} else {
			// Step forward
			while (
				from < (step = txt::EncUtils::next(from, end)) &&
				dest > m_col
			) {
				from = step;
				++m_col;
			}
		}
		m_index = std::distance(begin, from);
		/*DUCT_DEBUGF(
			"  m_col = %zd, m_index = %zd, dist = %ld",
			m_col,
			m_index,
			std::distance(begin, from)
		);*/
	}
}

// operations

void
Cursor::assign(
	String const& str
) {
	auto& node = get_node();
	auto const ucount = signed_cast(node.units());
	auto const pcount = signed_cast(node.points());
	node.m_buffer.assign(str.cbegin(), str.cend());
	auto const new_pcount = signed_cast(
		txt::EncUtils::count(str.cbegin(), str.cend(), false)
	);
	get_tree().update_counts(
		node,
		signed_cast(node.units()) - ucount,
		new_pcount - pcount
	);
	col_recalc();
}

std::size_t
Cursor::insert(
	char32 const cp
) {
	txt::EncUtils::char_type units[txt::EncUtils::max_units];
	auto const it = txt::EncUtils::encode(
		cp,
		std::begin(units),
		duct::CHAR_NULL
	);
	if (std::begin(units) != it) {
		auto& node = get_node();
		node.m_buffer.insert(node.cbegin() + m_index, std::begin(units), it);
		auto const size = std::distance(std::begin(units), it);
		get_tree().update_counts(node, size, 1);
		return unsigned_cast(size);
	} else {
		// Invalid code point (ignored)
		return 0u;
	}
}

std::size_t
Cursor::insert_step(
	char32 const cp
) {
	auto const size = insert(cp);
	if (0u < size) {
		/*DUCT_DEBUGF(
			"insert_step: m_col = %zd, m_index = %zd, size = %zu",
			m_col, m_index, size
		);*/
		++m_col;
		m_index += size;
	}
	return size;
}

std::size_t
Cursor::erase() {
	auto& node = get_node();
	/*DUCT_DEBUGF(
		"erase: m_col = %zd, m_index = %zd, ucount = %zu"
		", tree units: %zu",
		m_col, m_index, node.units(),
		get_tree().units()
	);*/
	if (signed_cast(node.units()) <= m_index) {
		return 0u;
	}

	auto const it = node.cbegin() + m_index;
	auto const size = signed_cast(txt::EncUtils::required_first_whole(*it));
	if (signed_cast(node.units()) >= m_index + size) {
		node.m_buffer.erase(it, it + size);
		get_tree().update_counts(node, -size, -1);
		return size;
	} else {
		// Incomplete sequence
		//DUCT_DEBUG("Cursor::erase: ics");
		return 0u;
	}
}

std::size_t
Cursor::erase_before() {
	/*DUCT_DEBUGF(
		"erase_before: m_col = %zu, m_index = %zu",
		m_col, m_index
	);*/
	if (0 < m_col) {
		col_prev();
		return erase();
	} else {
		// Nothing preceding the cursor to erase
		return 0u;
	}
}

} // namespace Beard
} // namespace txt
