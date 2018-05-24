#pragma once

#include "Core/Common.h"
#include <map>
#include "../Interfaces/GraphicsResources.h"

class TextureCache
{
public:

	static TextureCache* GInstance;

	void InitCache();
	void DestroyCache();

	ITexture2D* LoadTexture(const std::string& TextureName);
	ITextureCube* LoadCubemap(const std::string& TextureName);
	ITextureCube* LoadCubemap(
		const std::string& Name, 
		const std::string& NX, 
		const std::string& NY, 
		const std::string& NZ, 
		const std::string& PX, 
		const std::string& PY, 
		const std::string& PZ
	);

	void AddTexture(const std::string& TextureName, ITextureBase* Texture);
	void DestroyTexture(const std::string& TextureName);

	ITextureCube* GetActiveCubemap();
	void SetActiveCubemap(const std::string& Cubemap);

	ForceInline ITexture2D* GetDefaultWhiteTexture() const
	{
		return m_DefaultWhite;
	}

	ForceInline ITexture2D* GetDefaultBlackTexture() const
	{
		return m_DefaultBlack;
	}

	ForceInline ITexture2D* GetDefaultBlueTexture() const
	{
		return m_DefaultBlue;
	}

	ForceInline ITexture2D* GetDefaultNormal() const
	{
		return m_DefaultNormal;
	}
	
private:
	
	ITexture2D* m_DefaultBlue;
	ITexture2D* m_DefaultWhite;
	ITexture2D* m_DefaultBlack;
	ITexture2D* m_DefaultNormal;
	ITextureCube* m_ActiveCubemap;

	typedef struct LOADED_IMAGE
	{
		uint8* Buff;
		int Width, Height, Step;
	} LOADED_IMAGE;

	void InternalLoadCubemap(
		LOADED_IMAGE* OutImages, 
		const std::string& NX,
		const std::string& NY,
		const std::string& NZ,
		const std::string& PX,
		const std::string& PY,
		const std::string& PZ
	);

	SUBRESOURCE_DATA CreateTexData(LOADED_IMAGE* InImages, uint& OutWidth, uint& OutHeight, uint& OutDepth);

	byte* InternalLoadImage(const char* FileName, int& OutWidth, int& OutHeight, int& OutStep, bool bFlip = false);

	std::map<std::string, ITextureBase*> m_Textures;


};

ForceInline void InitializeTextureCache()
{
	TextureCache::GInstance = new TextureCache();
	TextureCache::GInstance->InitCache();
}

ForceInline void DestroyTextureCache()
{
	TextureCache::GInstance->DestroyCache();
	delete TextureCache::GInstance;
	TextureCache::GInstance = nullptr;
}

ForceInline TextureCache* GetTextureCache()
{
	return TextureCache::GInstance;
}
