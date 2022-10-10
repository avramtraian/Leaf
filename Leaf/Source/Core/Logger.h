// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "CoreTypes.h"

namespace Leaf {

	class LEAF_API Logger
	{
	public:
		enum class LogType : uint8
		{
			Unknown = 0,
			Debug, Trace, Info, Warn, Error, Fatal,
			MaxEnumValue
		};

	public:
		static bool Initialize();

		static void Shutdown();

	public:
		static void Submit(LogType type, StringViewUTF8 tag, StringViewUTF8 message);

		template<typename... Args>
		static void Submit(LogType type, StringViewUTF8 tag, StringViewUTF8 message, Args&&... args)
		{
			Submit(type, tag, StringUTF8::Format(message, Leaf::Forward<Args>(args)...));
		}
	};

}

/** Build Configuration Switch. */
#if LF_CONFIGURATION_DEBUG
#	define LF_ENABLE_LOG_DEBUG (1)
#	define LF_ENABLE_LOG_TRACE (1)
#	define LF_ENABLE_LOG_INFO  (1)
#	define LF_ENABLE_LOG_WARN  (1)
#	define LF_ENABLE_LOG_ERROR (1)
#	define LF_ENABLE_LOG_FATAL (1)
#elif LF_CONFIGURATION_RELEASE
#	define LF_ENABLE_LOG_DEBUG (0)
#	define LF_ENABLE_LOG_TRACE (0)
#	define LF_ENABLE_LOG_INFO  (1)
#	define LF_ENABLE_LOG_WARN  (1)
#	define LF_ENABLE_LOG_ERROR (1)
#	define LF_ENABLE_LOG_FATAL (1)
#elif LF_CONFIGURATION_SHIPPING
#	define LF_ENABLE_LOG_DEBUG (0)
#	define LF_ENABLE_LOG_TRACE (0)
#	define LF_ENABLE_LOG_INFO  (0)
#	define LF_ENABLE_LOG_WARN  (0)
#	define LF_ENABLE_LOG_ERROR (1)
#	define LF_ENABLE_LOG_FATAL (1)
#endif