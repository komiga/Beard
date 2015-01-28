/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Terminal information.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/utility.hpp>
#include <Beard/aux.hpp>
#include <Beard/String.hpp>
#include <Beard/tty/Caps.hpp>

#include <iosfwd>

namespace Beard {
namespace tty {

// Forward declarations
class TerminalInfo;

/**
	@addtogroup tty
	@{
*/

/**
	%Terminal information.

	@note This class does not deserialize the ncurses extended
	storage format.

	@par
	@note See `man 5 term`.
*/
class TerminalInfo final {
public:
	/** Name vector type. */
	using name_vector_type = aux::vector<String>;

	/**
		Capability string map type.
	*/
	using cap_string_map_type
	= aux::unordered_map<
		unsigned,
		String
	>;

private:
	using cap_flag_vector_type = aux::vector<std::uint8_t>;
	using cap_number_vector_type = aux::vector<std::int16_t>;

	bool m_initialized;
	name_vector_type m_names;
	cap_flag_vector_type m_cap_flags;
	cap_number_vector_type m_cap_numbers;
	cap_string_map_type m_cap_strings;

	TerminalInfo& operator=(TerminalInfo const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~TerminalInfo() noexcept;

	/** Default constructor. */
	TerminalInfo() noexcept;
	/** Move constructor. */
	TerminalInfo(TerminalInfo&&) noexcept;
	/** Copy constructor. */
	TerminalInfo(TerminalInfo const&);
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	TerminalInfo& operator=(TerminalInfo&&) noexcept;
/// @}

/** @name Properties */ /// @{
	/**
		Check if the object has been initialized.
	*/
	bool
	is_initialized() const noexcept {
		return m_initialized;
	}

	/**
		Get name collection.
	*/
	name_vector_type const&
	names() const noexcept {
		return m_names;
	}
/// @}

/** @name Capabilities */ /// @{
	/**
		Get the value of a capability flag.

		@returns @c true if the flag is present; @c false if it is
		absent.
		@param cap Capability flag to test.
	*/
	tty::CapFlagValue
	cap_flag(
		tty::CapFlag const cap
	) const noexcept {
		return
			m_cap_flags.size() > enum_cast(cap)
			? 0u != m_cap_flags[enum_cast(cap)]
			: false
		;
	}

	/**
		Get capability flag count.
	*/
	std::size_t
	cap_flag_count() const noexcept {
		return m_cap_flags.size();
	}

	/**
		Get the value of a capability number.

		@returns The value of the capability number,
		or tty::CAP_NUMBER_NOT_SUPPORTED if the terminal does not
		support the capability.
		@param cap Capability number to fetch.
	*/
	tty::CapNumberValue
	cap_number(
		tty::CapNumber const cap
	) const noexcept {
		return
			m_cap_numbers.size() > enum_cast(cap)
			? m_cap_numbers[enum_cast(cap)]
			: tty::CAP_NUMBER_NOT_SUPPORTED
		;
	}

	/**
		Get capability number count.
	*/
	std::size_t
	cap_number_count() const noexcept {
		return m_cap_numbers.size();
	}

	/**
		Lookup a capability string.

		@returns @c true if @a iterator points to a valid iterator,
		or @c false if @a cap was not found.
		@param cap Capability string to lookup.
		@param iterator Capability string map iterator.
	*/
	bool
	lookup_cap_string(
		tty::CapString const cap,
		cap_string_map_type::const_iterator& iterator
	) const noexcept {
		iterator = m_cap_strings.find(enum_cast(cap));
		return m_cap_strings.cend() != iterator;
	}

	/**
		Get a capability string value.

		@note @a value is unmodified if the return value is @c false.

		@returns @c true if the capability was fetched and stored
		in @a value; @c false otherwise.
		@param cap Capability string to lookup.
		@param value Result value.
	*/
	bool
	cap_string(
		tty::CapString const cap,
		String& value
	) const noexcept {
		auto const it = m_cap_strings.find(enum_cast(cap));
		if (m_cap_strings.cend() != it) {
			value.assign(it->second);
			return true;
		}
		return false;
	}

	/**
		Get capability string map.
	*/
	cap_string_map_type const&
	cap_string_map() const noexcept {
		return m_cap_strings;
	}

	/**
		Get capability string count.
	*/
	std::size_t
	cap_string_count() const noexcept {
		return m_cap_strings.size();
	}
/// @}

/** @name Serialization */ /// @{
	/**
		Deserialize from stream.

		@note @a stream need not be seekable.

		@par
		@note Any stored properties are discarded before
		deserialization.

		@throws Error{ErrorCode::serialization_io_failed}
		If an IO operation fails.

		@throws Error{ErrorCode::serialization_data_malformed}
		If malformed data is encountered.

		@param stream Stream to read from.
	*/
	void
	deserialize(
		std::istream& stream
	);
/// @}
};

/** @} */ // end of doc-group tty

} // namespace tty
} // namespace Beard
