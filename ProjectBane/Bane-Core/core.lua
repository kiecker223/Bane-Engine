group "Core"
project "BaneCore"
    kind "StaticLib"
    includedirs { "../Bane-Common" }
    files { "Core/Containers/StackQueue.h",
            "Core/Data/Format.h",
            "Core/Systems/Logging/**.h",
            "Core/Systems/Logging/**.cpp",
            "Core/Systems/File/**.h",
            "Core/Systems/File/**.cpp",
        }

project "BaneEngine"
    kind "StaticLib"
    links {"BaneGraphicsRenderer", "BaneObject", "BaneCore"}
    includedirs { 
        "../Bane-Common", 
        "../Bane-Graphics", 
        "../Bane-Object",
        "../Bane-Math",
        "../Bane-Core", }
    files { "Engine/Application/**.h",
            "Engine/Application/**.cpp"}