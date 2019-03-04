group "BaneLib"
project "BaneMath"
    kind "StaticLib"
    includedirs { "../Bane-Common/" }
    links { "BaneCommon" }

    files { "**.h", "**.cpp" }