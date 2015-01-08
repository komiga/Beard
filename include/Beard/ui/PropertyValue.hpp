/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Property value class.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/ui/Defs.hpp>

#include <utility>

namespace Beard {
namespace ui {

// Forward declarations
class PropertyValue;

/**
	@addtogroup ui
	@{
*/

/**
	%Property value.
*/
class PropertyValue final {
private:
	ui::PropertyType m_type;

	union {
		ui::property_number_type m_number;
		ui::property_attr_type m_attr;
		ui::property_boolean_type m_boolean;
	};
	ui::property_string_type m_string;

	PropertyValue() = delete;

	void
	set_type(
		ui::PropertyType const type
	) noexcept {
		if (type != m_type && ui::PropertyType::string == m_type) {
			m_string.clear();
			m_string.shrink_to_fit();
		}
		m_type = type;
	}

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~PropertyValue() noexcept = default;

	/**
		Constructor with signed number.

		@param value Value.
	*/
	PropertyValue(
		ui::property_number_type const value
	) noexcept
		: m_type(PropertyType::number)
		, m_number(value)
		, m_string()
	{}

	/**
		Constructor with attribute.

		@param value Value.
	*/
	PropertyValue(
		ui::property_attr_type const value
	) noexcept
		: m_type(PropertyType::attr)
		, m_attr(value)
		, m_string()
	{}

	/**
		Constructor with boolean.

		@param value Value.
	*/
	PropertyValue(
		ui::property_boolean_type const value
	) noexcept
		: m_type(PropertyType::boolean)
		, m_boolean(value)
		, m_string()
	{}

	/**
		Constructor with string.

		@param value Value.
	*/
	explicit
	PropertyValue(
		ui::property_string_type value
	) noexcept
		: m_type(PropertyType::string)
		, m_number()
		, m_string(std::move(value))
	{}

	/** Copy constructor. */
	PropertyValue(PropertyValue const&) = default;
	/** Move constructor. */
	PropertyValue(PropertyValue&&) noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	PropertyValue& operator=(PropertyValue const&) = default;
	/** Move assignment operator. */
	PropertyValue& operator=(PropertyValue&&) noexcept = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get type.
	*/
	ui::PropertyType
	get_type() const noexcept {
		return m_type;
	}

	/**
		Check type.

		@returns <code>type == get_type()</code>.
		@param type Type to compare with.
	*/
	bool
	is_type(
		ui::PropertyType const type
	) const noexcept {
		return type == m_type;
	}

	/**
		Assign to number.

		@param value Value.
	*/
	void
	set_number(
		ui::property_number_type const value
	) noexcept {
		set_type(ui::PropertyType::number);
		m_number = value;
	}

	/**
		Get number value.
	*/
	ui::property_number_type
	get_number() const noexcept {
		return m_number;
	}

	/**
		Assign to attribute.

		@param value Value.
	*/
	void
	set_attr(
		ui::property_attr_type const value
	) noexcept {
		set_type(ui::PropertyType::attr);
		m_attr = value;
	}

	/**
		Get attribute value.
	*/
	ui::property_attr_type
	get_attr() const noexcept {
		return m_attr;
	}

	/**
		Assign to boolean.

		@param value Value.
	*/
	void
	set_boolean(
		ui::property_boolean_type const value
	) noexcept {
		set_type(ui::PropertyType::boolean);
		m_boolean = value;
	}

	/**
		Get boolean value.
	*/
	ui::property_boolean_type
	get_boolean() const noexcept {
		return m_boolean;
	}

	/**
		Assign to string.

		@param value Value.
	*/
	void
	set_string(
		ui::property_string_type value
	) noexcept {
		set_type(ui::PropertyType::string);
		m_string.assign(std::move(value));
	}

	/**
		Get string value.
	*/
	ui::property_string_type const&
	get_string() const noexcept {
		return m_string;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Assign to number.

		@param value Value.
	*/
	void
	assign(
		ui::property_number_type const value
	) noexcept {
		set_number(value);
	}

	/**
		Assign to attribute.

		@param value Value.
	*/
	void
	assign(
		ui::property_attr_type const value
	) noexcept {
		set_attr(value);
	}

	/**
		Assign to boolean.

		@param value Value.
	*/
	void
	assign(
		ui::property_boolean_type const value
	) noexcept {
		set_boolean(value);
	}

	/**
		Assign to string.

		@param value Value.
	*/
	void
	assign(
		ui::property_string_type value
	) noexcept {
		set_string(std::move(value));
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard
