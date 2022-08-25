-- Copyright to Avram Traian. 2022 - 2022.
-- File created on August 21 2022.

project "Leaf-Editor"
	location "%{wks.location}/Leaf-Editor"

	language "C++"
	cppdialect "C++20"

	staticruntime "Off"
	rtti "Off"
	exceptionhandling "Off"
	characterset "Unicode"

	-- NOTE (Avr): Premake token expansion seems to be disabled.
	pchheader "EditorPCH.h"
	pchsource "../../Leaf-Editor/Source/EditorPCH.cpp"

	targetname "Leaf-Editor"
	targetdir "%{wks.location}/Bin/%{cfg.platform}/%{cfg.buildcfg}"
	objdir "%{wks.location}/Bin-Int/%{cfg.buildcfg}/%{prj.name}"

	files {
		"%{prj.location}/Source/**.h",
		"%{prj.location}/Source/**.cpp"
	}

	includedirs {
		"%{prj.location}/Source",

		(IncludeDirectories["Leaf"])
	}

	forceincludes {
		"EditorPCH.h"
	}

	links {
		"Leaf"
	}

	filter "platforms:Win64"
		systemversion "latest"

		defines {
			"LF_PLATFORM_WINDOWS=(1)"
		}

	filter "configurations:Debug"
		kind "ConsoleApp"

		optimize "Off"
		symbols "On"
		runtime "Debug"

		defines {
			"LF_CONFIGURATION_DEBUG=(1)"
		}

	filter "configurations:Release"
		kind "ConsoleApp"

		optimize "On"
		symbols "On"
		runtime "Release"

		defines {
			"LF_CONFIGURATION_RELEASE=(1)"
		}

	filter "configurations:Shipping"
		kind "WindowedApp"

		optimize "Speed"
		symbols "Off"
		runtime "Release"

		defines {
			"LF_CONFIGURATION_SHIPPING=(1)"
		}

	filter {}