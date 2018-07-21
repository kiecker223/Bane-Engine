group "App"
project "BaneShaderCompiler"
	kind "ConsoleApp"
	includedirs {
		"../Bane-Graphics-Core",
		"../Bane-Common",
		"../Bane-Math",
		"../Bane-Core",
		"../Bane-Platform",
		"../External/include"
	}

	links {
		"BaneCore",
		"BaneGraphicsCore",
		"BanePlatform",
		"d3dcompiler"
	}

	files { "**.h", "**.cpp" }
