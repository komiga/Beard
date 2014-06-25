/**
@file ui/Geom.hpp
@brief %Widget geometry.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Beard/config.hpp>
#include <Beard/utility.hpp>
#include <Beard/geometry.hpp>

#include <duct/StateStore.hpp>

namespace Beard {
namespace ui {

// Forward declarations
struct Geom;

/**
	@addtogroup ui
	@{
*/

/**
	%Widget geometry.
*/
struct Geom final {
private:
	enum : unsigned {
		// NB: Axis::both takes two bits, Flags is 8 bits;
		// occupying last four bits so that we don't have to update
		// this unless Flags bitlength changes.
		expand_shift	= 4u,
		fill_shift		= expand_shift + 2u
	};

	enum class Flags : std::uint8_t {
		none		= 0u,
		static_size	= bit(0u),
		expand_mask	= enum_cast(Axis::both) << expand_shift,
		fill_mask	= enum_cast(Axis::both) << fill_shift
	};

	Vec2 m_request_size;
	Rect m_area;
	Rect m_frame;
	duct::StateStore<Flags> m_flags;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Geom() noexcept = default;

	/** Default constructor. */
	Geom() noexcept
		: m_request_size()
		, m_area()
		, m_frame()
		, m_flags()
	{}

	/**
		Constructor with request size and modes.

		@param request_size Natural size.
		@param static_size Static size mode.
		@param expand Expand axes.
		@param fill Fill axes.
	*/
	Geom(
		Vec2 request_size,
		bool const static_size = false,
		Axis const expand = Axis::none,
		Axis const fill = Axis::none
	)
		: m_request_size(std::move(request_size))
		, m_area()
		, m_frame()
		, m_flags(
			static_size ? Flags::static_size : Flags::none,
			static_cast<Flags>(enum_cast(expand) << expand_shift),
			static_cast<Flags>(enum_cast(fill) << fill_shift)
		)
	{}

	/**
		Constructor with sizing.

		@param static_size Static size mode.
		@param expand Expand axes.
		@param fill Fill axes.
	*/
	Geom(
		bool const static_size,
		Axis const expand = Axis::none,
		Axis const fill = Axis::none
	)
		: Geom(Vec2(), static_size, expand, fill)
	{}

	/** Copy constructor. */
	Geom(Geom const&) noexcept = default;
	/** Move constructor. */
	Geom(Geom&&) noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	Geom& operator=(Geom const&) noexcept = default;
	/** Move assignment operator. */
	Geom& operator=(Geom&&) noexcept = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set request size.

		@param request_size New request size.
	*/
	void
	set_request_size(
		Vec2 request_size
	) noexcept {
		m_request_size = std::move(request_size);
	}

	/**
		Get request size (mutable).
	*/
	Vec2&
	get_request_size() noexcept {
		return m_request_size;
	}

	/**
		Get request size.
	*/
	Vec2 const&
	get_request_size() const noexcept {
		return m_request_size;
	}

	/**
		Set area.

		@param area New area.
	*/
	void
	set_area(
		Rect area
	) noexcept {
		m_area = std::move(area);
	}

	/**
		Get area (mutable).
	*/
	Rect&
	get_area() noexcept {
		return m_area;
	}

	/**
		Get area.
	*/
	Rect const&
	get_area() const noexcept {
		return m_area;
	}

	/**
		Set frame.

		@param frame New frame.
	*/
	void
	set_frame(
		Rect frame
	) noexcept {
		m_frame = std::move(frame);
	}

	/**
		Get frame (mutable).
	*/
	Rect&
	get_frame() noexcept {
		return m_frame;
	}

	/**
		Get frame.
	*/
	Rect const&
	get_frame() const noexcept {
		return m_frame;
	}

	/**
		Set sizing axes.

		@param expand Expand axes.
		@param fill Fill axes.
	*/
	void
	set_sizing(
		Axis const expand,
		Axis const fill
	) noexcept {
		set_expand(expand);
		set_fill(fill);
	}

	/**
		Enable or disable static request size.

		@param enable Whether to enable or disable static-size
		mode.
	*/
	void
	set_static(
		bool const enable
	) noexcept {
		m_flags.set(Flags::static_size, enable);
	}

	/**
		Get static-size mode.
	*/
	bool
	is_static() const noexcept {
		return m_flags.test(Flags::static_size);
	}

	/**
		Set expand axes.

		@param axes Expand axes.
	*/
	void
	set_expand(
		Axis const axes
	) noexcept {
		m_flags.set_masked(
			Flags::expand_mask,
			static_cast<Flags>(enum_cast(axes) << expand_shift)
		);
	}

	/**
		Get expand axes.
	*/
	Axis
	get_expand() const noexcept {
		return static_cast<Axis>(
			enum_cast(m_flags.get_states(Flags::expand_mask)) >> expand_shift
		);
	}

	/**
		Test whether the geometry expands along any specified axes.

		@param axes Axes to test.
		@param equal Whether the axes should match exactly.
	*/
	bool
	expands(
		Axis const axes,
		bool const equal = false
	) const noexcept {
		return
			equal
			? axes == get_expand()
			: enum_cast(axes) & enum_cast(get_expand())
		;
	}

	/**
		Set fill axes.

		@param axes Fill axes.
	*/
	void
	set_fill(
		Axis const axes
	) noexcept {
		m_flags.set_masked(
			Flags::fill_mask,
			static_cast<Flags>(enum_cast(axes) << fill_shift)
		);
	}

	/**
		Get fill axes.
	*/
	Axis
	get_fill() const noexcept {
		return static_cast<Axis>(
			enum_cast(m_flags.get_states(Flags::fill_mask)) >> fill_shift
		);
	}

	/**
		Test whether the geometry fills expand area along any
		specified axes.

		@param axes Axes to test.
		@param equal Whether the fill axes should match exactly.
	*/
	bool
	fills(
		Axis const axes,
		bool const equal = false
	) const noexcept {
		return
			equal
			? axes == get_fill()
			: enum_cast(axes) & enum_cast(get_fill())
		;
	}

	/**
		Check whether a reflow could cause margins around any
		specified axes.

		@param axes Axes to test.
		@param equal Whether the axes should match exactly.
	*/
	bool
	margined(
		Axis const axes,
		bool const equal = false
	) const noexcept {
		return expands(axes, equal) && !fills(axes, equal);
	}

	/**
		Check whether a reflow will expand and fill extra area along
		any specified axes.

		@param axes Axes to test.
		@param equal Whether the axes should match exactly.
	*/
	bool
	expands_and_fills(
		Axis const axes,
		bool const equal = false
	) const noexcept {
		return expands(axes, equal) && fills(axes, equal);
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

