#ifndef RAGE_MATH_HPP_
#define RAGE_MATH_HPP_

#include <algorithm>

namespace Rage
{
	/** @brief Have PI be defined cleanly. */
	float constexpr PI = 3.141592653589793f;
	double constexpr D_PI = 3.1415926535897932384626433832795;
	double constexpr D_PI_REC = 1.0 / D_PI;

	/** @brief Convert from radians to degrees. */
	template<typename T>
	T constexpr RadiansToDegrees(T radians)
	{
		return radians * (180.f / PI);
	}

	/** @brief Convert from degrees to radians. */
	template<typename T>
	T constexpr DegreesToRadians(T degrees)
	{
		return degrees * (PI / 180.f);
	}
	
	/** @brief Calculate the sin of a number quickly. */
	float FastSin( float x );
	
	/** @brief Claculate the cosine of a number quickly. */
	float FastCos( float x );

	/** @brief Bring a value within range. */
	template<typename T>
	T constexpr clamp( T const & val, T const & low, T const & high )
	{
		return std::max(low, std::min(val, high));
	}

	/** @brief Interpolate within the ranges and interlopant. */
	template<typename T, typename U>
	U constexpr lerp( T x, U l, U h )
	{
		return static_cast<U>( (h - l) * x + l );
	}

	/** @brief Scale the target number so that the two targets match.
	 *
	 * This does not modify x, so it MUST assign the result to something!
	 * Do the multiply before the divide so that integer scales have more precision.
	 *
	 * One such example: scale(x, 0, 1, L, H); interpolate between L and H.
	 */
	template<typename T>
	T constexpr scale(T x, T l1, T h1, T l2, T h2)
	{
		return
			( l1 == 0 && h1 == 1 ) ?
			lerp(x, l2, h2) :
			(x - l1) * (h2 - l2) / (h1 - l1) + l2;
	}
}

#endif
