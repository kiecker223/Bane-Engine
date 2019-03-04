-- premake5.lua

newoption {
 
  trigger = "andrew",
  description = "Make shit suck less",
 
}
 

function SetupWinSDK()
  if os.getversion().majorversion == 10 then
    local sRegArch = iif( os.is64bit(), "\\Wow6432Node\\", "\\" )
    local sWin10SDK = os.getWindowsRegistry( "HKLM:SOFTWARE" .. sRegArch .. "Microsoft\\Microsoft SDKs\\Windows\\v10.0\\ProductVersion" )

    -- apparently it needs an extra ".0" to be recognized by VS
    if sWin10SDK ~= nil then systemversion( sWin10SDK .. ".0" ) end
  end
end


workspace "ProjectBane"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    warnings "Default"

    if _OPTIONS["ANDREW"] then
      location "Generated/"
--	  files {"../Tools/Formatting/.clang-format"}
--	  vpaths {
--		["Source"] = "**.cpp",
--		["Headers"] = "**.h",
--		["Formatting"] = "../Tools/Formatting/.clang-format",
--	  }
    end

    filter "platforms:x64"
      architecture "x86_64"
    filter {}
   
    filter "configurations:Debug"
      defines { "DEBUG" }

    filter "configurations:Release"
      defines { "RELEASE", "NDEBUG" }
      flags {
        "LinkTimeOptimization",
      }
      optimize "On"

    filter {"platforms:x64", "configurations:Release"}
      targetdir "Build/release"
    filter {"platforms:x64", "configurations:Debug"}
      targetdir "Build/Debug"
    filter {}

    objdir "%{cfg.targetdir}/obj"

    symbols "FastLink"

    filter {"configurations:Release"}
      symbols "Full"
    filter {}

    flags {
      "MultiProcessorCompile",
      "StaticRuntime",
      "FatalWarnings",
    }

    exceptionhandling "Off"
    cppdialect "C++17"
    language "C++"
    characterset "ASCII"
    startproject "BaneGame"

    vpaths {
      ["Lua"] = "**.lua"
    }
    
    filter "system:windows"
      SetupWinSDK()

project "BuildFiles"
    kind "None"
    files {"**.lua"}

project("Regenerate premake")
    kind "Utility"
    prebuildcommands("%{prj.location}../Tools/premake5.exe --file=%{prj.location}../premake5.lua vs2017")

include "Bane-Common/bane_common.lua"
include "Bane-Core/bane_core.lua"
include "Bane-Math/math.lua"
include "Bane-Platform/platform.lua"
include "Bane-Graphics-Core/graphics_core.lua"
include "Bane-Shader-Compiler/shader_compiler.lua"
include "Bane-Irradiance-Generator/irradiance.lua"
include "Bane-Game/Game.lua"
include "Bane-Font-Builder/font-builder.lua"
