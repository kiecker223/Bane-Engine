group "Game"
project "BaneGame"
    kind "StaticLib"
    links {"BaneObject", "BaneCore", "BaneGraphicsCore", "BanePhysics" }
    includedirs { 
        "../Bane-Common",
        "../Bane-Math",
        "../Bane-Object",
        "../Bane-Core",
		"../Bane-Graphics-Core",
		"../Bane-Physics",
        "../External/include"}

--  libdirs {"../External/libs/" }
--  filter "system:windows"
--      links { "d3d12", "assimp-vc140-mt", "DevIL.lib", "ILU.lib", "ILUT.lib", "dxgi", "d3dcompiler"}

    files { "Code/**.h", "Code/**.cpp"}
	excludes { "Code/RayTracingRenderer.h", "Code/RayTracingRenderer.cpp" }