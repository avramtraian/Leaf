-- Copyright to Avram Traian. 2022 - 2022.
-- File created on August 21 2022.

project "Leaf"
	location "%{wks.location}/Leaf"

	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	
	staticruntime "Off"
	rtti "Off"
	exceptionhandling "Off"
	characterset "Unicode"

	-- NOTE (Avr): Premake token expansion seems to be disabled.
	pchheader "LeafPCH.h"
	pchsource "../Source/LeafPCH.cpp"

	targetname "Leaf-Core"
	targetdir "%{wks.location}/Bin/%{cfg.platform}/%{cfg.buildcfg}"
	objdir "%{wks.location}/Bin-Int/%{cfg.buildcfg}/%{prj.name}"

	files {
		"%{prj.location}/Source/**.h",
		"%{prj.location}/Source/**.cpp",

		"%{prj.location}/Build/**.lua",

		"%{prj.location}/ThirdParty/VMA/**.h"
	}

	includedirs {
		(IncludeDirectories["Leaf"]),

		(IncludeDirectories["VulkanSDK"]),

		"%{prj.location}/ThirdParty/VMA/Include"
	}

	forceincludes {
		"LeafPCH.h"
	}

	defines {
		"LF_BUILD_CORE_LIBRARY"
	}

	libdirs {
		(LibraryDirectories["VulkanSDK"])
	}

	links {
		(LibraryNames["VulkanSDK"])
	}

	filter "platforms:Win64"
		systemversion "latest"

		defines {
			"LF_PLATFORM_WINDOWS=(1)"
		}

	filter "configurations:Debug"
		optimize "Off"
		symbols "On"
		runtime "Debug"

		defines {
			"LF_CONFIGURATION_DEBUG=(1)"
		}

	filter "configurations:Release"
		optimize "On"
		symbols "On"
		runtime "Release"

		defines {
			"LF_CONFIGURATION_RELEASE=(1)"
		}

	filter "configurations:Shipping"
		optimize "Speed"
		symbols "Off"
		runtime "Release"

		defines {
			"LF_CONFIGURATION_SHIPPING=(1)"
		}

	filter {}