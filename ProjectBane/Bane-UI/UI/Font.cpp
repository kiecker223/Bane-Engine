#include "Font.h"
#include <JSON/JsonCPP.h>
#include <fstream>
#include <Graphics/IO/TextureCache.h>
#include <Graphics/IO/ShaderCache.h>

using namespace nlohmann;

void Font::LoadFont(const std::string& FontName)
{
	m_Name = FontName;
	
	m_Characters.resize(128);

	IB = ApiRuntime::Get()->QuadIB;
	uint32 ImgWidth = 0;
	uint32 ImgHeight = 0;
	uint32 ByteStep = 0;
	uint8* SDFImageBuff = nullptr;
	std::string ImageName;
	{
		std::ifstream FileReadStream("FontInfo.json");
		json BaseJson = json::parse(FileReadStream);
		json FontJson = BaseJson[FontName];
		ImageName = BaseJson["TextureLocation"].get<std::string>();
		SDFImageBuff = GetTextureCache()->LoadRawBytes(ImageName, ImgWidth, ImgHeight, ByteStep, true);
		for (uint32 i = 0; i < 127; i++)
		{
			BANE_CHECK(FontJson.is_array());
			json FontData = FontJson[i];
			float StartX =		static_cast<float>(FontData["Location"]["x"].get<uint32>());
			float StartY =		static_cast<float>(FontData["Location"]["y"].get<uint32>());
			float FontAdvance = static_cast<float>(FontData["Advance"].get<uint32>());
			float BearingX =	static_cast<float>(FontData["Bearing"]["x"].get<uint32>());
			float BearingY =	static_cast<float>(FontData["Bearing"]["y"].get<uint32>());
			float DimensionX =	static_cast<float>(FontData["Dimensions"]["x"].get<uint32>());
			float DimensionY =	static_cast<float>(FontData["Dimensions"]["y"].get<uint32>());
			float fImgW = static_cast<float>(ImgWidth);
			float fImgH = static_cast<float>(ImgHeight);

			float HalfDimX = DimensionX / 2.f;
			float HalfDimY = DimensionY / 2.f;

			float2 Start((StartX - BearingX) / fImgW, StartY / fImgH);
			float2 End((StartX + (FontAdvance / 64.f)) / fImgW, (StartY - (DimensionY + BearingY)) / fImgH);
			m_Characters[i].Data[0].Position = float2(-HalfDimX, HalfDimY);
			m_Characters[i].Data[0].UV = float2(Start.x, Start.y);
			m_Characters[i].Data[1].Position = float2(HalfDimX, HalfDimY);
			m_Characters[i].Data[1].UV = float2(End.x, Start.y);
			m_Characters[i].Data[2].Position = float2(-HalfDimX, -HalfDimY);
			m_Characters[i].Data[2].UV = float2(Start.x, End.y);
			m_Characters[i].Data[3].Position = float2(HalfDimX, -HalfDimY);
			m_Characters[i].Data[3].UV = float2(End.x, End.y);
		}
	}

	uint8* ImageData = new uint8[ImgWidth * ImgHeight];
	struct Color
	{
		uint8 r, g, b, a;
	};
	Color* ImageColors = reinterpret_cast<Color*>(SDFImageBuff);
	for (uint32 y = 0; y < ImgHeight; y++)
	{
		for (uint32 x = 0; x < ImgWidth; x++)
		{
			ImageData[(y * ImgWidth) + x] = ImageColors[(y * ImgWidth) + x].r;
		}
	}
	delete SDFImageBuff;
	auto* Device = GetApiRuntime()->GetGraphicsDevice();
	SUBRESOURCE_DATA Data = { };
	Data.Width = ImgWidth;
	Data.Height = ImgHeight;
	Data.Depth = 1;
	Data.Step = 1;
	Data.Pointer = reinterpret_cast<void*>(ImageData);
	FontTexture = Device->CreateTexture2D(ImgWidth, ImgHeight, FORMAT_R8_UNORM, CreateDefaultSamplerDesc(), TEXTURE_USAGE_SHADER_RESOURCE, &Data);
	FontDataStructuredBuff = Device->CreateStructuredBuffer(static_cast<uint32>(sizeof(float4) * 2 * m_Characters.size()), reinterpret_cast<uint8*>(m_Characters.data()));
	FontShader = GetShaderCache()->LoadGraphicsPipeline("FontShader.gfx");
	IB = GetApiRuntime()->QuadIB;
	VB = Device->CreateVertexBuffer(sizeof(FONT_VERTEX) * static_cast<uint32>(m_Characters.size()), reinterpret_cast<uint8*>(m_Characters.data()));
}

//void Font::InitializeGPUMemory(CHARACTER_CBUFFER_DATA* pCbuffData)
//{
//
//}
