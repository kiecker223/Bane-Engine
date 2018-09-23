group "Engine"
project "BaneObject"
    kind "StaticLib"
    links {"BaneCore", "BaneGraphicsCore", "BanePlatform", "BanePhysics" }
    includedirs { 
        "../Bane-Common/",  
        "../Bane-Core/",
        "../Bane-Math/",
        "../Bane-Graphics-Core/",
		"../Bane-Platform/",
		"../Bane-Physics" }

    files { "**.h",
            "**.cpp"}

    excludes { "BaneObject/Entity/Transform.cpp", "BaneObject/Components/ComponentBase.cpp" }
