group "Platform"
project "BanePlatform"
    kind "StaticLib"
	links { "BaneCommon", "dinput8", "dxguid", "BaneCore" }
    includedirs { "../Bane-Common", "../Bane-Math", "../Bane-Core" }
    filter "system:windows"
        files {
            "Platform-Windows/**.cpp"
        }
    
	files {
		"Platform/System/File/**.cpp",
		"Platform/Input/**.cpp",
		"**.h"
	}
 