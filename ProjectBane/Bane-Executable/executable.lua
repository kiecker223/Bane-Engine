group "App"
project "BaneExecutable"
    kind "ConsoleApp"
    links { "BaneObject", "BaneCore", "BaneGraphicsCore", "BaneEngine", "BanePlatform" }
    includedirs {
        "../Bane-Common",
        "../Bane-Math",
        "../Bane-Object",
        "../Bane-Core",
        "../Bane-Graphics",
		"../Bane-Systems",
        "../External/include"
    }
	characterset "ASCII"
    libdirs {"../External/libs/"}
    filter "system:windows"
        links {"d3d12", "assimp-vc140-mt", "DevIL.lib", "ILU.lib", "dxgi", "d3dcompiler", }
        includedirs {"../Bane-Systems/Platform-Windows/"}
    files {"Executable/**.h", "Executable/**.cpp"}



