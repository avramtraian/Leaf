// Copyright to Avram Traian. 2022 - 2022.
// File created on August 24 2022.

#pragma once

#include "Math.h"

namespace Leaf {

	template<typename ValueType, typename FloatType>
	struct Vector3
	{
	public:
		ValueType X, Y, Z;

	public:
		static LF_INLINE ValueType LengthSquared(const Vector3& v)
		{
			return (v.X * v.X) + (v.Y * v.Y) + (v.Z * v.Z);
		}

		static LF_INLINE FloatType Length(const Vector3& v)
		{
			return (FloatType)Mathd::SquareRoot((double)Vector3::LengthSquared(v));
		}

		static ValueType DistanceSquared(const Vector3& v1, const Vector3& v2)
		{
			ValueType x = v1.X - v2.X;
			ValueType y = v1.Y - v2.Y;
			ValueType z = v1.Z - v2.Z;
			return (x * x) + (y * y) + (z * z);
		}

		static FloatType Distance(const Vector3& v1, const Vector3& v2)
		{
			return (FloatType)Mathd::SquareRoot((double)Vector3::DistanceSquared(v1, v2));
		}

		static LF_INLINE Vector3 Normalize(const Vector3& v)
		{
			FloatType one_over_length = (FloatType)1 / Vector3::Length(v);
			return Vector3(
				(ValueType)((FloatType)v.X * one_over_length),
				(ValueType)((FloatType)v.Y * one_over_length),
				(ValueType)((FloatType)v.Z * one_over_length)
			);
		}

		static LF_INLINE ValueType Dot(const Vector3& v1, const Vector3& v2)
		{
			return (v1.X * v2.X) + (v1.Y * v2.Y) + (v1.Z * v2.Z);
		}

		static LF_INLINE Vector3 Cross(const Vector3& v1, const Vector3& v2)
		{
			return Vector3(
				v1.Y * v2.Z - v1.Z * v2.Y,
				v1.Z * v2.X - v1.X * v2.Z,
				v1.X * v2.Y - v1.Y * v2.X
			);
		}

	public:
		static LF_INLINE Vector3 Add(const Vector3& v1, const Vector3& v2)
		{
			return Vector3(
				v1.X + v2.X,
				v1.Y + v2.Y,
				v1.Z + v2.Z
			);
		}

		static LF_INLINE Vector3 Substract(const Vector3& v1, const Vector3& v2)
		{
			return Vector3(
				v1.X - v2.X,
				v1.Y - v2.Y,
				v1.Z - v2.Z
			);
		}

		static LF_INLINE Vector3 Multiply(const Vector3& v, ValueType s)
		{
			return Vector3(
				v.X * s,
				v.Y * s,
				v.Z * s
			);
		}

		static LF_INLINE Vector3 Multiply(const Vector3& v1, const Vector3& v2)
		{
			return Vector3(
				v1.X * v2.X,
				v1.Y * v2.Y,
				v1.Z * v2.Z
			);
		}

		static LF_INLINE Vector3 Divide(const Vector3& v, ValueType s)
		{
			FloatType one_over_s = (FloatType)1 / (FloatType)s;
			return Vector3(
				(ValueType)((FloatType)v.X * one_over_s),
				(ValueType)((FloatType)v.Y * one_over_s),
				(ValueType)((FloatType)v.Z * one_over_s)
			);
		}

		static LF_INLINE Vector3 Divide(const Vector3& v1, const Vector3& v2)
		{
			return Vector3(
				v1.X / v2.X,
				v1.Y / v2.Y,
				v1.Z / v2.Z
			);
		}

	public:
		Vector3()
			: X(0)
			, Y(0)
			, Z(0)
		{}

		Vector3(const Vector3& other)
			: X(other.X)
			, Y(other.Y)
			, Z(other.Z)
		{}

		Vector3(ValueType x, ValueType y, ValueType z)
			: X(x)
			, Y(y)
			, Z(z)
		{}

		Vector3(ValueType scalar)
			: X(scalar)
			, Y(scalar)
			, Z(scalar)
		{}

	public:
		Vector3& operator=(const Vector3& other)
		{
			X = other.X;
			Y = other.Y;
			Z = other.Z;
			return *this;
		}

		Vector3& operator+=(const Vector3& other)
		{
			X += other.X;
			Y += other.Y;
			Z += other.Z;
			return *this;
		}

		Vector3& operator-=(const Vector3& other)
		{
			X -= other.X;
			Y -= other.Y;
			Z -= other.Z;
			return *this;
		}

		Vector3& operator*=(ValueType scalar)
		{
			X *= scalar;
			Y *= scalar;
			Z *= scalar;
			return *this;
		}

		Vector3& operator*=(const Vector3& other)
		{
			X *= other.X;
			Y *= other.Y;
			Z *= other.Z;
			return *this;
		}

		Vector3& operator/=(ValueType scalar)
		{
			FloatType one_over_scalar = (FloatType)1 / (FloatType)scalar;
			X = (ValueType)((FloatType)X * one_over_scalar);
			Y = (ValueType)((FloatType)Y * one_over_scalar);
			Z = (ValueType)((FloatType)Z * one_over_scalar);
			return *this;
		}

		Vector3& operator/=(const Vector3& other)
		{
			X /= other.X;
			Y /= other.Y;
			Z /= other.Z;
			return *this;
		}

		constexpr ValueType operator[](Math::Axis axis) const
		{
			return *(&X + (uint8)axis);
		}

		constexpr ValueType operator[](uint8 axis) const
		{
			return *(&X + axis);
		}

	public:
		constexpr Vector3 ByAxis(Math::Axis a1, Math::Axis a2, Math::Axis a3) const
		{
			return Vector3(
				(*this)[a1],
				(*this)[a2],
				(*this)[a3]
			);
		}

		constexpr Vector3 ByAxis(uint8 a1, uint8 a2, uint8 a3) const
		{
			return Vector3(
				(*this)[a1],
				(*this)[a2],
				(*this)[a3]
			);
		}
	};

	template<typename ValueType, typename FloatType>
	Vector3<ValueType, FloatType> operator+(const Vector3<ValueType, FloatType>& v1, const Vector3<ValueType, FloatType>& v2)
	{
		return Vector3<ValueType, FloatType>::Add(v1, v2);
	}

	template<typename ValueType, typename FloatType>
	Vector3<ValueType, FloatType> operator-(const Vector3<ValueType, FloatType>& v1, const Vector3<ValueType, FloatType>& v2)
	{
		return Vector3<ValueType, FloatType>::Substract(v1, v2);
	}

	template<typename ValueType, typename FloatType>
	Vector3<ValueType, FloatType> operator*(const Vector3<ValueType, FloatType>& v, ValueType s)
	{
		return Vector3<ValueType, FloatType>::Multiply(v, s);
	}

	template<typename ValueType, typename FloatType>
	Vector3<ValueType, FloatType> operator*(ValueType s, const Vector3 <ValueType, FloatType>& v)
	{
		return Vector3<ValueType, FloatType>::Multiply(v, s);
	}

	template<typename ValueType, typename FloatType>
	Vector3<ValueType, FloatType> operator*(const Vector3<ValueType, FloatType>& v1, const Vector3<ValueType, FloatType>& v2)
	{
		return Vector3<ValueType, FloatType>::Multiply(v1, v2);
	}

	template<typename ValueType, typename FloatType>
	Vector3<ValueType, FloatType> operator/(const Vector3<ValueType, FloatType>& v, ValueType s)
	{
		return Vector3<ValueType, FloatType>::Divide(v, s);
	}

	template<typename ValueType, typename FloatType>
	Vector3<ValueType, FloatType> operator/(const Vector3<ValueType, FloatType>& v1, const Vector3<ValueType, FloatType>& v2)
	{
		return Vector3<ValueType, FloatType>::Divide(v1, v2);
	}

	using Vector3f = Vector3<float,  float>;
	using Vector3d = Vector3<double, double>;
	using Vector3i = Vector3<int32,  float>;
	using Vector3u = Vector3<uint32, float>;

}