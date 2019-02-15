group "Core"
project "BaneUI"
    kind "StaticLib"
    links { "BaneGraphicsCore", "BaneGraphicsRenderer", "BaneObject" }
    includedirs {
        "../Bane-Common/",
		"../Bane-Graphics-Core",
        "../Bane-Core",
        "../Bane-Math",
        "../Bane-Platform",
        "../External/include"
    }
    files { "**.h", "**.cpp" }
    