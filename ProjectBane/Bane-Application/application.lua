group "Engine"
    project "BaneApplicaton"
	kind "StaticLib"
	files {
		"**.h", "**.cpp"
	}
	includedirs {
		"../Bane-Common/",
		"../Bane-Core",
		"../Bane-Platform",
		"../Bane-Graphics-Core",
		"../Bane-Rendering",
		"../Bane-Object",
		"../Bane-Platform/Platform",
		"../Bane-Core-Components",
		"../Bane-Math"

	}
	links {
		"BaneObject",
		"BaneCore",
		"BaneCommon",
		"BanePlatform",
		"BaneGraphicsCore",
		"BaneGraphicsRenderer"
	}