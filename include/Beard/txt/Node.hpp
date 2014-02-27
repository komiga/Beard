/**
@file txt/Node.hpp
@brief Text node class.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_TXT_NODE_HPP_
#define BEARD_TXT_NODE_HPP_

#include <Beard/config.hpp>
#include <Beard/String.hpp>

namespace Beard {
namespace txt {

// Forward declarations
class Tree; // external
class Node;
class Cursor; // external

/**
	@addtogroup txt
	@{
*/

/**
	Text node.

	This uses a buffer in UTF-8, so code units are 8 bits.
*/
class Node final {
	friend class txt::Tree;
	friend class txt::Cursor;

public:
	/** Buffer type. */
	using buffer_type = aux::vector<char8>;

	/** Buffer iterator type. */
	using iterator = buffer_type::iterator;
	/** Const buffer iterator type. */
	using const_iterator = buffer_type::const_iterator;

private:
	buffer_type m_buffer{};
	std::size_t m_pcount{0u};

	Node(Node const&) = delete;
	Node& operator=(Node const&) = delete;

	Node(
		char8 const* const first,
		char8 const* const last,
		std::size_t const pcount
	)
		: m_buffer(first, last)
		, m_pcount(pcount)
	{}

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Node() = default;

	/** Default constructor. */
	Node() = default;

	/**
		Constructor with capacity.

		@param capacity Capacity.
	*/
	Node(
		std::size_t const capacity
	) {
		m_buffer.reserve(capacity);
	}

	/** Move constructor. */
	Node(Node&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Node& operator=(Node&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get number of code units.
	*/
	std::size_t
	units() const noexcept {
		return m_buffer.size();
	}

	/**
		Get number of code points.
	*/
	std::size_t
	points() const noexcept {
		return m_pcount;
	}

	/**
		Get capacity in code units.
	*/
	std::size_t
	capacity() const noexcept {
		return m_buffer.capacity();
	}

	/**
		Check if the node contains only single units.

		i.e., <code>units() == points()</code>.
	*/
	bool
	singular() const noexcept {
		return units() == points();
	}

	/**
		Check if the node is empty.
	*/
	bool
	empty() const noexcept {
		return m_buffer.empty();
	}

	/**
		Get iterator to beginning (code units).
	*/
	iterator
	begin() noexcept {
		return m_buffer.begin();
	}
	/**
		Get iterator to beginning (code units).
	*/
	const_iterator
	begin() const noexcept {
		return m_buffer.begin();
	}
	/**
		Get iterator to beginning (code units).
	*/
	const_iterator
	cbegin() const noexcept {
		return m_buffer.cbegin();
	}

	/**
		Get iterator to end (code units).
	*/
	iterator
	end() noexcept {
		return m_buffer.end();
	}
	/**
		Get iterator to end (code units).
	*/
	const_iterator
	end() const noexcept {
		return m_buffer.end();
	}
	/**
		Get iterator to end (code units).
	*/
	const_iterator
	cend() const noexcept {
		return m_buffer.cend();
	}
/// @}

/** @name Operations */ /// @{
	/**
		Free unused capacity.
	*/
	void
	shrink_to_fit() {
		m_buffer.shrink_to_fit();
	}

	/**
		Convert the node to a string.
	*/
	String
	to_string() const noexcept {
		return String{m_buffer.cbegin(), m_buffer.cend()};
	}
/// @}
};

/** @} */ // end of doc-group txt

} // namespace txt
} // namespace Beard

#endif // BEARD_TXT_NODE_HPP_
