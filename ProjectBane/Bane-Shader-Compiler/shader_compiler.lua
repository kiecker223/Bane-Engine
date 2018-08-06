group "App"
project "BaneShaderCompiler"
	kind "ConsoleApp"
	includedirs {
		"../Bane-Graphics-Core",
		"../Bane-Common",
		"../Bane-Math",
		"../Bane-Core",
		"../Bane-Platform",
		"../Bane-PE-Loader",
		"../External/include"
	}

	links {
		"BaneCore",
		"BaneGraphicsCore",
		"BanePlatform",
		"BanePELoader",
		"d3dcompiler"
	}

	files { "**.h", "**.cpp" }
