-- Copyright to Avram Traian. 2022 - 2022.
-- File created on August 21 2022.

workspace "Leaf"
	location "../../"

	configurations {
		"Debug", "Release", "Shipping"
	}

	platforms {
		"Win64"
	}

	filter "platforms:Win64"
		system "Windows"
		architecture "x86_64"
	filter ""

	startproject "Leaf-Editor"

	IncludeDirectories = {}
	LibraryDirectories = {}
	LibraryNames = {}

	IncludeDirectories["Leaf"] = "%{wks.location}/Leaf/Source"

	Vulkan = os.getenv("VULKAN_SDK")
	IncludeDirectories["Vulkan"] = Vulkan.."Include"
	LibraryDirectories["Vulkan"] = Vulkan.."Lib"
	LibraryNames["Vulkan"] = "vulkan-1"

	group "Core"
		include "Dev-Core-Leaf.lua"
	group ""

	group "Tools"
		include "Dev-Tool-Editor.lua"
	group ""

	group "ThirdParty"
	group ""