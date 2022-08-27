// Copyright to Avram Traian. 2022 - 2022.
// File created on August 21 2022.

#pragma once

#include "CoreDefines.h"

/** Platform switch. */
#if LF_PLATFORM_WINDOWS
#	include "Platform/Windows/WindowsPlatformTypes.h"
	namespace Leaf { using PlatformTypes = WindowsPlatformTypes; }

#elif LF_PLATFORM_MACOS
#	include "Platform/MacOS/MacOSPlatformTypes.h"
	namespace Leaf { using PlatformTypes = MacOSPlatformTypes; }

#elif LF_PLATFORM_LINUX
#	include "Platform/Linux/LinuxPlatformTypes.h"
	namespace Leaf { using PlatformTypes = LinuxPlatformTypes; }

#endif

namespace Leaf {

	/** An 8-bit Integer. Unsigned. */
	using uint8 = PlatformTypes::uint8;

	/** A 16-bit Integer. Unsigned. */
	using uint16 = PlatformTypes::uint16;

	/** A 32-bit Integer. Unsigned. */
	using uint32 = PlatformTypes::uint32;

	/** A 64-bit Integer. Unsigned. */
	using uint64 = PlatformTypes::uint64;

	/** An 8-bit Integer. Signed. */
	using int8 = PlatformTypes::int8;

	/** A 16-bit Integer. Signed. */
	using int16 = PlatformTypes::int16;

	/** A 32-bit Integer. Signed. */
	using int32 = PlatformTypes::int32;

	/** A 64-bit Integer. Signed. */
	using int64 = PlatformTypes::int64;

	/** An Unsigned Integer representing a size. Usually 64-bit, but never assume. */
	using SizeT = PlatformTypes::SizeT;

	constexpr SizeT LF_INVALID_SIZE = static_cast<SizeT>(-1);

	template<typename T>
	struct RemoveReference
	{
		using Type = T;
	};

	template<typename T>
	struct RemoveReference<T&>
	{
		using Type = T;
	};

	template<typename T>
	struct RemoveReference<T&&>
	{
		using Type = T;
	};

	template<typename T>
	using RemoveReferenceType = typename RemoveReference<T>::Type;

	template<typename T>
	struct RemoveConst
	{
		using Type = T;
	};

	template<typename T>
	struct RemoveConst<const T>
	{
		using Type = T;
	};

	template<typename T>
	using RemoveConstType = typename RemoveConst<T>::Type;

	template<typename T>
	struct ArrayToPointerDecay
	{
		using Type = T;
	};

	template<typename T, SizeT N>
	struct ArrayToPointerDecay<T[N]>
	{
		using Type = T*;
	};

	template<typename T>
	using ArrayToPointerDecayType = ArrayToPointerDecay<T>::Type;

	template<typename T>
	constexpr RemoveReferenceType<T>&& Move(T&& object) noexcept
	{
		return static_cast<RemoveReferenceType<T>&&>(object);
	}

	template<typename T>
	constexpr T&& Forward(RemoveReferenceType<T>& object) noexcept
	{
		return static_cast<T&&>(object);
	}

}