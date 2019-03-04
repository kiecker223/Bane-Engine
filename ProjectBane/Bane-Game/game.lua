group "Game"
project "BaneGame"
    kind "ConsoleApp"
    links {"BaneCore", "BanePlatform", "BaneGraphicsCore" }
    includedirs { "../Bane-Graphics-Core/", "../Bane-Math/", "../Bane-Platform/", "../Bane-Common/", "../Bane-Core/", "../External/include", "Code/" }

    libdirs { "../External/libs/" }
    filter "system:windows"
        links { "d3d12", "assimp-vc140-mt", "DevIL.lib", "ILU.lib", "ILUT.lib", "dxgi", "d3dcompiler", "dxguid" }

    files { "Code/**.h", "Code/**.cpp" }

	
	debugdir "../Build/Debug/"

	dependson { "BaneShaderCompiler" }

	postbuildcommands {
		"{COPY} ../External/dlls/*.dll %{cfg.targetdir}",
		"{COPY} ../Assets ../Build/%{cfg.targetdir}"
	}