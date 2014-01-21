
#include <Beard/String.hpp>
#include <Beard/txt/Defs.hpp>
#include <Beard/txt/Tree.hpp>
#include <Beard/txt/Node.hpp>
#include <Beard/txt/Cursor.hpp>

#include <cassert>
#include <iostream>

using namespace Beard;

void
print_tree(
	String const& name,
	txt::Tree const& tree
) {
	std::cout << "tree '" << name << "':\n";
	for (auto const& node : tree) {
		std::cout
			<< "¦"
			<< node.to_string()
			<< "¦\n"
		;
	}
	std::cout << '\n';
}

signed
main() {
	txt::Tree const t_1{String{"line 1"}};
	txt::Tree const t_2{String{"line 1\nline 2"}};
	txt::Tree const t_3{String{"line 1\nline 2\n"}};

	assert(
		1u == t_1.lines() &&
		6u == t_1.units() &&
		6u == t_1.points()
	);
	assert(
		2u == t_2.lines() &&
		12u == t_2.units() &&
		12u == t_2.points()
	);
	assert(
		3u == t_3.lines() &&
		12u == t_3.units() &&
		12u == t_3.points()
	);

	print_tree("t_1", t_1);
	print_tree("t_2", t_2);
	print_tree("t_3", t_3);

	std::cout.flush();
	return 0;
}
