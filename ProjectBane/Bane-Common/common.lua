group "Core"
project "BaneCommon"
    kind "Utility"
    includedirs { "*", "Common/**.h" }
    files { "Common/**.h", 
            "Common/**.cpp" }