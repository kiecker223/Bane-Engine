group "App"
project "BaneExecutable"
    kind "ConsoleApp"
    links { "BaneObject", "BaneCore", "BaneGraphicsCore", "BaneApplicaton", "BaneGraphicsRenderer", "BaneCoreComponents"}
    includedirs {
        "../Bane-Common",
        "../Bane-Math",
        "../Bane-Object",
        "../Bane-Core",
        "../Bane-Graphics-Core",
		"../Bane-Rendering",
        "../External/include",
		"../Bane-Platform/Platform",
		"../Bane-Application"
    }
	characterset "ASCII"
    libdirs {"../External/libs/"}
    filter "system:windows"
        links {"d3d12", "assimp-vc140-mt", "DevIL.lib", "ILU.lib", "dxgi", "d3dcompiler", }
        includedirs {"../Bane-Systems/Platform-Windows/"}
    files {"**.h", "**.cpp"}



