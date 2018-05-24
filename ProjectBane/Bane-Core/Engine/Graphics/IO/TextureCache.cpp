#include "TextureCache.h"
#include "../Interfaces/ApiRuntime.h"
#include <IL/il.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Core/Data/stb_image_write.h"
#include <IL/devil_cpp_wrapper.hpp>


TextureCache* TextureCache::GInstance = nullptr;

void TextureCache::InitCache()
{
	struct COL
	{
		uint8 R, G, B, A;
	};
	COL WhiteData[64 * 64];
	memset(WhiteData, 255, 64 * 64 * 4);

	IRuntimeGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();

	SUBRESOURCE_DATA Data = { };
	Data.Width = 64;
	Data.Height = 64;
	Data.Step = 4;
	Data.Subresource = 0;
	Data.Pointer = &WhiteData[0];
	m_DefaultWhite = Device->CreateTexture2D(64, 64, FORMAT_R8G8B8A8_UNORM, TEXTURE_USAGE_SHADER_RESOURCE, &Data);

	memset(WhiteData, 0, 64 * 64 * 4);
	Data.Pointer = &WhiteData[0];
	m_DefaultBlack = Device->CreateTexture2D(64, 64, FORMAT_R8G8B8A8_UNORM, TEXTURE_USAGE_SHADER_RESOURCE, &Data);

	for (uint y = 0; y < 64; y++)
	{
		for (uint x = 0; x < 64; x++)
		{
			WhiteData[x + (y * 64)] = { 0, 0, 255, 0 };
		}
	}
	m_DefaultBlue = Device->CreateTexture2D(64, 64, FORMAT_R8G8B8A8_UNORM, TEXTURE_USAGE_SHADER_RESOURCE, &Data);

	for (uint y = 0; y < 64; y++)
	{
		for (uint x = 0; x < 64; x++)
		{
			WhiteData[x + (y * 64)] = { 128, 128, 255, 0 };
		}
	}
	m_DefaultNormal = Device->CreateTexture2D(64, 64, FORMAT_R8G8B8A8_UNORM, TEXTURE_USAGE_SHADER_RESOURCE, &Data);
	Device->GenerateMips(m_DefaultWhite);
	Device->GenerateMips(m_DefaultBlack);
	Device->GenerateMips(m_DefaultBlue);
	Device->GenerateMips(m_DefaultNormal);
}

void TextureCache::DestroyCache()
{
	for (auto& i : m_Textures)
	{
		delete (i).second;
	}
}

ITexture2D* TextureCache::LoadTexture(const std::string& TextureName)
{
	ITexture2D* Result = nullptr;
	auto FoundIter = m_Textures.find(TextureName);

	if (FoundIter == m_Textures.end())
	{
		int Width, Height, Step;
		void* Buffer = InternalLoadImage(TextureName.c_str(), Width, Height, Step);
		SUBRESOURCE_DATA Data = { };
		Data.Depth = 1;
		Data.Width = Width;
		Data.Height = Height;
		Data.Pointer = Buffer;
		Data.Step = Step;
		Result = GetApiRuntime()->GetGraphicsDevice()->CreateTexture2D(Width, Height, FORMAT_R8G8B8A8_UNORM, TEXTURE_USAGE_SHADER_RESOURCE, &Data);
		GetApiRuntime()->GetGraphicsDevice()->GenerateMips(Result);
		AddTexture(TextureName, Result);
		delete[](uint8*)Buffer;
	}
	else
	{
		Result = FoundIter->second;
	}
	return Result;
}

enum TexCubeFace
{
	Left,
	Down,
	Front,
	Right,
	Up,
	Back,
};

ITextureCube* TextureCache::LoadCubemap(const std::string& Tex)
{
	return LoadCubemap(Tex, Tex + "_Left", Tex + "_Up", Tex + "_Back", Tex + "_Right", Tex + "_Up", Tex + "_Front");
}

void TextureCache::AddTexture(const std::string& TextureName, ITextureBase* Texture)
{
	m_Textures.insert(std::pair<std::string, ITextureBase*>(TextureName, Texture));
}

void TextureCache::DestroyTexture(const std::string& TextureName)
{
	auto FoundIter = m_Textures.find(TextureName);

	if (FoundIter != m_Textures.end())
	{
		delete FoundIter->second;
		m_Textures.erase(FoundIter);
	}
}


ITextureCube* TextureCache::LoadCubemap(
	const std::string& Name,
	const std::string& NX,
	const std::string& NY,
	const std::string& NZ,
	const std::string& PX,
	const std::string& PY,
	const std::string& PZ)
{
	auto FoundIter = m_Textures.find(Name);
	ITexture3D* Result;

	if (FoundIter == m_Textures.end())
	{
		LOADED_IMAGE Images[6];
		InternalLoadCubemap(&Images[0], NX, NY, NZ, PX, PY, PZ);

		uint Width, Height, Depth;
		SUBRESOURCE_DATA Data = CreateTexData(&Images[0], Width, Height, Depth);
		Result = GetApiRuntime()->GetGraphicsDevice()->CreateTextureCube(Width, FORMAT_R8G8B8A8_UNORM, TEXTURE_USAGE_SHADER_RESOURCE, &Data);
		GetApiRuntime()->GetGraphicsDevice()->GenerateMips(Result);
		delete[] (uint8*)Data.Pointer;
	}
	else
	{
		Result = FoundIter->second;
	}
	return Result;
}

ITextureCube* TextureCache::GetActiveCubemap()
{
	return m_ActiveCubemap;
}

void TextureCache::SetActiveCubemap(const std::string& Cubemap)
{
	m_ActiveCubemap = m_Textures.find(Cubemap)->second;
}

void TextureCache::InternalLoadCubemap(
	LOADED_IMAGE* OutImages,
	const std::string& NX,
	const std::string& NY,
	const std::string& NZ,
	const std::string& PX,
	const std::string& PY,
	const std::string& PZ)
{
	OutImages[0].Buff = InternalLoadImage(NX.c_str(), OutImages[0].Width, OutImages[0].Height, OutImages[0].Step, true);
	OutImages[1].Buff = InternalLoadImage(PX.c_str(), OutImages[1].Width, OutImages[1].Height, OutImages[1].Step, true);
	OutImages[2].Buff = InternalLoadImage(PY.c_str(), OutImages[2].Width, OutImages[2].Height, OutImages[2].Step, false);
	OutImages[3].Buff = InternalLoadImage(NY.c_str(), OutImages[3].Width, OutImages[3].Height, OutImages[3].Step, false);
	OutImages[4].Buff = InternalLoadImage(NZ.c_str(), OutImages[4].Width, OutImages[4].Height, OutImages[4].Step, true);
	OutImages[5].Buff = InternalLoadImage(PZ.c_str(), OutImages[5].Width, OutImages[5].Height, OutImages[5].Step, true);

	bool bEqual = true;
	for (uint i = 0; i < 6; i++)
	{
		for (uint x = 0; x < 6; x++)
		{
			bEqual = (OutImages[i].Width == OutImages[x].Width && OutImages[i].Height == OutImages[x].Height && OutImages[i].Step == OutImages[x].Step);
		}
	}
	assert(bEqual);
}

static uint GetLocation(uint X, uint Y, uint Z, uint Width, uint Depth)
{
	return (X + ((Y * Width) + (Z * Width * Depth)));
}

SUBRESOURCE_DATA TextureCache::CreateTexData(LOADED_IMAGE* InImages, uint& OutWidth, uint& OutHeight, uint& OutDepth)
{
	SUBRESOURCE_DATA Result;
	uint Width = InImages[0].Width;
	uint Height = InImages[0].Height;
	uint Step = InImages[0].Step;
	OutWidth = Width;
	OutHeight = Height;
	OutDepth = 6;
	Result.Width = Width;
	Result.Height = Height;
	Result.Depth = 6;
	Result.Subresource = 0;
	Result.Step = Step;
	uint8* Buff = new uint8[(Width * Height * 6) * Step];

	for (uint i = 0; i < 6; i++)
	{
		uint8* Dst = &Buff[(Width * Height * Step) * i];
		memcpy(Dst, InImages[i].Buff, (Width * Height * Step));
	}
	Result.Pointer = Buff;
	return Result;
}


byte* TextureCache::InternalLoadImage(const char* FileName, int& OutWidth, int& OutHeight, int& OutStep, bool bFlip)
{
	uint8* Buffer;
	ilImage Img;
	if (!Img.Load(FileName))
	{
		__debugbreak();
	}
	OutWidth = Img.Width();
	OutHeight = Img.Height();
	OutStep = Img.Bpp();
	if (bFlip)
	{
		Img.Bind();
		iluFlipImage();
	}
	if (OutStep == 3)
	{
		Img.Convert(IL_RGBA);
	}
	OutStep = Img.Bpp();
	Buffer = Img.GetData();
	byte* CopyBuff = new byte[OutWidth * OutHeight * OutStep];
	memcpy(CopyBuff, Buffer, OutWidth * OutHeight * OutStep);
	Img.Delete();
	return CopyBuff;
}
