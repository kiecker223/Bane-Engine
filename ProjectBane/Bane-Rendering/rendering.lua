group "Graphics"
    project "BaneGraphicsRenderer"
        kind "StaticLib"
        links {"BaneCore", "BaneGraphicsCore", "BanePlatform" }
        includedirs { 
            "../Bane-Common",
            "../Bane-Core",
            "../Bane-Math",
            "../Bane-Graphics-Core",
            "../Bane-Platform",
            "../External/include",
			"*"
        }
        libdirs {"../External/libs/" }
        files { "**.h",
                "**.cpp"}

            