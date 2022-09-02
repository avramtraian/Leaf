// Copyright to Avram Traian. 2022 - 2022.
// File created on August 24 2022.

#pragma once

#include "Math.h"

#include <cmath>
#include <cstdlib>

namespace Leaf {

	namespace Mathf {

		float SquareRoot(float x)
		{
			return sqrtf(x);
		}

		double SquareRootD(double x)
		{
			return sqrt(x);
		}

		float Power(float base, float exp)
		{
			return powf(base, exp);
		}

		float Power(float base, int64 exp)
		{
			float result = 1.0f;

			for (int64 index = 0; index < exp; index++)
				result *= base;

			if (exp < 0)
			{
				float one_over_base = 1.0f / base;
				for (int64 index = exp; index > 0; index--)
					result *= one_over_base;
			}

			return result;
		}

		float Sin(float x)
		{
			return sinf(x);
		}

		float Asin(float x)
		{
			return asinf(x);
		}

		float Cos(float x)
		{
			return cosf(x);
		}

		float Acos(float x)
		{
			return acosf(x);
		}

		float Tan(float x)
		{
			return tanf(x);
		}

		float Atan(float x)
		{
			return atanf(x);
		}

	}

	namespace Mathd {

		double SquareRoot(double x)
		{
			return sqrt(x);
		}

		double Power(double base, double exp)
		{
			return pow(base, exp);
		}

		double Power(double base, int64 exp)
		{
			double result = 1.0f;

			for (int64 index = 0; index < exp; index++)
				result *= base;

			if (exp < 0)
			{
				double one_over_base = 1.0f / base;
				for (int64 index = exp; index > 0; index--)
					result *= one_over_base;
			}

			return result;
		}

		double Sin(double x)
		{
			return sin(x);
		}

		double Asin(double x)
		{
			return asin(x);
		}

		double Cos(double x)
		{
			return cos(x);
		}

		double Acos(double x)
		{
			return acos(x);
		}

		double Tan(double x)
		{
			return tan(x);
		}

		double Atan(double x)
		{
			return atan(x);
		}

	}

}