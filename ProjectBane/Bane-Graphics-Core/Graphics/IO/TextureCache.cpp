#include "TextureCache.h"
#include "../Interfaces/ApiRuntime.h"
#include <IL/il.h>
#include <IL/devil_cpp_wrapper.hpp>

TextureCache* TextureCache::GInstance = nullptr;

void TextureCache::InitCache()
{
	struct COL
	{
		uint8 R, G, B, A;
	};
	COL ImgData[64 * 64];
	memset(ImgData, 255, 64 * 64 * 4);

	IRuntimeGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();

	SUBRESOURCE_DATA Data = { };
	Data.Width = 64;
	Data.Height = 64;
	Data.Step = 4;
	Data.Subresource = 0;
	Data.Pointer = &ImgData[0];
	SAMPLER_DESC DefaultSampler = CreateDefaultSamplerDesc();
	m_DefaultWhite = Device->CreateTexture2D(64, 64, FORMAT_R8G8B8A8_UNORM, DefaultSampler, TEXTURE_USAGE_SHADER_RESOURCE, &Data);

	memset(ImgData, 0, 64 * 64 * 4);
	Data.Pointer = &ImgData[0];
	m_DefaultBlack = Device->CreateTexture2D(64, 64, FORMAT_R8G8B8A8_UNORM, DefaultSampler, TEXTURE_USAGE_SHADER_RESOURCE, &Data);

	for (uint32 y = 0; y < 64; y++)
	{
		for (uint32 x = 0; x < 64; x++)
		{
			ImgData[x + (y * 64)] = { 0, 0, 255, 0 };
		}
	}
	m_DefaultBlue = Device->CreateTexture2D(64, 64, FORMAT_R8G8B8A8_UNORM, DefaultSampler, TEXTURE_USAGE_SHADER_RESOURCE, &Data);

	for (uint32 y = 0; y < 64; y++)
	{
		for (uint32 x = 0; x < 64; x++)
		{
			ImgData[x + (y * 64)] = { 128, 128, 255, 0 };
		}
	}
	m_DefaultNormal = Device->CreateTexture2D(64, 64, FORMAT_R8G8B8A8_UNORM, DefaultSampler, TEXTURE_USAGE_SHADER_RESOURCE, &Data);
	Device->GenerateMips(m_DefaultWhite);
	Device->GenerateMips(m_DefaultBlack);
	Device->GenerateMips(m_DefaultBlue);
	Device->GenerateMips(m_DefaultNormal);
	AddTexture("DefaultWhite", m_DefaultWhite);
	AddTexture("DefaultBlack", m_DefaultBlack);
	AddTexture("DefaultBlue", m_DefaultBlue);
	AddTexture("DefaultNormal", m_DefaultNormal);
}

void TextureCache::DestroyCache()
{
	for (auto& i : m_Textures)
	{
		delete (i).second;
	}
}

ITexture2D* TextureCache::LoadTexture(const std::string& TextureName, const SAMPLER_DESC* pSampler)
{
	ITexture2D* Result = nullptr;
	auto FoundIter = m_Textures.find(TextureName);

	SAMPLER_DESC SamplerToUse = pSampler ? *pSampler : CreateDefaultSamplerDesc();

	if (FoundIter == m_Textures.end())
	{
		int32 Width, Height, Step;
		void* Buffer = InternalLoadImage(TextureName.c_str(), Width, Height, Step);
		SUBRESOURCE_DATA Data = { };
		Data.Depth = 1;
		Data.Width = Width;
		Data.Height = Height;
		Data.Pointer = Buffer;
		Data.Step = Step;
		Result = GetApiRuntime()->GetGraphicsDevice()->CreateTexture2D(Width, Height, FORMAT_R8G8B8A8_UNORM, SamplerToUse, TEXTURE_USAGE_SHADER_RESOURCE, &Data);
		Result->SetDebugName(TextureName);
		GetApiRuntime()->GetGraphicsDevice()->GenerateMips(Result);
		AddTexture(TextureName, Result);
		delete[] (uint8*)Buffer;
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

ITextureCube* TextureCache::LoadCubemap(const std::string& Tex, const SAMPLER_DESC* pSampler)
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
	const std::string& PZ,
	const SAMPLER_DESC* pSampler)
{
	auto FoundIter = m_Textures.find(Name);
	ITexture3D* Result;

	SAMPLER_DESC SamplerToUse = pSampler ? *pSampler : CreateDefaultSamplerDesc();

	if (FoundIter == m_Textures.end())
	{
		LOADED_IMAGE Images[6];
		InternalLoadCubemap(&Images[0], NX, NY, NZ, PX, PY, PZ);

		uint32 Width, Height, Depth;
		SUBRESOURCE_DATA Data = CreateTexData(&Images[0], Width, Height, Depth);
		Result = GetApiRuntime()->GetGraphicsDevice()->CreateTextureCube(Width, FORMAT_R8G8B8A8_UNORM, SamplerToUse, TEXTURE_USAGE_SHADER_RESOURCE, &Data);
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
	OutImages[0].Buff = InternalLoadImage(NX.c_str(), OutImages[0].Width, OutImages[0].Height, OutImages[0].Step,  true);
	OutImages[1].Buff = InternalLoadImage(PX.c_str(), OutImages[1].Width, OutImages[1].Height, OutImages[1].Step,  true);
	OutImages[2].Buff = InternalLoadImage(PY.c_str(), OutImages[2].Width, OutImages[2].Height, OutImages[2].Step, false);
	OutImages[3].Buff = InternalLoadImage(NY.c_str(), OutImages[3].Width, OutImages[3].Height, OutImages[3].Step, false);
	OutImages[4].Buff = InternalLoadImage(NZ.c_str(), OutImages[4].Width, OutImages[4].Height, OutImages[4].Step,  true);
	OutImages[5].Buff = InternalLoadImage(PZ.c_str(), OutImages[5].Width, OutImages[5].Height, OutImages[5].Step,  true);

	bool bEqual = true;
	for (uint32 i = 0; i < 6; i++)
	{
		for (uint32 x = 0; x < 6; x++)
		{
			bEqual = (OutImages[i].Width == OutImages[x].Width && OutImages[i].Height == OutImages[x].Height && OutImages[i].Step == OutImages[x].Step);
			BANE_CHECK(bEqual);
		}
	}
	//assert(bEqual);
}

// static uint32 GetLocation(uint32 X, uint32 Y, uint32 Z, uint32 Width, uint32 Depth)
// {
// 	return (X + ((Y * Width) + (Z * Width * Depth)));
// }

SUBRESOURCE_DATA TextureCache::CreateTexData(LOADED_IMAGE* InImages, uint32& OutWidth, uint32& OutHeight, uint32& OutDepth)
{
	SUBRESOURCE_DATA Result;
	uint32 Width = InImages[0].Width;
	uint32 Height = InImages[0].Height;
	uint32 Step = InImages[0].Step;
	OutWidth = Width;
	OutHeight = Height;
	OutDepth = 6;
	Result.Width = Width;
	Result.Height = Height;
	Result.Depth = 6;
	Result.Subresource = 0;
	Result.Step = Step;
	uint8* Buff = new uint8[(Width * Height * 6) * Step];

	for (uint32 i = 0; i < 6; i++)
	{
		uint8* Dst = &Buff[(Width * Height * Step) * i];
		memcpy(Dst, InImages[i].Buff, (Width * Height * Step));
	}
	Result.Pointer = Buff;
	return Result;
}


uint8* TextureCache::InternalLoadImage(const char* FileName, int32& OutWidth, int32& OutHeight, int32& OutStep, bool bFlip)
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
