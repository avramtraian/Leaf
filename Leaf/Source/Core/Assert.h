// Copyright to Avram Traian. 2022 - 2022.
// File created on August 29 2022.

#pragma once

#include "CoreTypes.h"

namespace Leaf {

	LEAF_API void SubmitAssertionFailed(const char* expression, const char* file, const char* function, uint32 line);

}

/** Build Configuration Switch. */
#if LF_CONFIGURATION_DEBUG
#	define LF_ENABLE_DEBUG_ASSERTS  (1)
#	define LF_ENABLE_DEBUG_VERIFIES (1)
#	define LF_ENABLE_ASSERTS        (1)
#	define LF_ENABLE_VERIFIES       (1)
#elif LF_CONFIGURATION_RELEASE
#	define LF_ENABLE_DEBUG_ASSERTS  (0)
#	define LF_ENABLE_DEBUG_VERIFIES (0)
#	define LF_ENABLE_ASSERTS        (1)
#	define LF_ENABLE_VERIFIES       (1)
#elif LF_CONFIGURATION_SHIPPING
#	define LF_ENABLE_DEBUG_ASSERTS  (0)
#	define LF_ENABLE_DEBUG_VERIFIES (0)
#	define LF_ENABLE_ASSERTS        (0)
#	define LF_ENABLE_VERIFIES       (0)
#endif

#if LF_ENABLE_ASSERTS
#	define LF_ASSERT(EXPRESSION)                                                       \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
		}

#	define LF_ASSERT_RETURN_VOID(EXPRESSION)                                           \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			return;                                                                    \
		}

#	define LF_ASSERT_RETURN(EXPRESSION, RETURN_VALUE)                                  \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			return (RETURN_VALUE);                                                     \
		}

#	define LF_ASSERT_BREAK(EXPRESSION)                                                 \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			break;                                                                     \
		}

#	define LF_ASSERT_CONTINUE(EXPRESSION)                                              \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			continue;                                                                  \
		}

#	define LF_ASSERT_AND(EXPRESSION, ...)                                              \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			__VA_ARGS__                                                                \
		}

#	define LF_ASSERT_NO_ENTRY()                                                       \
		{                                                                             \
			::Leaf::SubmitAssertionFailed("NO ENTRY", LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                          \
		}

#	define LF_ASSERT_NO_ENTRY_RETURN(RETURN_VALUE)                                    \
		{                                                                             \
			::Leaf::SubmitAssertionFailed("NO ENTRY", LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                          \
			return (RETURN_VALUE);													  \
		}

#else
#	define LF_ASSERT(EXPRESSION) // Excluded from build.
#	define LF_ASSERT_RETURN_VOID(EXPRESSION) // Excluded from build.
#	define LF_ASSERT_RETURN(EXPRESSION, RETURN_VALUE) // Excluded from build.
#	define LF_ASSERT_BREAK(EXPRESSION) // Excluded from build.
#	define LF_ASSERT_CONTINUE(EXPRESSION) // Excluded from build.
#	define LF_ASSERT_AND(EXPRESSION, ...) // Excluded from build.
#	define LF_ASSERT_NO_ENTRY() // Excluded from build.
#	define LF_ASSERT_NO_ENTRY_RETURN(RETURN_VALUE) return (RETURN_VALUE)

#endif

#if LF_ENABLE_VERIFIES
#	define LF_VERIFY(EXPRESSION)                                                       \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
		}

#	define LF_VERIFY_RETURN_VOID(EXPRESSION)                                           \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			return;                                                                    \
		}

#	define LF_VERIFY_RETURN(EXPRESSION, RETURN_VALUE)                                  \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			return (RETURN_VALUE);                                                     \
		}

#	define LF_VERIFY_BREAK(EXPRESSION)                                                 \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			break;                                                                     \
		}

#	define LF_VERIFY_CONTINUE(EXPRESSION)                                              \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			continue;                                                                  \
		}
#else
#	define LF_VERIFY(EXPRESSION)                      EXPRESSION
#	define LF_VERIFY_RETURN_VOID(EXPRESSION)          EXPRESSION
#	define LF_VERIFY_RETURN(EXPRESSION, RETURN_VALUE) EXPRESSION
#	define LF_VERIFY_BREAK(EXPRESSION)                EXPRESSION
#	define LF_VERIFY_CONTINUE(EXPRESSION)             EXPRESSION
#endif

#if LF_ENABLE_DEBUG_ASSERTS
#	define LF_DEBUG_ASSERT(EXPRESSION)                                                 \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
		}

#	define LF_DEBUG_ASSERT_RETURN_VOID(EXPRESSION)                                     \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			return;                                                                    \
		}

#	define LF_DEBUG_ASSERT_RETURN(EXPRESSION, RETURN_VALUE)                            \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			return (RETURN_VALUE);                                                     \
		}

#	define LF_DEBUG_ASSERT_BREAK(EXPRESSION)                                           \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			break;                                                                     \
		}

#	define LF_DEBUG_ASSERT_CONTINUE(EXPRESSION)                                        \
		if (!(EXPRESSION))                                                             \
		{                                                                              \
			::Leaf::SubmitAssertionFailed(#EXPRESSION, LF_FILE, LF_FUNCTION, LF_LINE); \
			LF_DEBUGBREAK();                                                           \
			continue;                                                                  \
		}
#else
#	define LF_DEBUG_ASSERT(EXPRESSION) // Excluded from build.
#	define LF_DEBUG_ASSERT_RETURN_VOID(EXPRESSION) // Excluded from build.
#	define LF_DEBUG_ASSERT_RETURN(EXPRESSION, RETURN_VALUE) // Excluded from build.
#	define LF_DEBUG_ASSERT_BREAK(EXPRESSION) // Excluded from build.
#	define LF_DEBUG_ASSERT_CONTINUE(EXPRESSION) // Excluded from build.
#endif