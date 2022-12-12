project "Raven"
kind "StaticLib"
   language "C++"
   cppdialect "C++latest"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "vendor/spdlog/include",
      "src",
      "../Walnut/Walnut/src"
   }

   links
   {
   }

   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
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
      defines { "PS_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"