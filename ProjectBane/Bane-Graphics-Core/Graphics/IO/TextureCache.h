#pragma once

#include "Common.h"
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

	inline ITexture2D* GetDefaultWhiteTexture() const
	{
		return m_DefaultWhite;
	}

	inline ITexture2D* GetDefaultBlackTexture() const
	{
		return m_DefaultBlack;
	}

	inline ITexture2D* GetDefaultBlueTexture() const
	{
		return m_DefaultBlue;
	}

	inline ITexture2D* GetDefaultNormal() const
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
		int32 Width, Height, Step;
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

	SUBRESOURCE_DATA CreateTexData(LOADED_IMAGE* InImages, uint32& OutWidth, uint32& OutHeight, uint32& OutDepth);

	uint8* InternalLoadImage(const char* FileName, int32& OutWidth, int32& OutHeight, int32& OutStep, bool bFlip = false);

	std::map<std::string, ITextureBase*> m_Textures;


};

inline void InitializeTextureCache()
{
	TextureCache::GInstance = new TextureCache();
	TextureCache::GInstance->InitCache();
}

inline void DestroyTextureCache()
{
	TextureCache::GInstance->DestroyCache();
	delete TextureCache::GInstance;
	TextureCache::GInstance = nullptr;
}

inline TextureCache* GetTextureCache()
{
	return TextureCache::GInstance;
}
