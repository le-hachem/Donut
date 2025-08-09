workspace "Donut"
	architecture "x64"
	startproject "Donut"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glfw"] = "Vendor/glfw/include"
IncludeDir["glad"] = "Vendor/glad/include"

group "Dependencies"

project "GLAD"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Vendor/glad/include/glad/glad.h",
        "Vendor/glad/include/KHR/khrplatform.h",

        "Vendor/glad/src/glad.c"
    }

    includedirs
    {
        "Vendor/glad/include"
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "GLFW"
	kind "StaticLib"
	language "C"
	staticruntime "off"
	warnings "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Vendor/glfw/include/GLFW/glfw3.h",
		"Vendor/glfw/include/GLFW/glfw3native.h",
		
		"Vendor/glfw/src/context.c",
		"Vendor/glfw/src/init.c",
		"Vendor/glfw/src/input.c",
		"Vendor/glfw/src/monitor.c",
		"Vendor/glfw/src/platform.c",
		"Vendor/glfw/src/vulkan.c",
		"Vendor/glfw/src/window.c",

		"Vendor/glfw/src/internal.h",
		"Vendor/glfw/src/platform.h",
		"Vendor/glfw/src/mappings.h",

		"Vendor/glfw/src/null_init.c",
		"Vendor/glfw/src/null_joystick.c",
		"Vendor/glfw/src/null_joystick.h",
		"Vendor/glfw/src/null_monitor.c",
		"Vendor/glfw/src/null_platform.h",
		"Vendor/glfw/src/null_window.c",
	}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		
		files
		{
			"Vendor/glfw/src/x11_init.c",
			"Vendor/glfw/src/x11_monitor.c",
			"Vendor/glfw/src/x11_platform.h",
			"Vendor/glfw/src/x11_window.c",
			"Vendor/glfw/src/xkb_unicode.c",
			"Vendor/glfw/src/xkb_unicode.h",
			
			"Vendor/glfw/src/wl_init.c",
			"Vendor/glfw/src/wl_monitor.c",
			"Vendor/glfw/src/wl_platform.h",
			"Vendor/glfw/src/wl_window.c",
			
			"Vendor/glfw/src/posix_module.c",
			"Vendor/glfw/src/posix_time.c",
			"Vendor/glfw/src/posix_time.h",
			"Vendor/glfw/src/posix_thread.c",
			"Vendor/glfw/src/posix_thread.h",
			
			"Vendor/glfw/src/glx_context.c",
			"Vendor/glfw/src/egl_context.c",
			"Vendor/glfw/src/osmesa_context.c",
			
			"Vendor/glfw/src/linux_joystick.c",
			"Vendor/glfw/src/linux_joystick.h"
		}

		defines
		{
			"_GLFW_X11"
		}

	filter "system:macosx"
		pic "On"

		files
		{
			"Vendor/glfw/src/cocoa_init.m",
			"Vendor/glfw/src/cocoa_joystick.h",
			"Vendor/glfw/src/cocoa_joystick.m",
			"Vendor/glfw/src/cocoa_monitor.m",
			"Vendor/glfw/src/cocoa_platform.h",
			"Vendor/glfw/src/cocoa_time.c",
			"Vendor/glfw/src/cocoa_time.h",
			"Vendor/glfw/src/cocoa_window.m",
			
			"Vendor/glfw/src/nsgl_context.m",
			"Vendor/glfw/src/egl_context.c",
			"Vendor/glfw/src/osmesa_context.c",
			
			"Vendor/glfw/src/posix_module.c",
			"Vendor/glfw/src/posix_thread.c",
			"Vendor/glfw/src/posix_thread.h"
		}

		defines
		{
			"_GLFW_COCOA"
		}

	filter "system:windows"
		systemversion "latest"

		files
		{
			"Vendor/glfw/src/win32_init.c",
			"Vendor/glfw/src/win32_joystick.c",
			"Vendor/glfw/src/win32_joystick.h",
			"Vendor/glfw/src/win32_module.c",
			"Vendor/glfw/src/win32_monitor.c",
			"Vendor/glfw/src/win32_platform.h",
			"Vendor/glfw/src/win32_thread.c",
			"Vendor/glfw/src/win32_thread.h",
			"Vendor/glfw/src/win32_time.c",
			"Vendor/glfw/src/win32_time.h",
			"Vendor/glfw/src/win32_window.c",
			"Vendor/glfw/src/wgl_context.c",
			"Vendor/glfw/src/egl_context.c",
			"Vendor/glfw/src/osmesa_context.c"
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS",
		}

		links
		{
			"gdi32",
			"user32",
			"kernel32",
			"shell32",
			"msvcrt"
		}

		linkoptions
		{
			"/NODEFAULTLIB:library"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter { "system:windows", "configurations:Debug-AS" }	
		runtime "Debug"
		symbols "on"
		sanitize { "Address" }
		flags { "NoRuntimeChecks", "NoIncrementalLink" }

	filter "configurations:Release"
		runtime "Release"
		optimize "speed"

    filter "configurations:Dist"
		runtime "Release"
		optimize "speed"
        symbols "off"

group ""

project "Donut"
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}"
	}

    links
    {
        "GLFW",
        "GLAD",
    }

	filter "system:windows"
		systemversion "latest"
        defines
		{
			"GLFW_INCLUDE_NONE"
		}
		
		links
		{
			"gdi32",
			"user32",
			"kernel32",
			"shell32",
			"opengl32",
			"msvcrt"
		}

		linkoptions
		{
			"/NODEFAULTLIB:library"
		}
		
	filter "configurations:Debug"
		defines "DONUT_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "DONUT_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "DONUT_DIST"
		runtime "Release"
		optimize "on"