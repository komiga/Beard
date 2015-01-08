/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/utility.hpp>
#include <Beard/txt/Defs.hpp>
#include <Beard/txt/Tree.hpp>

#include <utility>
#include <iterator>

namespace Beard {
namespace txt {

// class Tree implementation

Tree::Tree(
	String const& str
)
	: m_nodes()
{
	auto first = str.cbegin(), last = first;
	std::size_t pcount = 0u;
	for (; str.cend() > last;) {
		if ('\n' == *last) {
			m_nodes.emplace_back(std::move(txt::Node{&*first, &*last, pcount}));
			m_ucount += unsigned_cast(std::distance(first, last));
			m_pcount += pcount;
			++last;
			first = last;
			pcount = 0u;
		} else {
			last += txt::EncUtils::required_first_whole(*last);
			++pcount;
		}
	}
	if (0u < pcount) {
		m_nodes.emplace_back(std::move(txt::Node{&*first, &*last, pcount}));
		m_ucount += unsigned_cast(std::distance(first, last));
		m_pcount += pcount;
	} else if (m_nodes.empty() || !str.empty()) {
		m_nodes.emplace_back();
	}
}

void
Tree::update_counts(
	txt::Node& node,
	std::ptrdiff_t const udiff,
	std::ptrdiff_t const pdiff
) noexcept {
	// aiethjiorthjr
	m_pcount = unsigned_cast(max_ce(
		std::ptrdiff_t{0}, signed_cast(m_pcount) + pdiff
	));
	m_ucount = unsigned_cast(max_ce(
		std::ptrdiff_t{0}, signed_cast(m_ucount) + udiff
	));
	node.m_pcount = unsigned_cast(max_ce(
		std::ptrdiff_t{0}, signed_cast(node.m_pcount) + pdiff
	));
}

String
Tree::to_string() const noexcept {
	String str;
	str.reserve(units() + lines());
	for (
		auto it = m_nodes.cbegin();
		m_nodes.cend() != it;
		++it
	) {
		str.append(
			it->m_buffer.cbegin(),
			it->m_buffer.cend()
		);
		if (m_nodes.cend() != (it + 1)) {
			str.push_back('\n');
		}
	}
	return str;
}

} // namespace txt
} // namespace Beard
