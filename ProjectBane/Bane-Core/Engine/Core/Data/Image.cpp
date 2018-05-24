#include "Image.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


DXGI_FORMAT Image::GetImageFormatAsDXGIFormat(EIMAGE_FORMAT Format)
{
	switch (Format)
	{
	case IMAGE_FORMAT_R32:
		return DXGI_FORMAT_R32_FLOAT;
	case IMAGE_FORMAT_R32G32:
		return DXGI_FORMAT_R32G32_FLOAT;
	case IMAGE_FORMAT_R32G32B32:
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case IMAGE_FORMAT_R32G32B32A32:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case IMAGE_FORMAT_R8:
		return DXGI_FORMAT_R8_UNORM;
	case IMAGE_FORMAT_R8G8:
		return DXGI_FORMAT_R8G8_UNORM;
	case IMAGE_FORMAT_R8G8B8A8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;


	}
	return (DXGI_FORMAT)0;
}

void Image::LoadFromFile(const char* FileName, EIMAGE_FORMAT Format)
{
	m_FileName = FileName;
	int Composition;
	int Width, Height;
	void* Buffer = stbi_load(FileName, &Width, &Height, &Composition, STBI_rgb_alpha);

	m_Format = Format;

	if (Buffer == nullptr)
	{
		std::cout << "Failed to load image " << FileName << std::endl << "Failure reason: \n" << stbi_failure_reason();
		return;
	}

	LoadFromMemory(Buffer, Format, Width, Height);
	STBI_FREE(Buffer);
}

void Image::LoadFromMemory(void* pBuffer, EIMAGE_FORMAT Format, uint Width, uint Height)
{
	m_Format = Format;
	m_Width = Width;
	m_Height = Height;
	uint ImageSizeInBytes = GetImageSizeInBytes();

	if (m_Buffer != nullptr)
	{
		HeapFree(GetProcessHeap(), 0, m_Buffer);
		m_Buffer = nullptr;
	}

	m_Buffer = HeapAlloc(GetProcessHeap(), 0, ImageSizeInBytes);

	memcpy(m_Buffer, pBuffer, ImageSizeInBytes);
}

void Image::InitializeImage(uint Width, uint Height, EIMAGE_FORMAT Format)
{
	m_Width = Width;
	m_Height = Height;
	m_Format = Format;
	m_Buffer = malloc(GetImageSizeInBytes());
}

void Image::CopyFromOther(const Image* pImage)
{
	m_Width = pImage->GetWidth();
	m_Height = pImage->GetHeight();
	m_Format = pImage->GetFormat();

	uint SizeInBytes = pImage->GetImageSizeInBytes();

	if (m_Buffer)
		free(m_Buffer);

	memcpy(m_Buffer, pImage->GetBuffer(), SizeInBytes);
}

//DXGI_FORMAT Image::GetFormat() const
//{
//	return GetImageFormatAsDXGIFormat(m_Format);
//}

uint Image::GetByteWidthFromImageFormat(EIMAGE_FORMAT InFormat)
{
	if ((uint)InFormat <= 4)
		return (uint)InFormat;
	else if ((uint)InFormat >= 5)
		return (uint)InFormat - 1;
	return 0;
}

ColorRGBA8& Image::GetPixelAtRGBA(uint x, uint y)
{
	uint MemPos = (x + (y * m_Height));

	ColorRGBA8* Buff = (ColorRGBA8*)m_Buffer;
	return Buff[MemPos];
}

ColorRGB8& Image::GetPixelAtRGB(uint x, uint y)
{
	uint MemPos = (x + (y * m_Height));

	byte* Pointer = (byte*)m_Buffer;

	Pointer += MemPos * 3;
	return *((ColorRGB8*)Pointer);
}

