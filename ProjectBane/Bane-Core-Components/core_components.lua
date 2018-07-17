group "Engine"
project "BaneCoreComponents"
    kind "StaticLib"
	 includedirs { 
        "../Bane-Common",  
        "../Bane-Core",
        "../Bane-Object",
        "../Bane-Math",
        "../Bane-Platform/Platform",
		"../Bane-Rendering",
		"../Bane-Graphics-Core",
        "../Bane-Application",
        "../External/include",
    }
    files {
        "**.h",
        "**.cpp"
    }