// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "Application.h"

namespace Leaf {

	/**
	* User-defined function. Creates the application instance. This can return any class derived from Application.
	* This is mandatory to be defined.
	* 
	* @param cmd_line_args The command line arguments passed to the executable.
	* 
	* @returns The application instance.
	*/
	Application* CreateApplication(const CommandLineArguments& cmd_line_args);


	/**
	* The process entry point. Called on all platforms and all build configurations.
	* 
	* @param cmd_args Array of all the arguments passed to the executable on launch.
	* @param cmd_args_count The count of the command line arguments passed to the executable.
	* 
	* @returns The process exit code.
	*/
	inline int32 Main(char** cmd_args, uint16 cmd_args_count)
	{
		// The command argument list passed to the executable.
		CommandLineArguments cmd_line_args;
		cmd_line_args.Arguments = cmd_args;
		cmd_line_args.ArgumentsCount = cmd_args_count;

		// The executable exit code. Used to debug runtime failures.
		int32 exit_code = 0;

		do
		{
			// Initializes the core systems.
			if (!InitializeCore())
				continue;

			// Instantiates (and initializes) the application.
			Application* application = CreateApplication(cmd_line_args);

			// Runs the application. The main loop is executed here.
			exit_code = application->Run();

			// Destroys (and shutdowns) the application.
			delete application;

			// Shutdowns the core systems.
			ShutdownCore();
		}
		// If this returns true, the loop will be executed again, restarting the application.
		while (ShouldRestartApplication());

		// The executable finishes. The process will be closed.
		return exit_code;
	}

}

/** Platform switch. */
#if LF_PLATFORM_WINDOWS
#	if LF_CONFIGURATION_SHIPPING
#		include "Platform/Windows/WindowsHeaders.h"

		int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
		{
			return (int)(Leaf::Main((char**)__argv, (Leaf::uint16)__argc));
		}

#	else
		int main(int argc, char** argv)
		{
			return (int)(Leaf::Main((char**)argv, (Leaf::uint16)argc));
		}

#	endif

#elif LF_PLATFORM_MACOS
	int main(int argc, char** argv)
	{
		return (int)(Leaf::Main((char**)argv, (Leaf::uint16)argc));
	}

#elif LF_PLATFORM_LINUX
	int main(int argc, char** argv)
	{
		return (int)(Leaf::Main((char**)argv, (Leaf::uint16)argc));
	}

#endif