group "BaneLib"
project "BanePlatform"
    kind "StaticLib"
    includedirs { "../External/include/", "../Bane-Common/", "../Bane-Math/", "../Bane-Core/" }
    links {
        "BaneCore",
		"dinput8",
		"dxguid"
    }

    files
    {
        "**.h", "**.cpp"
    }