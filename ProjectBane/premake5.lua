-- premake5.lua

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
    warnings "Extra"
    location "Generated/"

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
    startproject "BaneIrradianceGenerator"

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
   
include "Bane-Executable/executable.lua"
include "Bane-Systems/platform.lua"
include "Bane-Common/common.lua"
include "Bane-Core/core.lua"
include "Bane-Object/object.lua"
include "Bane-Graphics/graphics.lua"
include "Bane-Math/math.lua"
include "Bane-Irradiance-Generator/irradiance.lua"
include "Bane-Game/game.lua"