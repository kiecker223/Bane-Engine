group "Core"
project "BaneMath"
    kind "StaticLib"
    links {"BaneCore"}
    includedirs { 
        "../Bane-Common",  
        "../Bane-Core",
        "../Bane-Object",
        "../Bane-Core-Components"}
    files { "Math/**.h",
            "Math/**.cpp"}