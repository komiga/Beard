/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/tty/Ops.hpp>

#include <cstring>
#include <stdio.h>

namespace Beard {
namespace tty {

String
this_path() noexcept {
	char buf[L_ctermid];
	buf[L_ctermid - 1u] = '\0';
	::ctermid(buf);
	return String{buf};
}

} // namespace tty
} // namespace Beard
