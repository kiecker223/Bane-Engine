group "Platform"
project "BanePlatform"
    kind "StaticLib"
	links { "BaneCommon" }
    includedirs { "../Bane-Common" }
    filter "system:windows"
        files {
            "Platform-Windows/**.cpp"
        }
    
	files {
		"Platform/System/File/**.cpp",
		"**.h"
	}
 