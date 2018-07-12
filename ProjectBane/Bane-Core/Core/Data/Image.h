#pragma once

#include "Core/Common.h"
#include "Core/Data/Color.h"
#include "dxgiformat.h"


typedef enum EIMAGE_FORMAT
{
	IMAGE_FORMAT_R8 = 1,
	IMAGE_FORMAT_R8G8 = 2,
	IMAGE_FORMAT_R8G8B8A8 = 4,
	IMAGE_FORMAT_R32 = (4 * 1) + 1,
	IMAGE_FORMAT_R32G32 = (4 * 2) + 1,
	IMAGE_FORMAT_R32G32B32 = (4 * 3) + 1,
	IMAGE_FORMAT_R32G32B32A32 = (4 * 4) + 1
} EIMAGE_FORMAT;

class Image
{
public:

	static DXGI_FORMAT GetImageFormatAsDXGIFormat(EIMAGE_FORMAT Format);

	Image() : m_Buffer(nullptr), m_Width(0), m_Height(0) { }

	inline const EIMAGE_FORMAT GetFormat() const { return m_Format; }
	inline void* GetBuffer() const { return m_Buffer; }
	inline const uint GetWidth() const { return m_Width; }
	inline const uint GetHeight() const { return m_Height; }

	inline void FreeBuffer() 
	{ 
		//if (m_Buffer != nullptr) 
		//{ 
		//	HeapFree(GetProcessHeap(), 0, m_Buffer); 
		//	m_Buffer = nullptr;
		//} 
		m_Width = 0;
		m_Height = 0;
	}

	void LoadFromFile(const char* FileName, EIMAGE_FORMAT Format);
	void LoadFromMemory(void* pBuffer, EIMAGE_FORMAT Format, uint Width, uint Height);
	void InitializeImage(uint Width, uint Height, EIMAGE_FORMAT Format);
	void CopyFromOther(const Image* pImage);

	//EFORMAT GetFormat() const;
	
	inline uint GetImageSizeInBytes() const { return (m_Width * m_Height) * GetByteWidthFromImageFormat(m_Format); }
	static uint GetByteWidthFromImageFormat(EIMAGE_FORMAT InFormat);

	ColorRGBA8& GetPixelAtRGBA(uint x, uint y);
	ColorRGB8& GetPixelAtRGB(uint x, uint y);

private:

	const char* m_FileName;
	EIMAGE_FORMAT m_Format;
	void* m_Buffer;
	uint m_Width;
	uint m_Height;

};

