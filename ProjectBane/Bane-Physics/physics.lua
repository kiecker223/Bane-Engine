group "Game"
project "BanePhysics"
    kind "StaticLib"
    links {"BaneCommon", "BaneCore", "BaneGraphicsCore" }
    includedirs {
        "../Bane-Common",
        "../Bane-Math",
		"../Bane-Graphics-Core",
        "../Bane-Core"
    }
    files {
        "**.cpp", "**.h"
    }