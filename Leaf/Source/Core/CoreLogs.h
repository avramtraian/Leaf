// Copyright to Avram Traian. 2022 - 2022.
// File created on August 29 2022.

#pragma once

#include "Logger.h"

#define LF_OVERRIDE_CORE_LOGS (0)

#if LF_OVERRIDE_CORE_LOGS
#	if LF_CONFIGURATION_DEBUG
#		define LF_ENABLE_CORE_LOG_DEBUG (1)
#		define LF_ENABLE_CORE_LOG_TRACE (1)
#		define LF_ENABLE_CORE_LOG_INFO  (1)
#		define LF_ENABLE_CORE_LOG_WARN  (1)
#		define LF_ENABLE_CORE_LOG_ERROR (1)
#		define LF_ENABLE_CORE_LOG_FATAL (1)
#	elif LF_CONFIGURATION_RELEASE
#		define LF_ENABLE_CORE_LOG_DEBUG (0)
#		define LF_ENABLE_CORE_LOG_TRACE (0)
#		define LF_ENABLE_CORE_LOG_INFO  (1)
#		define LF_ENABLE_CORE_LOG_WARN  (1)
#		define LF_ENABLE_CORE_LOG_ERROR (1)
#		define LF_ENABLE_CORE_LOG_FATAL (1)
#	elif LF_CONFIGURATION_SHIPPING
#		define LF_ENABLE_CORE_LOG_DEBUG (0)
#		define LF_ENABLE_CORE_LOG_TRACE (0)
#		define LF_ENABLE_CORE_LOG_INFO  (0)
#		define LF_ENABLE_CORE_LOG_WARN  (0)
#		define LF_ENABLE_CORE_LOG_ERROR (1)
#		define LF_ENABLE_CORE_LOG_FATAL (1)
#	endif
#else
#	define LF_ENABLE_CORE_LOG_DEBUG LF_ENABLE_LOG_DEBUG
#	define LF_ENABLE_CORE_LOG_TRACE LF_ENABLE_LOG_TRACE
#	define LF_ENABLE_CORE_LOG_INFO  LF_ENABLE_LOG_INFO
#	define LF_ENABLE_CORE_LOG_WARN  LF_ENABLE_LOG_WARN
#	define LF_ENABLE_CORE_LOG_ERROR LF_ENABLE_LOG_ERROR
#	define LF_ENABLE_CORE_LOG_FATAL LF_ENABLE_LOG_FATAL
#endif

#if LF_ENABLE_CORE_LOG_DEBUG
#	define LF_CORE_DEBUG(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Debug, "CORE", __VA_ARGS__)
#else
#	define LF_CORE_DEBUG(...) // Excluded from build.
#endif

#if LF_ENABLE_CORE_LOG_TRACE
#	define LF_CORE_TRACE(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Trace, "CORE", __VA_ARGS__)
#else
#	define LF_CORE_TRACE(...) // Excluded from build.
#endif

#if LF_ENABLE_CORE_LOG_INFO
#	define LF_CORE_INFO(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Info, "CORE", __VA_ARGS__)
#else
#	define LF_CORE_INFO(...) // Excluded from build.
#endif

#if LF_ENABLE_CORE_LOG_WARN
#	define LF_CORE_WARN(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Warn, "CORE", __VA_ARGS__)
#else
#	define LF_CORE_WARN(...) // Excluded from build.
#endif

#if LF_ENABLE_CORE_LOG_ERROR
#	define LF_CORE_ERROR(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Error, "CORE", __VA_ARGS__)
#else
#	define LF_CORE_ERROR(...) // Excluded from build.
#endif

#if LF_ENABLE_CORE_LOG_FATAL
#	define LF_CORE_FATAL(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Fatal, "CORE", __VA_ARGS__)
#else
#	define LF_CORE_FATAL(...) // Excluded from build.
#endif