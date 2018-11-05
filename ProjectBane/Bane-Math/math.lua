group "Core"
project "BaneMath"
    kind "StaticLib"
    links {}
    includedirs { 
        "../Bane-Common" 
		}

    files { "Math/**.h",
            "Math/**.cpp",
            "KieckerMath.h"}