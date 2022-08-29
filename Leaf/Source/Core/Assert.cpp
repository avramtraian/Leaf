// Copyright to Avram Traian. 2022 - 2022.
// File created on August 29 2022.

#include "Assert.h"
#include "Logger.h"

namespace Leaf {

	void SubmitAssertionFailed(const char* expression, const char* file, const char* function, uint32 line)
	{
		String expr_string     = String::Format("EXPRESSION: %{}", expression);
		String file_string     = String::Format("FILE:       %{}", file);
		String function_string = String::Format("FUNCTION:   %{}", function);
		String line_string     = String::Format("LINE:       %{}", line);

		SizeT max_width = sizeof(" ASSERTION FAILED ") / sizeof(char) - 1;
		if (max_width < expr_string.Length())
			max_width = expr_string.Length();
		if (max_width < file_string.Length())
			max_width = file_string.Length();
		if (max_width < function_string.Length())
			max_width = function_string.Length();
		if (max_width < line_string.Length())
			max_width = line_string.Length();

		char whitespace_buffer[256] = {};
		Memory::Set(whitespace_buffer, ' ', sizeof(whitespace_buffer));

		{
			char header_buffer[256] = {};
			Memory::Set(header_buffer, '-', sizeof(header_buffer));

			SizeT header_length = max_width - (sizeof(" ASSERTION FAILED ") / sizeof(char) - 1);
			StringView header_pre = StringView(header_buffer, header_length / 2);
			StringView header_post = StringView(header_buffer, header_length / 2 + header_length % 2);
			LF_CORE_FATAL("+-%{} ASSERTION FAILED %{}-+", header_pre, header_post);
		}
		{
			StringView whitespace = StringView(whitespace_buffer, max_width - expr_string.Length());
			LF_CORE_FATAL("| %{}%{} |", expr_string, whitespace);
		}
		{
			StringView whitespace = StringView(whitespace_buffer, max_width - file_string.Length());
			LF_CORE_FATAL("| %{}%{} |", file_string, whitespace);
		}
		{
			StringView whitespace = StringView(whitespace_buffer, max_width - function_string.Length());
			LF_CORE_FATAL("| %{}%{} |", function_string, whitespace);
		}
		{
			StringView whitespace = StringView(whitespace_buffer, max_width - line_string.Length());
			LF_CORE_FATAL("| %{}%{} |", line_string, whitespace);
		}
		{
			char footer_buffer[256] = {};
			Memory::Set(footer_buffer, '-', sizeof(footer_buffer));

			StringView footer = StringView(footer_buffer, max_width);
			LF_CORE_FATAL("+-%{}-+\n", footer);
		}
	}

}