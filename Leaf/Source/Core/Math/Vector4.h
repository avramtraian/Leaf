// Copyright to Avram Traian. 2022 - 2022.
// File created on September 2 2022.

#pragma once

#include "Math.h"

namespace Leaf {

	template<typename ValueType, typename FloatType>
	struct Vector4
	{
	public:
		ValueType X, Y, Z, W;

	public:
		static LF_INLINE Vector4 Add(const Vector4& v1, const Vector4& v2)
		{
			return Vector4(
				v1.X + v2.X,
				v1.Y + v2.Y,
				v1.Z + v2.Z,
				v1.W + v2.W
			);
		}

		static LF_INLINE Vector4 Substract(const Vector4& v1, const Vector4& v2)
		{
			return Vector4(
				v1.X - v2.X,
				v1.Y - v2.Y,
				v1.Z - v2.Z,
				v1.W - v2.W
			);
		}

		static LF_INLINE Vector4 Multiply(const Vector4& v, ValueType s)
		{
			return Vector4(
				v.X * s,
				v.Y * s,
				v.Z * s,
				v.W * s
			);
		}

		static LF_INLINE Vector4 Multiply(const Vector4& v1, const Vector4& v2)
		{
			return Vector4(
				v1.X * v2.X,
				v1.Y * v2.Y,
				v1.Z * v2.Z,
				v1.W * v2.W
			);
		}

		static LF_INLINE Vector4 Divide(const Vector4& v, ValueType s)
		{
			FloatType one_over_s = (FloatType)1 / (FloatType)s;
			return Vector4(
				(ValueType)((FloatType)v.X * one_over_s),
				(ValueType)((FloatType)v.Y * one_over_s),
				(ValueType)((FloatType)v.Z * one_over_s),
				(ValueType)((FloatType)v.W * one_over_s)
			);
		}

		static LF_INLINE Vector4 Divide(const Vector4& v1, const Vector4& v2)
		{
			return Vector4(
				v1.X / v2.X,
				v1.Y / v2.Y,
				v1.Z / v2.Z,
				v1.W / v2.W
			);
		}

	public:
		Vector4()
			: X(0)
			, Y(0)
			, Z(0)
			, W(0)
		{}

		Vector4(const Vector4& other)
			: X(other.X)
			, Y(other.Y)
			, Z(other.Z)
			, W(other.W)
		{}

		Vector4(ValueType x, ValueType y, ValueType z, ValueType w)
			: X(x)
			, Y(y)
			, Z(z)
			, W(w)
		{}

		Vector4(ValueType scalar)
			: X(scalar)
			, Y(scalar)
			, Z(scalar)
			, W(scalar)
		{}

	public:
		Vector4& operator=(const Vector4& other)
		{
			X = other.X;
			Y = other.Y;
			Z = other.Z;
			W = other.W;
			return *this;
		}

		Vector4& operator+=(const Vector4& other)
		{
			X += other.X;
			Y += other.Y;
			Z += other.Z;
			W += other.W;
			return *this;
		}

		Vector4& operator-=(const Vector4& other)
		{
			X -= other.X;
			Y -= other.Y;
			Z -= other.Z;
			W -= other.W;
			return *this;
		}

		Vector4& operator*=(ValueType scalar)
		{
			X *= scalar;
			Y *= scalar;
			Z *= scalar;
			W *= scalar;
			return *this;
		}

		Vector4& operator*=(const Vector4& other)
		{
			X *= other.X;
			Y *= other.Y;
			Z *= other.Z;
			W *= other.W;
			return *this;
		}

		Vector4& operator/=(ValueType scalar)
		{
			FloatType one_over_scalar = (FloatType)1 / (FloatType)scalar;
			X = (ValueType)((FloatType)X * one_over_scalar);
			Y = (ValueType)((FloatType)Y * one_over_scalar);
			Z = (ValueType)((FloatType)Z * one_over_scalar);
			W = (ValueType)((FloatType)W * one_over_scalar);
			return *this;
		}

		Vector4& operator/=(const Vector4& other)
		{
			X /= other.X;
			Y /= other.Y;
			Z /= other.Z;
			W /= other.W;
			return *this;
		}
	};

	template<typename ValueType, typename FloatType>
	Vector4<ValueType, FloatType> operator+(const Vector4<ValueType, FloatType>& v1, const Vector4<ValueType, FloatType>& v2)
	{
		return Vector4<ValueType, FloatType>::Add(v1, v2);
	}

	template<typename ValueType, typename FloatType>
	Vector4<ValueType, FloatType> operator-(const Vector4<ValueType, FloatType>& v1, const Vector4<ValueType, FloatType>& v2)
	{
		return Vector4<ValueType, FloatType>::Substract(v1, v2);
	}

	template<typename ValueType, typename FloatType>
	Vector4<ValueType, FloatType> operator*(const Vector4<ValueType, FloatType>& v, ValueType s)
	{
		return Vector4<ValueType, FloatType>::Multiply(v, s);
	}

	template<typename ValueType, typename FloatType>
	Vector4<ValueType, FloatType> operator*(ValueType s, const Vector4 <ValueType, FloatType>& v)
	{
		return Vector4<ValueType, FloatType>::Multiply(v, s);
	}

	template<typename ValueType, typename FloatType>
	Vector4<ValueType, FloatType> operator*(const Vector4<ValueType, FloatType>& v1, const Vector4<ValueType, FloatType>& v2)
	{
		return Vector4<ValueType, FloatType>::Multiply(v1, v2);
	}

	template<typename ValueType, typename FloatType>
	Vector4<ValueType, FloatType> operator/(const Vector4<ValueType, FloatType>& v, ValueType s)
	{
		return Vector4<ValueType, FloatType>::Divide(v, s);
	}

	template<typename ValueType, typename FloatType>
	Vector4<ValueType, FloatType> operator/(const Vector4<ValueType, FloatType>& v1, const Vector4<ValueType, FloatType>& v2)
	{
		return Vector4<ValueType, FloatType>::Divide(v1, v2);
	}

	using Vector4f = Vector4<float, float>;
	using Vector4f = Vector4<double, double>;
	using Vector4i = Vector4<int32, float>;
	using Vector4u = Vector<uint32, float>;

}