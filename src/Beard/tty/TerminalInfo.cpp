/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Beard/tty/TerminalInfo.hpp>

#include <duct/EndianUtils.hpp>
#include <duct/IO/arithmetic.hpp>
#include <duct/IO/unicode.hpp>

#include <istream>

#include <Beard/detail/gr_ceformat.hpp>

namespace Beard {
namespace tty {

// class TerminalInfo implementation

#define BEARD_SCOPE_CLASS tty::TerminalInfo

TerminalInfo::~TerminalInfo() noexcept = default;

TerminalInfo::TerminalInfo() noexcept
	: m_initialized(false)
	, m_names()
	, m_cap_flags()
	, m_cap_numbers()
	, m_cap_strings()
{}

TerminalInfo::TerminalInfo(TerminalInfo&&) noexcept = default;
TerminalInfo::TerminalInfo(TerminalInfo const&) = default;
TerminalInfo& TerminalInfo::operator=(TerminalInfo&&) noexcept = default;

// serialization

// TODO: <tic.h> specifies some differences:
// 1. max names field size is 512 (XSI);
// 2. there are two different "signed" values specifying
//	  different meanings for capabilities;

// terminfo format:
/*
	uint16_t magic = 0x011a

	uint16_t names_size
	uint16_t flag_count
	uint16_t number_count
	uint16_t string_offset_count
	uint16_t string_table_size

	// Names for terminal type, separated by '|'
	char names[names_size]

	// Boolean flags
	uint8_t flags[flag_count]

	// Seek ahead to align to 2-byte word (ergo: possible dead byte)

	uint16_t numbers[number_count]

	// Offsets are relative to string_table
	uint16_t string_offsets[string_offset_count]
	char string_table[string_table_size]
*/

namespace {
enum : unsigned {
	terminfo_magic = 0x011a,
	terminfo_max_names_size = 128u,

	terminfo_table_offset_empty = 0xFFFFu,
	mask_offset_signbit = 0x8000,
};

constexpr auto const
terminfo_endian = duct::Endian::little;

struct terminfo_header {
	std::uint16_t magic{0};
	std::uint16_t names_size{0};
	std::uint16_t flag_count{0};
	std::uint16_t number_count{0};
	std::uint16_t string_offset_count{0};
	std::uint16_t string_table_size{0};
};
} // anonymous namespace

#define BEARD_TERMINFO_CHECK_IO_ERROR_(m_)				\
	if (stream.fail()) {								\
		BEARD_THROW_FQN(								\
			ErrorCode::serialization_io_failed,			\
			m_											\
		);												\
	}
//

#define BEARD_SCOPE_FUNC deserialize
namespace {
BEARD_DEF_FMT_FQN(
	s_err_bad_magic,
	"bad magic encountered: expected %-#04x, got %-#04x"
);
BEARD_DEF_FMT_FQN(
	s_err_name_too_large,
	"names section too large: expected s <= %u, got s = %u"
);
BEARD_DEF_FMT_FQN(
	s_err_string_offset_invalid,
	"index %u offset %u overflows string table (size = %u)"
);
} // anonymous namespace

void
TerminalInfo::deserialize(
	std::istream& stream
) {
	m_initialized = false;
	m_names.clear();
	m_cap_flags.clear();
	m_cap_numbers.clear();
	m_cap_strings.clear();

	terminfo_header hdr{};

	// header
	duct::IO::read_arithmetic(stream, hdr.magic, terminfo_endian);
	duct::IO::read_arithmetic(stream, hdr.names_size, terminfo_endian);
	duct::IO::read_arithmetic(stream, hdr.flag_count, terminfo_endian);
	duct::IO::read_arithmetic(stream, hdr.number_count, terminfo_endian);
	duct::IO::read_arithmetic(stream, hdr.string_offset_count, terminfo_endian);
	duct::IO::read_arithmetic(stream, hdr.string_table_size, terminfo_endian);

	BEARD_TERMINFO_CHECK_IO_ERROR_(
		"failed to read header"
	);

	if (terminfo_magic != hdr.magic) {
		BEARD_THROW_FMT(
			ErrorCode::serialization_data_malformed,
			s_err_bad_magic,
			static_cast<unsigned>(terminfo_magic),
			static_cast<unsigned>(hdr.magic)
		);
	}

	if (terminfo_max_names_size < hdr.names_size) {
		BEARD_THROW_FMT(
			ErrorCode::serialization_data_malformed,
			s_err_name_too_large,
			static_cast<unsigned>(terminfo_max_names_size),
			static_cast<unsigned>(hdr.names_size)
		);
	}

	// names
	// Assuming ASCII encoding -- compatible with UTF-8, so no
	// decoding necessary.

	String names_glob{};
	duct::IO::read_string_copy(
		stream,
		names_glob,
		static_cast<std::size_t>(hdr.names_size),
		terminfo_endian
	);
	BEARD_TERMINFO_CHECK_IO_ERROR_(
		"failed to read names field"
	);

	if (0u < names_glob.size() && '\0' == *names_glob.crbegin()) {
		names_glob.pop_back();
	}

	String::size_type pos = 0u, next = String::npos;
	for (;;) {
		next = names_glob.find('|', pos + 1u);
		if (String::npos == next) {
			m_names.emplace_back(names_glob.substr(pos, String::npos));
			break;
		} else if (next > (pos + 1u)) {
			m_names.emplace_back(names_glob.substr(pos, next - pos));
			pos = next + 1u;
		} else {
			// next <= pos (empty block)
			++pos;
			continue;
		}
	}

	// flags
	m_cap_flags.resize(static_cast<std::size_t>(hdr.flag_count));
	duct::IO::read_arithmetic_array(
		stream,
		m_cap_flags.data(),
		m_cap_flags.size(),
		terminfo_endian
	);
	BEARD_TERMINFO_CHECK_IO_ERROR_(
		"failed to read flag section"
	);

	// align
	// names_size and flag_count will indicate unalignment if
	// their sum is uneven because their respective elements are
	// bytes.
	if ((hdr.names_size + hdr.flag_count) % 2) {
		char dead_byte;
		stream.read(&dead_byte, 1u);
		BEARD_TERMINFO_CHECK_IO_ERROR_(
			"failed to read dead byte for alignment"
		);
	}

	// numbers
	m_cap_numbers.resize(static_cast<std::size_t>(hdr.number_count));
	duct::IO::read_arithmetic_array(
		stream,
		m_cap_numbers.data(),
		m_cap_numbers.size(),
		terminfo_endian
	);
	BEARD_TERMINFO_CHECK_IO_ERROR_(
		"failed to read number section"
	);

	// string offsets
	aux::vector<std::uint16_t> string_offsets(
		static_cast<std::size_t>(hdr.string_offset_count)
	);
	duct::IO::read_arithmetic_array(
		stream,
		string_offsets.data(),
		string_offsets.size(),
		terminfo_endian
	);
	BEARD_TERMINFO_CHECK_IO_ERROR_(
		"failed to read string offsets section"
	);

	// string table
	// Again assuming ASCII.
	aux::vector<char> string_table(
		static_cast<std::size_t>(hdr.string_table_size)
	);
	duct::IO::read(
		stream,
		string_table.data(),
		string_table.size()
	);
	BEARD_TERMINFO_CHECK_IO_ERROR_(
		"failed to read string table"
	);

	unsigned index = 0u, offset = 0u;
	auto
		it_start = string_table.cend(),
		it_cnull = it_start
	;
	for (
		auto it = string_offsets.cbegin();
		string_offsets.cend() != it;
		++it, ++index
	) {
		offset = static_cast<unsigned>(*it);

		// -1 means terminal does not support capability, and
		// "other negative values are illegal".
		// And in Unix fashion, /you will get illegal values/.
		if (
			terminfo_table_offset_empty == offset
		||  (mask_offset_signbit & offset)
		) {
			continue;
		}

		if (string_table.size() <= offset) {
			BEARD_THROW_FMT(
				ErrorCode::serialization_data_malformed,
				s_err_string_offset_invalid,
				index,
				offset,
				string_table.size()
			);
		}

		it_start = string_table.cbegin() + offset;
		it_cnull = std::find(
			it_start,
			string_table.cend(),
			'\0'
		);
		m_cap_strings.emplace(
			index,
			String{it_start, it_cnull}
		);
	}

	m_initialized = true;
}
#undef BEARD_SCOPE_FUNC

#undef BEARD_SCOPE_CLASS

} // namespace tty
} // namespace Beard
