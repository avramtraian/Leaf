// Copyright to Avram Traian. 2022 - 2022.
// File created on August 28 2022.

#pragma once

#include "Core/Logger.h"

#define LF_OVERRIDE_RENDERER_LOGS (0)

#if LF_OVERRIDE_RENDERER_LOGS
#	if LF_CONFIGURATION_DEBUG
#		define LF_ENABLE_RENDERER_LOG_DEBUG (1)
#		define LF_ENABLE_RENDERER_LOG_TRACE (1)
#		define LF_ENABLE_RENDERER_LOG_INFO  (1)
#		define LF_ENABLE_RENDERER_LOG_WARN  (1)
#		define LF_ENABLE_RENDERER_LOG_ERROR (1)
#		define LF_ENABLE_RENDERER_LOG_FATAL (1)
#	elif LF_CONFIGURATION_RELEASE
#		define LF_ENABLE_RENDERER_LOG_DEBUG (0)
#		define LF_ENABLE_RENDERER_LOG_TRACE (0)
#		define LF_ENABLE_RENDERER_LOG_INFO  (1)
#		define LF_ENABLE_RENDERER_LOG_WARN  (1)
#		define LF_ENABLE_RENDERER_LOG_ERROR (1)
#		define LF_ENABLE_RENDERER_LOG_FATAL (1)
#	elif LF_CONFIGURATION_SHIPPING
#		define LF_ENABLE_RENDERER_LOG_DEBUG (0)
#		define LF_ENABLE_RENDERER_LOG_TRACE (0)
#		define LF_ENABLE_RENDERER_LOG_INFO  (0)
#		define LF_ENABLE_RENDERER_LOG_WARN  (0)
#		define LF_ENABLE_RENDERER_LOG_ERROR (1)
#		define LF_ENABLE_RENDERER_LOG_FATAL (1)
#	endif
#else
#	define LF_ENABLE_RENDERER_LOG_DEBUG LF_ENABLE_LOG_DEBUG
#	define LF_ENABLE_RENDERER_LOG_TRACE LF_ENABLE_LOG_TRACE
#	define LF_ENABLE_RENDERER_LOG_INFO  LF_ENABLE_LOG_INFO
#	define LF_ENABLE_RENDERER_LOG_WARN  LF_ENABLE_LOG_WARN
#	define LF_ENABLE_RENDERER_LOG_ERROR LF_ENABLE_LOG_ERROR
#	define LF_ENABLE_RENDERER_LOG_FATAL LF_ENABLE_LOG_FATAL
#endif

#if LF_ENABLE_RENDERER_LOG_DEBUG
#	define LF_RENDERER_DEBUG(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Debug, "RENDERER", __VA_ARGS__)
#else
#	define LF_RENDERER_DEBUG(...) // Excluded from build.
#endif

#if LF_ENABLE_RENDERER_LOG_TRACE
#	define LF_RENDERER_TRACE(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Trace, "RENDERER", __VA_ARGS__)
#else
#	define LF_RENDERER_TRACE(...) // Excluded from build.
#endif

#if LF_ENABLE_RENDERER_LOG_INFO
#	define LF_RENDERER_INFO(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Info, "RENDERER", __VA_ARGS__)
#else
#	define LF_RENDERER_INFO(...) // Excluded from build.
#endif

#if LF_ENABLE_RENDERER_LOG_WARN
#	define LF_RENDERER_WARN(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Warn, "RENDERER", __VA_ARGS__)
#else
#	define LF_RENDERER_WARN(...) // Excluded from build.
#endif

#if LF_ENABLE_RENDERER_LOG_ERROR
#	define LF_RENDERER_ERROR(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Error, "RENDERER", __VA_ARGS__)
#else
#	define LF_RENDERER_ERROR(...) // Excluded from build.
#endif

#if LF_ENABLE_RENDERER_LOG_FATAL
#	define LF_RENDERER_FATAL(...) ::Leaf::Logger::Submit(::Leaf::Logger::LogType::Fatal, "RENDERER", __VA_ARGS__)
#else
#	define LF_RENDERER_FATAL(...) // Excluded from build.
#endif