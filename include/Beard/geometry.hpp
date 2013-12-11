/**
@file geometry.hpp
@brief Geometry primitives.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_GEOMETRY_HPP_
#define BEARD_GEOMETRY_HPP_

#include <Beard/config.hpp>
#include <Beard/utility.hpp>

namespace Beard {

// Forward declarations
enum class Axis : unsigned;
struct Vec2;
struct Rect;

/**
	@addtogroup geometry
	@{
*/

/**
	Axes.

	@note Axis::both and Axis::none are for special purposes.
	The geometric type operations (for Vec2, Rect, and Quad) only
	accept single, valid axes.
*/
enum class Axis : unsigned {
	/** X-axis. */
	x = 1u << 0,
	/** Y-axis. */
	y = 1u << 1,

	/** Alias for Axis::x. */
	horizontal = x,
	/** Alias for Axis::y. */
	vertical   = y,

	/** No axes. */
	none = 0u,
	/** Both axes. */
	both = x | y
};

/**
	Geometry element type.
*/
using geom_value_type = signed;

/**
	2-dimensional vector.
*/
struct Vec2 final {
	/**
		Horizontal dimension value (x-axis).
		@{
	*/
	union {
		//geom_value_type h;
		geom_value_type x;
		geom_value_type width;
	};
	/** @} */

	/**
		Vertical dimension value (y-axis).
		@{
	*/
	union {
		//geom_value_type v;
		geom_value_type y;
		geom_value_type height;
	};
	/** @} */
};

/**
	Rectangle.
*/
struct Rect final {
	/** Position of rectangle. */
	Vec2 pos;
	/** Size of rectangle. */
	Vec2 size;
};

/**
	Quad.
*/
struct Quad final {
	/** Top-left value. */
	Vec2 v1;
	/** Bottom-right value. */
	Vec2 v2;
};

/**
	Get an axis's transpose-axis.

	@note Axis::none and Axis::both are handled as transposes of
	eachother.

	@param axis %Axis.
*/
inline constexpr Axis
axis_transpose(
	Axis const axis
) noexcept {
	return
		static_cast<Axis>(enum_cast(axis) ^ 0x03)
	;
}

/**
	Get vector value by axis.

	@param v Vector.
	@param axis %Axis to get.
*/
inline constexpr geom_value_type
vec2_axis_value(
	Vec2 const& v,
	Axis const axis
) noexcept {
	return
		Axis::x == axis
		? v.x
		: v.y
	;
}

/**
	Get reference to vector value by axis.

	@param v Vector.
	@param axis %Axis to get.
*/
inline constexpr geom_value_type&
vec2_axis_ref(
	Vec2& v,
	Axis const axis
) noexcept {
	return
		Axis::x == axis
		? v.x
		: v.y
	;
}

/**
	Get the transpose of a vector.

	@param v Vector.
*/
inline constexpr Vec2
vec2_transpose(
	Vec2 const& v
) noexcept {
	return Vec2{
		v.y, v.x
	};
}

/**
	Get an axis-placed-first vector.

	@returns A copy of @a v where @a axis is in the X axis.
	@param v Vector.
	@param axis %Axis to place first.
*/
inline constexpr Vec2
vec2_axis_first(
	Vec2 const& v,
	Axis const axis
) noexcept {
	return
		Axis::x == axis
		? Vec2{v.x, v.y}
		: Vec2{v.y, v.x}
	;
}

/**
	Clamp a value to a range.

	@param value Value to clamp.
	@param min Minimum value.
	@param max Maximum value.
*/
inline geom_value_type
value_clamp(
	geom_value_type const value,
	geom_value_type const min,
	geom_value_type const max
) noexcept {
	return max_ce(min_ce(value, max), min);
}

/**
	Check if value is within a bound.

	@note This is non-max-inclusive: <code>[min; max)</code>.

	@param value Value to test.
	@param min Minimum value (inclusive).
	@param max Maximum value (exclusive).
*/
inline constexpr bool
value_in_bounds(
	geom_value_type const value,
	geom_value_type const min,
	geom_value_type const max
) noexcept {
	return min <= value && max > value;
}

/**
	Piecewise-clamp vector to a minimum value.

	@param v Vector to clamp.
	@param min Minimum value.
*/
inline Vec2&
vec2_clamp_min(
	Vec2& v,
	Vec2 const& min
) noexcept {
	v.x = max_ce(v.x, min.x);
	v.y = max_ce(v.y, min.y);
	return v;
}

/**
	Piecewise-clamp vector to a maximum value.

	@param v Vector to clamp.
	@param max Maximum value.
*/
inline Vec2&
vec2_clamp_max(
	Vec2& v,
	Vec2 const& max
) noexcept {
	v.x = min_ce(v.x, max.x);
	v.y = min_ce(v.y, max.y);
	return v;
}

/**
	Piecewise-clamp vector to a range.

	@param v Vector to clamp.
	@param min Minimum value.
	@param max Maximum value.
*/
inline Vec2&
vec2_clamp(
	Vec2& v,
	Vec2 const& min,
	Vec2 const& max
) noexcept {
	v.x = max_ce(min_ce(v.x, max.x), min.x);
	v.y = max_ce(min_ce(v.y, max.y), min.y);
	return v;
}

/**
	Axis-wise clamp vector to a minimum value.

	@param v Vector to clamp.
	@param min Minimum value.
	@param axis Axis to clamp.
*/
inline Vec2&
vec2_clamp_min(
	Vec2& v,
	Vec2 const& min,
	Axis const axis
) noexcept {
	auto& value = vec2_axis_ref(v, axis);
	value = max_ce(value, vec2_axis_value(min, axis));
	return v;
}

/**
	Axis-wise clamp vector to a maximum value.

	@param v Vector to clamp.
	@param max Maximum value.
	@param axis Axis to clamp.
*/
inline Vec2&
vec2_clamp_max(
	Vec2& v,
	Vec2 const& max,
	Axis const axis
) noexcept {
	auto& value = vec2_axis_ref(v, axis);
	value = min_ce(value, vec2_axis_value(max, axis));
	return v;
}

/**
	Axis-wise clamp vector to a range.

	@param v Vector to clamp.
	@param min Minimum value.
	@param max Maximum value.
	@param axis Axis to clamp.
*/
inline Vec2&
vec2_clamp(
	Vec2& v,
	Vec2 const& min,
	Vec2 const& max,
	Axis const axis
) noexcept {
	auto& value = vec2_axis_ref(v, axis);
	value = max_ce(
		min_ce(value, vec2_axis_value(max, axis)),
		vec2_axis_value(min, axis)
	);
	return v;
}

/**
	Check if vector is within a bound.

	@note This is non-max-inclusive: <code>[min; max)</code>.

	@param v Vector to test.
	@param min Minimum value (inclusive).
	@param max Maximum value (exclusive).
*/
inline constexpr bool
vec2_in_bounds(
	Vec2 const& v,
	Vec2 const& min,
	Vec2 const& max
) noexcept {
	return
		min.x <= v.x && max.x > v.x &&
		min.y <= v.y && max.y > v.y
	;
}

/**
	Check if vector is within a bound by axis.

	@note This is non-max-inclusive: <code>[min; max)</code>.

	@param v Vector to test.
	@param min Minimum value (inclusive).
	@param max Maximum value (exclusive).
	@param axis Axis to test.
*/
inline constexpr bool
vec2_in_bounds(
	Vec2 const& v,
	Vec2 const& min,
	Vec2 const& max,
	Axis const axis
) noexcept {
	return
		Axis::x == axis
		? min.x <= v.x && max.x > v.x
		: min.y <= v.y && max.y > v.y
	;
}

/**
	Check if vector is within a bound.

	@note This is non-size-inclusive:
	<code>[rect.pos; rect.pos + rect.size)</code>.

	@param v Vector to test.
	@param rect Bounds (size-exclusive).
*/
inline constexpr bool
vec2_in_bounds(
	Vec2 const& v,
	Rect const& rect
) noexcept {
	return
		rect.pos.x <= v.x && rect.pos.x + rect.size.width  > v.x &&
		rect.pos.y <= v.y && rect.pos.y + rect.size.height > v.y
	;
}

/**
	Construct absolute-position quad from rectangle.

	@param rect Rectangle.
*/
inline constexpr Quad
rect_abs_quad(
	Rect const& rect
) noexcept {
	return Quad{
		{rect.pos.x, rect.pos.y},
		{rect.pos.x + rect.size.width, rect.pos.y + rect.size.height}
	};
}

/**
	Check if a rectangle intersects with another rectangle.

	@param a,b Rectangles to test.
*/
inline bool
rect_intersects(
	Rect const& a,
	Rect const& b
) noexcept {
	geom_value_type const
		a_x2 = a.pos.x + a.size.width,
		a_y2 = a.pos.y + a.size.height,
		b_x2 = b.pos.x + b.size.width,
		b_y2 = b.pos.y + b.size.height
	;
	return !(
		b.pos.x > a_x2 ||
		a.pos.x > b_x2 ||
		b.pos.y > a_y2 ||
		a.pos.y > b_y2
	);
}

/**
	Construct rectangle from quad.

	@note The resultant rectangle can be degenerate -- i.e., its size
	might have negative values.

	@param quad %Quad.
*/
inline constexpr Rect
quad_rect(
	Quad const& quad
) noexcept {
	return Rect{
		{quad.v1.x, quad.v1.y},
		{quad.v2.x - quad.v1.x, quad.v2.y - quad.v1.y}
	};
}

/**
	Check if a quad intersects with another quad.

	@param a,b Quads to test.
*/
inline constexpr bool
quad_intersects(
	Quad const& a,
	Quad const& b
) noexcept {
	return !(
		a.v2.x < b.v1.x ||
		a.v1.x > b.v2.x ||
		a.v2.y < b.v1.y ||
		a.v1.y > b.v2.y
	);
}

/** @name Operators */ /// @{

/**
	Vector equality comparison operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline constexpr bool
operator==(
	Vec2 const& lhs,
	Vec2 const& rhs
) noexcept {
	return
		lhs.x == rhs.x &&
		lhs.y == rhs.y
	;
}

/**
	Vector addition operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline constexpr Vec2
operator+(
	Vec2 const& lhs,
	Vec2 const& rhs
) noexcept {
	return Vec2{
		lhs.x + rhs.x,
		lhs.y + rhs.y
	};
}

/**
	Vector addition assignment operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline /*constexpr*/ Vec2&
operator+=(
	Vec2& lhs,
	Vec2 const& rhs
) noexcept {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

/**
	Vector subtraction operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline constexpr Vec2
operator-(
	Vec2 const& lhs,
	Vec2 const& rhs
) noexcept {
	return Vec2{
		lhs.x - rhs.x,
		lhs.y - rhs.y
	};
}

/**
	Vector subtraction assignment operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline /*constexpr*/ Vec2&
operator-=(
	Vec2& lhs,
	Vec2 const& rhs
) noexcept {
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

/**
	Vector multiplication operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline constexpr Vec2
operator*(
	Vec2 const& lhs,
	Vec2 const& rhs
) noexcept {
	return Vec2{
		lhs.x * rhs.x,
		lhs.y * rhs.y
	};
}

/**
	Vector multiplication assignment operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline /*constexpr*/ Vec2&
operator*=(
	Vec2& lhs,
	Vec2 const& rhs
) noexcept {
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	return lhs;
}

/**
	Vector division operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline constexpr Vec2
operator/(
	Vec2 const& lhs,
	Vec2 const& rhs
) noexcept {
	return Vec2{
		lhs.x / rhs.x,
		lhs.y / rhs.y
	};
}

/**
	Vector division assignment operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline /*constexpr*/ Vec2&
operator/=(
	Vec2& lhs,
	Vec2 const& rhs
) noexcept {
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	return lhs;
}

/**
	Rectangle equality comparison operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline constexpr bool
operator==(
	Rect const& lhs,
	Rect const& rhs
) noexcept {
	return
		lhs.pos == rhs.pos &&
		lhs.size == rhs.size
	;
}

/**
	%Quad equality comparison operator.

	@param lhs Left-hand side.
	@param rhs Right-hand side.
*/
inline constexpr bool
operator==(
	Quad const& lhs,
	Quad const& rhs
) noexcept {
	return
		lhs.v1 == rhs.v1 &&
		lhs.v2 == rhs.v2
	;
}

/// @} // end of name-group Operators

/** @} */ // end of doc-group geometry

} // namespace Beard

#endif // BEARD_GEOMETRY_HPP_
