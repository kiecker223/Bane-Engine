group "App"
project "BaneIrradianceGenerator"
    kind "ConsoleApp"
    characterset "Default"
    links {"DevIL.lib", "ILU.lib", "ILUT.lib"}
    includedirs { 
        "../Bane-Common",  
        "../Bane-Math",
        "../External/include"}

    libdirs {"../External/libs/" }

    files { "Bane-Irradiance-Generator.cpp"}
    
    postbuildcommands {
        "{COPY} ../External/dlls/*.dll %{cfg.targetdir}"
    }  