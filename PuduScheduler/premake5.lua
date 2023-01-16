project "PuduScheduler"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    targetdir "bin/%{cfg.buildcfg}"
    staticruntime "off"

    files { "src/**.h", "src/**.cpp" }

    includedirs
    {
        "src",
        "../Walnut/vendor/imgui",
        "../Walnut/vendor/glfw/include",
        "../Walnut/vendor/glm",
        "../Walnut/vendor/implot",
        "../Walnut/Walnut/src",
        "../Raven/vendor/spdlog/include",
        "../Raven/src",
        "%{IncludeDir.VulkanSDK}",
    }
    libdirs{
	}

    links
    {
        "Walnut",
        "Raven"
    }

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
 
    filter "system:windows"
       systemversion "latest"
       defines { "PS_PLATFORM_WINDOWS" }
 
    filter "configurations:Debug"
       defines { "PS_DEBUG" }
       runtime "Debug"
       symbols "On"
 
    filter "configurations:Release"
       defines { "PS_RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"
 
    filter "configurations:Dist"
       kind "WindowedApp"
       defines { "PS_DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"