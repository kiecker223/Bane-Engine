group "Game"
project "BanePhysics"
    kind "StaticLib"
    links {"BaneCommon", "BaneCore"}
    includedirs {
        "../Bane-Common",
        "../Bane-Math",
        "../Bane-Core"
    }
    files {
        "**.cpp", "**.h"
    }