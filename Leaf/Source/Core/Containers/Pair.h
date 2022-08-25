// Copyright to Avram Traian. 2022 - 2022.
// File created on June 14 2022.

#pragma once

#include "Core/CoreTypes.h"

namespace Leaf {

	template<typename TypeA, typename TypeB>
	class Pair
	{
	public:
		Pair()
			: A()
			, B()
		{}

		Pair(const Pair& other)
			: A(other.A)
			, B(other.B)
		{}

		Pair(Pair&& other)
			: A(Leaf::Move(other.A))
			, B(Leaf::Move(other.B))
		{}

		Pair(const TypeA& a, const TypeB& b)
			: A(a)
			, B(b)
		{}

		Pair(const TypeA& a, TypeB&& b)
			: A(a)
			, B(Leaf::Move(b))
		{}

		Pair(TypeA&& a, const TypeB& b)
			: A(Leaf::Move(a))
			, B(b)
		{}

		Pair(TypeA&& a, TypeB&& b)
			: A(Leaf::Move(a))
			, B(Leaf::Move(b))
		{}

		~Pair()
		{
		}

	public:
		Pair& operator=(const Pair& other)
		{
			A = other.A;
			B = other.B;
			return *this;
		}

		Pair& operator=(Pair&& other)
		{
			A = Leaf::Move(other.A);
			B = Leaf::Move(other.B);
			return *this;
		}

	public:
		TypeA A;
		TypeB B;
	};

}