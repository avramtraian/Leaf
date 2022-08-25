// Copyright to Avram Traian. 2022 - 2022.
// File created on August 21 2022.

#pragma once

//////// Platforms ////////

#ifndef LF_PLATFORM_WINDOWS
#	error Leaf only supports Windows!
#	define LF_PLATFORM_WINDOWS (0)
#endif

#ifndef LF_PLATFORM_MACOS
#	define LF_PLATFORM_MACOS   (0)
#endif

#ifndef LF_PLATFORM_LINUX
#	define LF_PLATFORM_LINUX   (0)
#endif

//////// Import/Export Specifiers ////////

/** Platform switch. */
#if LF_PLATFORM_WINDOWS
#	ifndef _WIN64
#		error Leaf only supports 64-bit architectures!
#	endif

#	define LF_SPECIFIER_EXPORT __declspec(dllexport)
#	define LF_SPECIFIER_IMPORT __declspec(dllimport)

#elif LF_PLATFORM_MACOS
#	define LF_SPECIFIER_EXPORT __attribute__(visibility("default"))
#	define LF_SPECIFIER_IMPORT

#elif LF_PLATFORM_LINUX
#	define LF_SPECIFIER_EXPORT __attribute__(visibility("default"))
#	define LF_SPECIFIER_IMPORT

#endif

#ifdef LF_BUILD_CORE_LIBRARY
#	define LEAF_API LF_SPECIFIER_EXPORT
#else
#	define LEAF_API LF_SPECIFIER_IMPORT
#endif

//////// Compiler detection ////////

#ifdef _MSC_BUILD
#	define LF_COMPILER_MSVC (1)
#endif

#ifndef LF_COMPILER_MSVC
#	define LF_COMPILER_MSVC (0)
#endif

//////// Compiler Specific Keywords ////////

/** Compiler switch. */
#if LF_COMPILER_MSVC
#	define LF_DEBUGBREAK() __debugbreak()
#	define LF_INLINE  __forceinline

#	define LF_FUNCTION     __FUNCSIG__
#endif

/** Available on all compilers. */
#define LF_FILE __FILE__
#define LF_LINE __LINE__
#define LF_DATE __DATE__
#define LF_TIME __TIME__

#define LF_NODISCARD    [[nodiscard]]
#define LF_MAYBE_UNUSED [[maybe_unused]]
#define LF_LIKELY       [[likely]]
#define LF_UNLIKELY     [[unlikely]]

//////// Build Configurations ////////

#ifndef LF_CONFIGURATION_DEBUG
#	define LF_CONFIGURATION_DEBUG    (0)
#endif

#ifndef LF_CONFIGURATION_RELEASE
#	define LF_CONFIGURATION_RELEASE  (0)
#endif

#ifndef LF_CONFIGURATION_SHIPPING
#	define LF_CONFIGURATION_SHIPPING (0)
#endif