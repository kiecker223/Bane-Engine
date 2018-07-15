group "Core"
project "BaneCommon"
    kind "StaticLib"
    includedirs { "*", "Common/**.h" }
    files { "Common/**.h", 
            "Common/**.cpp" }