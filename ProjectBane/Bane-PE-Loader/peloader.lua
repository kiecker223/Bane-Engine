group "Platform"
project "BanePELoader"
    kind "StaticLib"
	links { "BaneCommon" }
    includedirs { "../Bane-Common" }
	defines     { "_CRT_SECURE_NO_WARNINGS" }
	files {
        "**.cpp",
		"**.h"
	}
 