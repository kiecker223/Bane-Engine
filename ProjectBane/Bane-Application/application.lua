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
		"BaneCore",
		"BaneCommon",
		"BanePlatform",
		"BaneGraphicsCore"
	}