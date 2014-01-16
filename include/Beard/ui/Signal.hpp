/**
@file ui/Signal.hpp
@brief UI signal.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef BEARD_UI_SIGNAL_HPP_
#define BEARD_UI_SIGNAL_HPP_

#include <Beard/config.hpp>
#include <Beard/aux.hpp>
#include <Beard/ui/Defs.hpp>

#include <utility>

namespace Beard {
namespace ui {

// Forward declarations
template<
	class
>
class Signal;
class SignalList;

/**
	@addtogroup ui
	@{
*/

/**
	%Event signal.

	@tparam R Return type.
	@tparam ArgP Argument types.
*/
template<
	typename R,
	typename... ArgP
>
class Signal<R(ArgP...)> final {
public:
	/** Return type. */
	using return_type = R;

	/** Function type. */
	using function_type = aux::function<R(ArgP...)>;

private:
	function_type m_func;

	Signal(Signal const&) = delete;
	Signal& operator=(Signal const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Signal() noexcept = default;

	/** Default constructor. */
	Signal() noexcept = default;

	/**
		Constructor with function.

		@param func Function to bind.
	*/
	explicit
	Signal(
		function_type func
	) noexcept
		: m_func(std::move(func))
	{}

	/** Move constructor. */
	Signal(Signal&&) noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Signal& operator=(Signal&&) noexcept = default;
/// @}

/** @name Properties */ /// @{
	/**
		Check if the signal is bound.
	*/
	bool
	is_bound() const noexcept {
		return m_func.operator bool();
	}
/// @}

/** @name Operations */ /// @{
	/**
		Bind a function to the signal.
	*/
	void
	bind(
		function_type func
	) noexcept {
		m_func = std::move(func);
	}

	/**
		Unbind the signal.
	*/
	void
	unbind() noexcept {
		m_func = nullptr;
	}

	/**
		Execute the bound function.

		@note Has no effect if the signal is unbound.

		@returns The return value (if any) of the bound function.

		@tparam CArgP Argument types.
		@param args Arguments.
	*/
	template<
		typename... CArgP
	>
	return_type
	operator()(
		CArgP&&... args
	) {
		if (is_bound()) {
			return m_func(std::forward<CArgP>(args)...);
		} else {
			return return_type();
		}
	}
/// @}
};

/** @} */ // end of doc-group ui

} // namespace ui
} // namespace Beard

#endif // BEARD_UI_SIGNAL_HPP_
