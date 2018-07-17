group "Graphics"
project "BaneGraphicsCore"
    kind "StaticLib"
    links { "BaneCore", "BanePlatform"}
    includedirs { 
        "../Bane-Common",  
        "../Bane-Core",
        "../Bane-Object",
        "../Bane-Math",
        "../Bane-Platform/Platform",
        "../Bane-Application",
        "../External/include",
    }
    files { "Graphics/D3D12/**.h",
            "Graphics/D3D12/**.cpp",
            "Graphics/D3DCommon/**.h",
            "Graphics/D3DCommon/**.cpp",
            "Graphics/Interfaces/**.h",
            "Graphics/Interfaces/**.cpp",
            "Graphics/IO/**.h",
            "Graphics/IO/**.cpp"
            }

    excludes { "Graphics/D3D12/D3D12ShaderSignature.cpp", "Graphics/Interfaces/GraphicsDevice.cpp", "Graphics/IO/ShaderTokenizer.cpp" }
            
    filter "system:windows"
        links { "d3d12"}

