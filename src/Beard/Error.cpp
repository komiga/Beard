
#include <Beard/utility.hpp>
#include <Beard/ErrorCode.hpp>
#include <Beard/String.hpp>
#include <Beard/Error.hpp>

#include <type_traits>
#include <utility>

namespace Beard {

namespace {
static char const
s_error_invalid[]{BEARD_STR_LIT("INVALID")},
* const s_error_names[]{
	BEARD_STR_LIT("unknown"),

// serialization
	BEARD_STR_LIT("serialization_io_failed"),
	BEARD_STR_LIT("serialization_data_malformed"),

// tty
	BEARD_STR_LIT("tty_terminal_already_open"),
	BEARD_STR_LIT("tty_terminal_info_uninitialized"),
	BEARD_STR_LIT("tty_sigwinch_handler_already_active"),
	BEARD_STR_LIT("tty_sigaction_failed"),
	BEARD_STR_LIT("tty_device_open_failed"),
	BEARD_STR_LIT("tty_invalid_fd"),
};
} // anonymous namespace

static_assert(
	enum_cast(ErrorCode::LAST)
	== std::extent<decltype(s_error_names)>::value,
	"ErrorCode name list is incomplete"
);

char const*
get_error_name(
	ErrorCode const error_code
) noexcept {
	std::size_t const index = static_cast<std::size_t>(error_code);
	if (index < std::extent<decltype(s_error_names)>::value) {
		return s_error_names[index];
	} else {
		return s_error_invalid;
	}
}

} // namespace Beard
