group "Graphics"
project "BaneGraphicsCore"
    kind "StaticLib"
    links { "BaneCore", "BanePlatform"}
    includedirs { 
        "../Bane-Common",  
        "../Bane-Core",
        "../Bane-Math",
        "../Bane-Platform",
        "../External/include",
    }
    files { "**.h", "**.cpp" }

    excludes { "Graphics/D3D12/D3D12ShaderSignature.cpp", "Graphics/Interfaces/GraphicsDevice.cpp", "Graphics/IO/ShaderTokenizer.cpp" }
            
--    filter "system:windows"
--        links { "d3d12"}

