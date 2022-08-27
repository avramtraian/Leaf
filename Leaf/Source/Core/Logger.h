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
		static void Submit(LogType type, StringView tag, StringView message);

		template<typename... Args>
		static void Submit(LogType type, StringView tag, StringView message, Args&&... args)
		{
			Submit(type, tag, String::Format(message, Leaf::Forward<Args>(args)...).ToView());
		}
	};

}