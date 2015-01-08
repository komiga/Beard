/**
@file
@brief Text tree class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/String.hpp>
#include <Beard/txt/Node.hpp>

namespace Beard {
namespace txt {

// Forward declarations
class Tree;
class Cursor; // external

/**
	@addtogroup txt
	@{
*/

/**
	Text tree.

	@note The tree will always contain at least one txt::Node to
	simplify operations.

	@sa txt::Node,
		txt::Cursor
*/
class Tree final {
	friend class txt::Cursor;

public:
	/** %Node vector type. */
	using node_vector_type = aux::vector<txt::Node>;

	/** %Node iterator type. */
	using iterator = node_vector_type::iterator;
	/** Const node iterator type. */
	using const_iterator = node_vector_type::const_iterator;

private:
	node_vector_type m_nodes;
	std::size_t m_ucount{0u};
	std::size_t m_pcount{0u};

	Tree(Tree const&) = delete;
	Tree& operator=(Tree const&) = delete;

	void
	update_counts(
		txt::Node& node,
		std::ptrdiff_t const udiff,
		std::ptrdiff_t const pdiff
	) noexcept;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Tree() = default;

	/** Default constructor. */
	Tree()
		: m_nodes(1)
	{}

	/**
		Constructor with string.

		@param str %String.
	*/
	Tree(
		String const& str
	);

	/** Move constructor. */
	Tree(Tree&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Tree& operator=(Tree&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get nodes (mutable).
	*/
	node_vector_type&
	get_nodes() noexcept {
		return m_nodes;
	}

	/**
		Get nodes.
	*/
	node_vector_type const&
	get_nodes() const noexcept {
		return m_nodes;
	}

	/**
		Get the number of code units.

		@note This does not include line endings.
	*/
	std::size_t
	units() const noexcept {
		return m_ucount;
	}

	/**
		Get the number of code points.

		@note This does not include line endings.
	*/
	std::size_t
	points() const noexcept {
		return m_pcount;
	}

	/**
		Get the number of lines.
	*/
	std::size_t
	lines() const noexcept {
		return m_nodes.size();
	}

	/**
		Get iterator to beginning.
	*/
	iterator
	begin() noexcept {
		return m_nodes.begin();
	}
	/**
		Get iterator to beginning.
	*/
	const_iterator
	begin() const noexcept {
		return m_nodes.begin();
	}
	/**
		Get iterator to beginning.
	*/
	const_iterator
	cbegin() const noexcept {
		return m_nodes.cbegin();
	}

	/**
		Get iterator to end.
	*/
	iterator
	end() noexcept {
		return m_nodes.end();
	}
	/**
		Get iterator to end.
	*/
	const_iterator
	end() const noexcept {
		return m_nodes.end();
	}
	/**
		Get iterator to end.
	*/
	const_iterator
	cend() const noexcept {
		return m_nodes.cend();
	}
/// @}

/** @name Operations */ /// @{
	/**
		Convert the tree to a string.
	*/
	String
	to_string() const noexcept;
/// @}
};

/** @} */ // end of doc-group txt

} // namespace txt
} // namespace Beard
