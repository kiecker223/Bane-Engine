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
		"../Bane-Object"
	}
	links {
		"BaneCore",
		"BaneCommon",
		"BanePlatform",
		"BaneGraphicsCore"
	}