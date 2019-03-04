group "BaneLib"
project "BaneGraphicsCore"
    kind "StaticLib"
    includedirs {
		"../Bane-Common",
        "../Bane-Math",
        "../Bane-Core/",
		"../Bane-Platform",
		"../External/include/"
    }

    links
    {
        "BaneCore",
        "BanePlatform"
    }

    files
    {
        "**.h", "**.cpp"
    }