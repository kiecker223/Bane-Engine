group "Engine"
project "BaneGraphicsCore"
    kind "StaticLib"
    links { "BaneCore"}
    includedirs { 
        "../Bane-Common",  
        "../Bane-Core",
        "../Bane-Object",
        "../Bane-Math",
        "../External/include",
    }
    files { "Graphics/D3D12/**.h",
            "Graphics/D3D12/**.cpp",
            "Graphics/D3DCommon/**.h",
            "Graphics/D3DCommon/**.cpp",
            "Graphics/Interfaces/**.h",
            "Graphics/Interfaces/**.cpp",
            "Graphics/IO/**.h",
            "Graphics/IO/**.cpp",
            "Graphics/Rendering/RendererInterface.h",
            "Graphics/Rendering/RendererInterface.cpp"
            }

    excludes { "Graphics/D3D12/D3D12ShaderSignature.cpp", "Graphics/Interfaces/GraphicsDevice.cpp", "Graphics/IO/ShaderTokenizer.cpp" }
            
    filter "system:windows"
        links { "d3d12"}

project "BaneGraphicsRenderer"
    kind "StaticLib"
    links {"BaneCore", "BaneGraphicsCore", "assimp-vc140-mt"}
    includedirs { 
        "../Bane-Common",  
        "../Bane-Core",
        "../Bane-Object",
        "../Bane-Core-Components",
        "../Bane-Math",
        "../Bane-Graphics",
        "../External/include",
    }
    libdirs {"../External/libs/" }
    files { "Graphics/Rendering/**.h",
            "Graphics/Rendering/**.cpp"}

    excludes { "Graphics/Rendering/RendererInterface.cpp",  "Graphics/Rendering/RendererInterface.h"}
            