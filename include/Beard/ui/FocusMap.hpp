/**
@file ui/FocusMap.hpp
@brief UI widget focus index map class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>

#include <stdexcept>
#include <iterator>

namespace Beard {
namespace ui {

// Forward declarations
class FocusMap;

/**
	@addtogroup ui
	@{
*/

/**
	%Widget focus index map.

	@note This is implemented in terms of an aux::multimap, so its
	guarantees generally hold.

	@par
	@note Insertion does not invalidate iterators or references.
	Erasure invalidates only the removed elements.
*/
class FocusMap final {
public:
	/** Focus index map. */
	using map_type = aux::multimap<
		ui::focus_index_type,
		ui::Widget::WPtr
	>;

	/** Focus index map iterator. */
	using iterator = map_type::iterator;
	/** Focus index map const iterator. */
	using const_iterator = map_type::const_iterator;

private:
	map_type m_map;

	FocusMap(FocusMap const&) = delete;
	FocusMap& operator=(FocusMap const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~FocusMap() noexcept = default;

	/** Default constructor. */
	FocusMap() noexcept = default;

	/** Move constructor. */
	FocusMap(FocusMap&&) noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	FocusMap& operator=(FocusMap&&) noexcept = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get map.
	*/
	map_type const&
	get_map() const noexcept {
		return m_map;
	}

	/**
		Get size of map.
	*/
	std::size_t
	size() const noexcept {
		return m_map.size();
	}

	/**
		Check if map is empty.
	*/
	bool
	empty() const noexcept {
		return m_map.empty();
	}

	/**
		Get iterator to beginning of map.
	*/
	iterator
	begin() noexcept {
		return m_map.begin();
	}

	/**
		Get iterator to beginning of map.
	*/
	const_iterator
	cbegin() const noexcept {
		return m_map.cbegin();
	}

	/**
		Get iterator to end of map.
	*/
	iterator
	end() noexcept {
		return m_map.end();
	}

	/**
		Get iterator to end of map.
	*/
	const_iterator
	cend() const noexcept {
		return m_map.cend();
	}

	/**
		Get iterator to last element.

		@returns Iterator to @c std::prev(cend()), or cend() if map
		is empty.
	*/
	const_iterator
	clast() const noexcept {
		return empty() ? m_map.cend() : std::prev(m_map.cend());
	}
/// @}

/** @name Lookup */ /// @{
	/**
		Lookup iterator for widget.

		@returns Iterator for @a widget.
		@param widget %Widget to lookup.
	*/
	const_iterator
	find(
		ui::Widget::SPtr const& widget
	) const noexcept {
		if (widget) {
			ui::focus_index_type const index = widget->get_focus_index();
			if (ui::focus_index_none != index) {
				auto const pair = equal_range(index);
				for (auto it = pair.first; pair.second != it; ++it) {
					if (
						!it->second.expired() &&
						widget == it->second.lock()
					) {
						return it;
					}
				}
			}
		}
		return cend();
	}
/// @}

/** @name Operations */ /// @{
	/**
		Clear map.
	*/
	void
	clear() noexcept {
		m_map.clear();
	}

	/**
		Insert an index-widget pair.

		@throws std::invalid_argument
		If @a widget is null.

		@returns Iterator to the inserted pair.
		@param index Index.
		@param widget %Widget.
	*/
	const_iterator
	emplace(
		ui::focus_index_type const index,
		ui::Widget::SPtr const& widget
	) {
		if (!widget) {
			throw std::invalid_argument(
				"FocusMap::emplace(): widget is invalid"
			);
		}
		return m_map.emplace(index, ui::Widget::WPtr(widget));
	}

	/**
		Remove by iterator.

		@warning Attempting to erase @c end() is undefined behavior.

		@returns Iterator following @a pos.
		@param pos Iterator to remove.
	*/
	const_iterator
	erase(
		const_iterator const pos
	) {
		return m_map.erase(pos);
	}

	/**
		Remove widget.

		@returns Pair containing the iterator following the widget
		(cend() if the widget was not found) and boolean success.
		@param widget %Widget to remove.
	*/
	std::pair<const_iterator, bool>
	erase(
		ui::Widget::SPtr const& widget
	) {
		auto const it = find(widget);
		if (cend() != it) {
			return {m_map.erase(it), true};
		}
		return {cend(), false};
	}

	/**
		Get a pair of iterators for all widgets with an index.

		@returns Pair of <code>[first; last)</code> iterators, where
		@c first is <em>not less</em> than @a index and where @c last
		is <em>greater</em> than @c index. If there is no index
		satisfying a relation, its corresponding value is @c cend().
		Thus, if there are no widgets with the given index,
		<code>first == last</code>.
		@param index Index.
	*/
	std::pair<const_iterator, const_iterator>
	equal_range(
		ui::focus_index_type const index
	) const noexcept {
		return m_map.equal_range(index);
	}

	/**
		Get iterator to previous widget.

		@param pos Iterator to step from.
	*/
	const_iterator
	prev(
		const_iterator const& pos
	) noexcept {
		return (cbegin() != pos)
			? std::prev(pos)
			: clast()
		;
	}

	/**
		Get iterator to next widget.

		@param pos Iterator to step from.
	*/
	const_iterator
	next(
		const_iterator const& pos
	) noexcept {
		return (clast() != pos && cend() != pos)
			? std::next(pos)
			: cbegin()
		;
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

