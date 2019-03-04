group "BaneLib"
project "BaneCore"
    kind "StaticLib"
    includedirs{
        "../Bane-Common/",
        "../Bane-Math/"
    }

    files
    {
        "**.h", "**.cpp"
    }

