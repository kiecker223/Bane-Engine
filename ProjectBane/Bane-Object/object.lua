group "Engine"
project "BaneObject"
    kind "StaticLib"
    links {"BaneCore", "BaneGraphicsCore"}
    includedirs { 
        "../Bane-Common",  
        "../Bane-Core",
        "../Bane-Math",
        "../Bane-Graphics"}

    files { "**.h",
            "**.cpp"}

    excludes { "BaneObject/Entity/Transform.cpp", "BaneObject/Components/ComponentBase.cpp" }