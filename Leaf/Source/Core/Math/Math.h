// Copyright to Avram Traian. 2022 - 2022.
// File created on August 24 2022.

#include "Core/CoreTypes.h"

namespace Leaf {

	/**
	* The namespace that stores the core templated math types & functions.
	*/
	namespace Math {

		enum class Axis : uint8
		{
			X = 0, Y = 1, Z = 2
		};

		/**
		* Checks if two variables are equal. Usage in floating point types.
		*
		* @param x The first variable to compare.
		* @param y The second variable to compare.
		* @param error_tolerance Maximum allowed (absolute) difference between the values. Must be a positive number.
		*
		* @returns True if the are nearly equal (might not have the same binary representation); False otherwise.
		*/
		template<typename T>
		LF_INLINE bool constexpr IsNearlyEqual(T x, T y, T error_tolerance)
		{
			T d = x - y;
			return -error_tolerance <= d && d <= error_tolerance;
		}

		/**
		* Clamps a value between two other values. Value is clamped in the interval [min, max].
		*
		* @param value The value to be clamped.
		* @param min_val The minimum allowed value.
		* @param max_val The maximum allowed value.
		*
		* @returns The clamped value. In the range [min_value, max_value].
		*/
		template<typename T>
		LF_INLINE T constexpr Clamp(T value, T min_val, T max_val)
		{
			return (value < min_val) ? min_val : ((value > max_val) ? max_val : value);
		}

		/**
		* Returns the smallest value between the two supplied variables.
		*
		* @param x First value.
		* @param y Second value.
		*
		* @returns The first value if x < y; y otherwise.
		*/
		template<typename T>
		LF_INLINE T constexpr Min(T x, T y)
		{
			return x < y ? x : y;
		}

		/**
		* Returns the biggest value between the two supplied variables.
		*
		* @param x First value.
		* @param y Second value.
		*
		* @returns The first value if x > y; y otherwise.
		*/
		template<typename T>
		LF_INLINE T constexpr Max(T x, T y)
		{
			return x > y ? x : y;
		}

		/**
		* Returns the absolute value (|x|).
		*
		* @param x The value.
		*
		* @returns The absolute value of x (x if x >= 0; -x otherwise).
		*/
		template<typename T>
		LF_INLINE T constexpr Absolute(T x)
		{
			return x < (T)0 ? -x : x;
		}

	}

	/**
	* The namespace that stores the core float values & functions.
	*/
	namespace Mathf {

		/**
		* PI constants.
		*/
		constexpr float PI                  = 3.1415926535897932f;
		constexpr float TWO_PI              = 2.0f * PI;
		constexpr float THREE_PI            = 3.0f * PI;
		constexpr float HALF_PI             = PI / 2.0f;
		constexpr float THIRD_PI            = PI / 3.0f;
		constexpr float QUARTER_PI          = PI / 4.0f;

		constexpr float ONE_OVER_PI         = 1.0f / PI;
		constexpr float ONE_OVER_TWO_PI     = 1.0f / TWO_PI;
		constexpr float ONE_OVER_THREE_PI   = 1.0f / THREE_PI;

		/**
		* Common square roots constants.
		*/
		constexpr float SQRT_TWO            = 1.4142135623730950f;
		constexpr float SQRT_THREE          = 1.7320508075688772f;
		constexpr float SQRT_FIVE           = 2.2360679774997896f;

		constexpr float ONE_OVER_SQRT_TWO   = 1.0f / SQRT_TWO;
		constexpr float ONE_OVER_SQRT_THREE = 1.0f / SQRT_THREE;
		constexpr float ONE_OVER_SQRT_FIVE  = 1.0f / SQRT_FIVE;

		/**
		* Radians & Degrees multipliers.
		*/
		constexpr float DEG_TO_RAD          = PI / 180.0f;
		constexpr float RAD_TO_DEG          = 180.0f / PI;

		/**
		* A number that is much larger than any valid number used in the engine.
		*/
		constexpr float LF_INFINITY         = 1e35f;

		//// Basic math functions.

		LEAF_API float SquareRoot(float x);

		LEAF_API float Power(float base, float exp);
		LEAF_API float Power(float base, int64 exp);

		LEAF_API float Sin(float x);
		LEAF_API float Asin(float x);

		LEAF_API float Cos(float x);
		LEAF_API float Acos(float x);

		LEAF_API float Tan(float x);
		LEAF_API float Atan(float x);

		LF_INLINE constexpr float Degrees(float rad)
		{
			return rad * RAD_TO_DEG;
		}

		LF_INLINE constexpr float Radians(float deg)
		{
			return deg * DEG_TO_RAD;
		}

		// See Math::IsNearlyEqual.
		LF_INLINE constexpr bool IsNearlyEqual(float x, float y, float error_tolerance)
		{
			return Math::IsNearlyEqual<float>(x, y, error_tolerance);
		}

		// See Math::Clamp.
		LF_INLINE float constexpr Clamp(float value, float min_val, float max_val)
		{
			return Math::Clamp<float>(value, min_val, max_val);
		}

		// See Math::Min.
		LF_INLINE float constexpr Min(float x, float y)
		{
			return Math::Min<float>(x, y);
		}

		// See Math::Max.
		LF_INLINE float constexpr Max(float x, float y)
		{
			return Math::Max<float>(x, y);
		}

		// See Math::Absolute.
		LF_INLINE float constexpr Absolute(float x)
		{
			return Math::Absolute<float>(x);
		}

	}

	/**
	* The namespace that stores the core double values & functions.
	*/
	namespace Mathd {

		/**
		* PI constants.
		*/
		constexpr double PI                  = 3.1415926535897932;
		constexpr double TWO_PI              = 2.0 * PI;
		constexpr double THREE_PI            = 3.0 * PI;
		constexpr double HALF_PI             = PI / 2.0;
		constexpr double THIRD_PI            = PI / 3.0;
		constexpr double QUARTER_PI          = PI / 4.0;

		constexpr double ONE_OVER_PI         = 1.0 / PI;
		constexpr double ONE_OVER_TWO_PI     = 1.0 / TWO_PI;
		constexpr double ONE_OVER_THREE_PI   = 1.0 / THREE_PI;

		/**
		* Common square roots constants.
		*/
		constexpr double SQRT_TWO            = 1.4142135623730950;
		constexpr double SQRT_THREE          = 1.7320508075688772;
		constexpr double SQRT_FIVE           = 2.2360679774997896;

		constexpr double ONE_OVER_SQRT_TWO   = 1.0 / SQRT_TWO;
		constexpr double ONE_OVER_SQRT_THREE = 1.0 / SQRT_THREE;
		constexpr double ONE_OVER_SQRT_FIVE  = 1.0 / SQRT_FIVE;

		/**
		* Radians & Degrees multipliers.
		*/
		constexpr double DEG_TO_RAD          = PI / 180.0;
		constexpr double RAD_TO_DEG          = 180.0 / PI;

		/**
		* A number that is much larger than any valid number used in the engine.
		*/
		constexpr double LF_INFINITY         = 1e300;

		//// Basic math functions.

		LEAF_API double SquareRoot(double x);

		LEAF_API double Power(double base, double exp);
		LEAF_API double Power(double base, int64 exp);

		LEAF_API double Sin(double x);
		LEAF_API double Asin(double x);

		LEAF_API double Cos(double x);
		LEAF_API double Acos(double x);

		LEAF_API double Tan(double x);
		LEAF_API double Atan(double x);

		LF_INLINE constexpr double Degrees(double rad)
		{
			return rad * RAD_TO_DEG;
		}

		LF_INLINE constexpr double Radians(double deg)
		{
			return deg * DEG_TO_RAD;
		}

		// See Math::IsNearlyEqual.
		LF_INLINE constexpr bool IsNearlyEqual(double x, double y, double error_tolerance)
		{
			return Math::IsNearlyEqual<double>(x, y, error_tolerance);
		}

		// See Math::Clamp.
		LF_INLINE double constexpr Clamp(double value, double min_val, double max_val)
		{
			return Math::Clamp<double>(value, min_val, max_val);
		}

		// See Math::Min.
		LF_INLINE double constexpr Min(double x, double y)
		{
			return Math::Min<double>(x, y);
		}

		// See Math::Max.
		LF_INLINE double constexpr Max(double x, double y)
		{
			return Math::Max<double>(x, y);
		}

		// See Math::Absolute.
		LF_INLINE double constexpr Absolute(double x)
		{
			return Math::Absolute<double>(x);
		}

	}

}