group "Graphics"
    project "BaneGraphicsRenderer"
        kind "StaticLib"
        links {"BaneCore", "BaneGraphicsCore", "BaneCoreComponents"}
        includedirs { 
            "../Bane-Common",  
            "../Bane-Core",
            "../Bane-Object",
            "../Bane-Core-Components",
            "../Bane-Math",
            "../Bane-Graphics-Core",
            "../Bane-Application",
            "../Bane-Platform/Platform",
            "../External/include",
			"*"
        }
        libdirs {"../External/libs/" }
        files { "**.h",
                "**.cpp"}

        --excludes { "Graphics/Rendering/RendererInterface.cpp",  "Graphics/Rendering/RendererInterface.h"}
            