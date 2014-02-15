
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>

#include <duct/debug.hpp>

#include <iostream>

#include "../common/common.hpp"

using namespace Beard;

static char const* const
s_rel_names[]{
	"equal",
	"disjoint_before",
	"disjoint_after",
	"subset",
	"superset",
	"intersection_before",
	"intersection_after"
};

static RangeRel const
s_rel_opposite[]{
	RangeRel::equal,
	RangeRel::disjoint_after,
	RangeRel::disjoint_before,
	RangeRel::superset,
	RangeRel::subset,
	RangeRel::intersection_after,
	RangeRel::intersection_before
};

bool
test_rel(
	Vec2 const& basis,
	Vec2 const& other,
	RangeRel const expected
) noexcept {
	auto const rel = range_rel(basis, other);
	std::cout
		<< basis << ", "
		<< other << ": "
		<< s_rel_names[enum_cast(rel)] << ", "
		<< s_rel_names[enum_cast(expected)]
	<< std::endl;
	return rel == expected;
}

bool
test_rel_opp(
	Vec2 const& basis,
	Vec2 const& other,
	RangeRel const expected
) noexcept {
	bool const a = test_rel(basis, other, expected);
	bool const b = test_rel(other, basis, s_rel_opposite[enum_cast(expected)]);
	return a && b;
}

signed
main(
	signed /*argc*/,
	char* /*argv*/[]
) {
	DUCT_ASSERTE(test_rel({1,1}, {1,1}, RangeRel::equal));
	DUCT_ASSERTE(test_rel({0,1}, {0,1}, RangeRel::equal));
	DUCT_ASSERTE(test_rel_opp({0,1}, {1,2}, RangeRel::disjoint_before));

	DUCT_ASSERTE(test_rel_opp({0,1}, {0,2}, RangeRel::subset));
	DUCT_ASSERTE(test_rel_opp({1,2}, {0,2}, RangeRel::subset));

	DUCT_ASSERTE(test_rel_opp({0,2}, {1,3}, RangeRel::intersection_before));
	DUCT_ASSERTE(test_rel_opp({2,4}, {1,3}, RangeRel::intersection_after));

	return 0;
}
