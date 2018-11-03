group "App"
project "BaneExecutable"
    kind "ConsoleApp"
    links { "BaneCore", "BaneGame", "BaneObject", "BanePlatform", "BaneGraphicsCore", "BaneGraphicsRenderer", "BanePhysics" }
    includedirs {
        "../Bane-Common",
        "../Bane-Math",
        "../Bane-Core",
		"../Bane-Game",
        "../Bane-Graphics-Core",
		"../Bane-Object",
		"../Bane-Platform",
		"../Bane-Rendering",
        "../External/include",
		"../Bane-Platform/Platform",
		"../Bane-Application",
		"../Bane-Physics"
    }
	characterset "ASCII"
    libdirs {"../External/libs/"}
    filter "system:windows"
        links {"d3d12", "assimp-vc140-mt", "DevIL.lib", "ILU.lib", "dxgi", "dxguid", "d3dcompiler", }
        includedirs {"../Bane-Systems/Platform-Windows/"}
    files {"**.h", "**.cpp"}
	
	debugdir "../Build/Debug/"

	dependson { "BaneShaderCompiler" }

	postbuildcommands {
		"{COPY} ../External/dlls/*.dll %{cfg.targetdir}",
		"{COPY} ../Assets ../Build/%{cfg.targetdir}"
	}




