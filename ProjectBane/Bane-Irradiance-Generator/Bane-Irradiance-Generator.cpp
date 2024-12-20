
#include <thread>
#include <string.h>
#include <random>
#include <stdio.h>
#include <tchar.h>
#include <Common.h>
#include "BaneMath.h"
#include <iostream>
#include <Windows.h>
#include "IL/il.h"
#include "IL/devil_cpp_wrapper.hpp"

// Usage
//     Bane-Irradiance-Generator.exe

//#pragma disable(warning:4201)

enum GeneratorDirection
{
	PosX,
	NegX,
	PosY,
	NegY,
	PosZ,
	NegZ,
	NUM
};

union Color
{
	Color() : Mask(0) { }
	Color(byte R, byte G, byte B, byte A) : r(R), g(G), b(B), a(A) { }
	struct { byte r, g, b, a; };
	uint32 Mask;
};

struct IntCol
{
	IntCol() : r(0), g(0), b(0), a(0) { }
	IntCol(uint32 R, uint32 G, uint32 B, uint32 A) : r(static_cast<uint32>(R)), g(static_cast<uint32>(G)), b(static_cast<uint32>(B)), a(static_cast<uint32>(A)) { }
	IntCol(const Color& C) : r(C.r), g(C.g), b(C.b), a(C.a) { }
	uint32 r, g, b, a;
};

IntCol operator + (const IntCol& Lhs, const IntCol& Rhs)
{
	return IntCol(Lhs.r + Rhs.r, Lhs.g + Rhs.g, Lhs.b + Rhs.b, Lhs.a + Rhs.a);
}

IntCol operator / (const IntCol& Lhs, uint32 Rhs)
{
	return IntCol(Lhs.r / Rhs, Lhs.g / Rhs, Lhs.b / Rhs, Lhs.a / Rhs);
}

Color operator / (const Color& Lhs, uint32 Rhs)
{
	return Color (static_cast<byte>(Lhs.r / Rhs), static_cast<byte>(Lhs.g / Rhs), static_cast<byte>(Lhs.b / Rhs), static_cast<byte>(Lhs.a / Rhs));
}

Color operator * (const Color& Lhs, float Rhs)
{
	return Color(
		static_cast<byte>(float(Lhs.r) * Rhs),
		static_cast<byte>(float(Lhs.g) * Rhs),
		static_cast<byte>(float(Lhs.b) * Rhs),
		static_cast<byte>(float(Lhs.a) * Rhs));
}

IntCol operator * (const IntCol& Lhs, float Rhs)
{
	const float R = (float(Lhs.r) * Rhs);
	const float G = (float(Lhs.g) * Rhs);
	const float B = (float(Lhs.b) * Rhs);
	const float A = (float(Lhs.a) * Rhs);
	return IntCol(
		static_cast<uint32>(R),
		static_cast<uint32>(G),
		static_cast<uint32>(B),
		static_cast<uint32>(A)
	);
}

IntCol operator / (const IntCol& Lhs, float Rhs)
{
	const float R = (float(Lhs.r) / Rhs);
	const float G = (float(Lhs.g) / Rhs);
	const float B = (float(Lhs.b) / Rhs);
	const float A = (float(Lhs.a) / Rhs);
	return IntCol(
		static_cast<uint32>(R),
		static_cast<uint32>(G),
		static_cast<uint32>(B),
		static_cast<uint32>(A)
	);
}

IntCol& operator += (IntCol& Lhs, const Color& Rhs)
{
	Lhs.r += static_cast<uint32>(Rhs.r);
	Lhs.g += static_cast<uint32>(Rhs.g);
	Lhs.b += static_cast<uint32>(Rhs.b);
	Lhs.a += static_cast<uint32>(Rhs.a);
	return Lhs;
}

Color FindAverage(IntCol c, uint32 D)
{
	return Color(static_cast<byte>(c.r / D), static_cast<byte>(c.g / D), static_cast<byte>(c.b / D), 255);
	//return Color(c.r, c.g, c.b, 255);
}

fvec3 ColorToFloat3(const Color& InCol)
{
	const float R = (float(InCol.r) / 255.f);
	const float G = (float(InCol.g) / 255.f);
	const float B = (float(InCol.b) / 255.f);
	return fvec3(
		R, G, B
	);
}

byte ClampToMaxByte(uint32 InByte)
{
	if (InByte > 255)
	{
		return 255;
	}
	return static_cast<byte>(InByte);
}

Color Float3ToColor(const fvec3& InCol)
{
	const byte R = ClampToMaxByte(static_cast<uint32>((InCol.r * 255.f)));
	const byte G = ClampToMaxByte(static_cast<uint32>((InCol.g * 255.f)));
	const byte B = ClampToMaxByte(static_cast<uint32>((InCol.b * 255.f)));
	return Color(
		R, G, B, 255
	);
}

struct ImageSide
{
	uvec2 Dimensions;
	Color* ColorBuffer;
};

struct Image
{
	ImageSide Sides[6];
};


struct PixelNormal
{
	fvec3 Normal;
	Quaternion Rotation;
};

bool NeedsFlip(GeneratorDirection Dir)
{
	switch (Dir)
	{
		case GeneratorDirection::NegY: return true;
	}
	return false;
}

struct ImageHandle
{
	enum OpenType
	{
		Read, Write
	};

	const wchar_t** Names;
	OpenType Type;
	Image Img;
	
	~ImageHandle()
	{
		
	}

	void OpenRead(const wchar_t** InNames)
	{
		Names = InNames;
		Type = Read;

		for (uint32 i = 0; i < 6; i++)
		{
			const wchar_t* Name = Names[i];
			ilImage LoadedImage;
			if (!LoadedImage.Load(Name))
			{
				std::wcout << L"Failed to load image: " << Name << std::endl;
				std::wcin.get();
				abort();
			}
			ILenum Origin = LoadedImage.GetOrigin();
			if (Origin == IL_ORIGIN_UPPER_LEFT && !NeedsFlip(static_cast<GeneratorDirection>(i)))
			{
				std::wcout << "==========================\nFlipping image\n==========================" << std::endl;
				LoadedImage.Bind();
				iluFlipImage();
			}
			if (i == static_cast<uint32>(GeneratorDirection::PosY) || i == static_cast<uint32>(GeneratorDirection::NegY))
			{
				LoadedImage.Bind();
				iluMirror();
			}
			if (LoadedImage.Bpp() < 4)
			{
				LoadedImage.Convert(IL_RGBA);
			}
			Color* ColorBuffer = reinterpret_cast<Color*>(LoadedImage.GetData());
			Img.Sides[i].Dimensions.x = LoadedImage.Width();
			Img.Sides[i].Dimensions.y = LoadedImage.Height();
			Img.Sides[i].ColorBuffer = new Color[(Img.Sides[i].Dimensions.x * Img.Sides[i].Dimensions.y)];
			memcpy(Img.Sides[i].ColorBuffer, ColorBuffer, (Img.Sides[i].Dimensions.x * Img.Sides[i].Dimensions.y) * sizeof(Color));
			LoadedImage.Close();
		}
	}

	void OpenWrite(const wchar_t** InNames, ImageHandle InHandle)
	{
		Names = InNames;
		Type = Write;
		for (uint32 i = 0; i < 6; i++)
		{
			Img.Sides[i].Dimensions = InHandle.Img.Sides[i].Dimensions;
			Img.Sides[i].ColorBuffer = new Color[InHandle.Img.Sides[i].Dimensions.x * InHandle.Img.Sides[i].Dimensions.y];
		}
	}

	void WriteToFile()
	{
		for (uint32 i = 0; i < 6; i++)
		{
			const wchar_t* Name = Names[i];
			uint32 Width  = Img.Sides[i].Dimensions.x;
			uint32 Height = Img.Sides[i].Dimensions.y;
			ilImage LoadedImg;
			LoadedImg.TexImage(Width, Height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, static_cast<void*>(Img.Sides[i].ColorBuffer));
			ilEnable(IL_FILE_OVERWRITE);
			//uint322 FourthSize = uint322(LoadedImg.Width(), LoadedImg.Height());
			//std::wcout << L"Original size: X: " << FourthSize.x << L" Y: " << FourthSize.y << std::endl;
			//FourthSize /= 2;
			//FourthSize.x = NextPowerOfTwo(FourthSize.x);
			//FourthSize.y = NextPowerOfTwo(FourthSize.y);
			//LoadedImg.Resize(FourthSize.x, FourthSize.y, 1);
			//std::wcout << L"New Size: X: " << LoadedImg.Width() << L" Y: " << LoadedImg.Height() << std::endl;
			//iluBlurAvg(2);
			LoadedImg.Save(Name);
			LoadedImg.Close();
			delete[] Img.Sides[i].ColorBuffer;
		}
	}
};

PixelNormal GenPixelNormal(GeneratorDirection Direction, const ImageSide& Img, const uvec2& Position)
{
	fvec3 Result(0.f, 0.f, -1.f);
	Quaternion Rot;
	float PosOfX = ((static_cast<float>(Position.x) / static_cast<float>(Img.Dimensions.x)) * 90.f);
	float PosOfY = ((static_cast<float>(Position.y) / static_cast<float>(Img.Dimensions.y)) * 90.f);

	switch (Direction)
	{
		case PosX: 
		{
			Rot = Quaternion(fvec3(PosOfY - 45.f, PosOfX + 45.f, 0.f));
		} break;
		case NegX: 
		{
			Rot = Quaternion(fvec3(PosOfY - 45.f, PosOfX + 225.f, 0.f));
		} break;
		case PosY: 
		{
			Rot = Quaternion::FromAxisAngle(fvec3(1.f, 0.f, 0.f), radians(PosOfY + 45.f)) * Quaternion::FromAxisAngle(fvec3(0.f, 0.f, 1.f), radians(PosOfY - 45.f));
		} break;
		case NegY: 
		{
			Rot = Quaternion::FromAxisAngle(fvec3(1.f, 0.f, 0.f), radians(PosOfY - 135.f)) * Quaternion::FromAxisAngle(fvec3(0.f, 0.f, 1.f), radians(PosOfY - 45.f));
		} break;
		case PosZ: 
		{
			Rot = Quaternion(fvec3(PosOfY - 45.f, PosOfX - 45.f, 0.f));
		} break;
		case NegZ:
		{
			Rot = Quaternion(fvec3(-PosOfY + 225.f, -PosOfX + 225.f, 0.f));
		} break;
	}

	Result = (fmat3x3)Rot.RotationMatrix() * Result;
	return { Result, Rot };
}

static const fvec3 UpDir(0.f, 1.f, 0.f);
fvec3 GetRightVector(const PixelNormal& Norm)
{
	return cross(Norm.Normal, UpDir);
}

fvec3 GetUpVector(const PixelNormal& Norm)
{
	return cross(cross(normalized(Norm.Normal), UpDir), normalized(Norm.Normal));
}

Color PixelFromNormal(const fvec3& InNorm, const Image& Img)
{
	GeneratorDirection Direction = GeneratorDirection::NUM;
	fvec3 Norm = normalized(fvec3(abs(InNorm.x), abs(InNorm.y), abs(InNorm.z)));
	float BiggestAbsValue = 0.f;
	enum EBiggestAbsUnitType
	{
		UnitX,
		UnitY,
		UnitZ 
	} BiggestAbsUnitType;

	if (Norm.x > Norm.y && Norm.x > Norm.z)
	{
		if (InNorm.x < 0.f)
		{
			Direction = GeneratorDirection::NegX;
		}
		else
		{
			Direction = GeneratorDirection::PosX;
		}
		BiggestAbsValue = Norm.x;
		BiggestAbsUnitType = UnitX;
	}
	else if (Norm.y > Norm.x && Norm.y > Norm.z)
	{
		if (InNorm.y < 0.f)
		{
			Direction = GeneratorDirection::NegY;
		}
		else
		{
			Direction = GeneratorDirection::PosY;
		}
		BiggestAbsValue = Norm.y;
		BiggestAbsUnitType = UnitY;
	}
	else if (Norm.z > Norm.x && Norm.z > Norm.y)
	{
		if (InNorm.z < 0.f)
		{
			Direction = GeneratorDirection::NegZ;
		}
		else
		{
			Direction = GeneratorDirection::PosZ;
		}
		BiggestAbsValue = Norm.z;
		BiggestAbsUnitType = UnitZ;
	}
	else
	{
		return Color(128, 128, 128, 255);
	}

	fvec2 Uv;
	switch (BiggestAbsUnitType)
	{
		case UnitX: {
			Uv.x = InNorm.z / BiggestAbsValue;
			Uv.y = InNorm.y / BiggestAbsValue;
		} break;
		case UnitY: {
			Uv.x = InNorm.x / BiggestAbsValue;
			Uv.y = InNorm.z / BiggestAbsValue;
		} break;
		case UnitZ: {
			Uv.x = InNorm.x / BiggestAbsValue;
			Uv.y = InNorm.y / BiggestAbsValue;
		} break;
	}

	Uv = (Uv * 0.5f) + 0.5f;
	
	uint32 PositionX = static_cast<uint32>(Uv.x * static_cast<float>(Img.Sides[Direction].Dimensions.x - 1));
	uint32 PositionY = static_cast<uint32>(Uv.y * static_cast<float>(Img.Sides[Direction].Dimensions.y - 1));
	if (Direction >= GeneratorDirection::NUM)
	{
		std::wcout << L"Failed to get the proper direction" << std::endl;
	}
	if ((PositionX + (PositionY * Img.Sides[Direction].Dimensions.x)) > (Img.Sides[Direction].Dimensions.x * Img.Sides[Direction].Dimensions.y))
	{
		std::wcout << L"Tried to find position past the end of the image buffer" << std::endl;
		std::wcout << L"PositionX: " << PositionX << L" PositionY: " << PositionY << L"\nImg : Sides[" << static_cast<uint32>(Direction) << L"].Dimensions : X : " << Img.Sides[Direction].Dimensions.x << L" Y: " << Img.Sides[Direction].Dimensions.y << std::endl;
		std::wcin.get();
	}
	Color Result = Img.Sides[Direction].ColorBuffer[PositionX + (PositionY * Img.Sides[Direction].Dimensions.x)];
	return Result;
}

#define SAMPLES_PER_FACE 512

void ProcessImage(uint32 Face, const ImageHandle& SrcTexture, ImageHandle& DstTexture)
{
	uint32 i = Face;
	GeneratorDirection Dir = static_cast<GeneratorDirection>(i);
	std::wcout << L"Begin Side: " << i + 1 << std::endl;
	std::random_device rd; 
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.f, 180.0f);
	for (uint32 y = 0; y < SrcTexture.Img.Sides[i].Dimensions.y; y++)
	{
		for (uint32 x = 0; x < SrcTexture.Img.Sides[i].Dimensions.x; x++)
		{
			PixelNormal BaseNorm = GenPixelNormal(Dir, SrcTexture.Img.Sides[i], uvec2(x, y));
			fvec3 RightVector = normalized(GetRightVector(BaseNorm));
			fvec3 UpVector = normalized(GetUpVector(BaseNorm));
			fvec3 Col = ColorToFloat3(PixelFromNormal(BaseNorm.Normal, SrcTexture.Img));
			float Iter = 1.f;
			for (int NumSamples = 0; NumSamples < SAMPLES_PER_FACE; NumSamples++)
			{
				float xx = dis(gen);
				float yy = dis(gen);
				Quaternion YRot = Quaternion::FromAxisAngle(RightVector, radians(yy - 90.f));
				Quaternion XRot = Quaternion::FromAxisAngle(UpVector, radians(xx - 90.f));
				fvec3 Normal = normalized(((fmat3x3)(YRot * XRot).Normalized().RotationMatrix()) * BaseNorm.Normal);
				Col += (ColorToFloat3(PixelFromNormal(Normal, SrcTexture.Img)) * (dot(Normal, BaseNorm.Normal)));
				Iter++;
			}
			Col *= _PI_;
			Col /= Iter;
			Color Avg = Float3ToColor(Col);
			DstTexture.Img.Sides[i].ColorBuffer[x + (y * SrcTexture.Img.Sides[i].Dimensions.x)] = Avg;
		}
	}
	std::wcout << L"End Side: " << i + 1 << L" On thread: " << std::this_thread::get_id() << std::endl;
}


int main(int argc, wchar_t** argv)
{
	if (argc < 13)
	{
		std::wcout << L"Not enough arguments of textures passed in, aborting" << std::endl;
		return 1;
	}

	const wchar_t** Src = const_cast<const wchar_t**>(&argv[1]);
	const wchar_t** Dst = const_cast<const wchar_t**>(&argv[7]);

// 	if (false)
// 	{
// 		std::wstring DstFolder = std::wstring(Dst[i]);
// 		std::wcout << DstFolder.length() << std::endl;
// 		size_t LastSlashLocation = DstFolder.find_last_of(L'/', 0);
// 
// 		if (LastSlashLocation != std::wstring::npos)
// 		{
// 			std::wstring DirectoryName = DstFolder.substr(0, LastSlashLocation);
// 			DWORD DirectoryTraits = GetFileAttributes(DirectoryName.c_str());
// 			if (DirectoryTraits & FILE_ATTRIBUTE_DIRECTORY)
// 			{
// 				std::wcout << L"Directory already exists?" << std::endl;
// 				continue;
// 			}
// 			else
// 			{
// 				CreateDirectoryW(DirectoryName.c_str(), nullptr);
// 				std::wcout << L"Created directory: " << DirectoryName << std::endl;
// 			}
// 		}
// 		else
// 		{
// 			std::wcout << L"No need to create directory for: " << DstFolder << std::endl;
// 		}
// 	}

	ImageHandle SrcTexture;
	SrcTexture.OpenRead(Src);

	ImageHandle DstTexture;
	DstTexture.OpenWrite(Dst, SrcTexture);

	//for (uint32 i = 0; i < 2; i++)
	{
		std::thread Thread1([&]() { ProcessImage(0, SrcTexture, DstTexture); });
		Sleep(2);
		std::thread Thread2([&]() { ProcessImage(1, SrcTexture, DstTexture); });
		Sleep(2);
		std::thread Thread3([&]() { ProcessImage(2, SrcTexture, DstTexture); });
		Sleep(2);
		std::thread Thread4([&]() { ProcessImage(3, SrcTexture, DstTexture); });
		Sleep(2);
		std::thread Thread5([&]() { ProcessImage(4, SrcTexture, DstTexture); });
		Sleep(2);
		ProcessImage(5, SrcTexture, DstTexture);
		if (Thread1.joinable()) { Thread1.join(); }
		if (Thread2.joinable()) { Thread2.join(); }		
		if (Thread3.joinable()) { Thread3.join(); }
		if (Thread4.joinable()) { Thread4.join(); }
		if (Thread5.joinable()) { Thread5.join(); }
	}
	DstTexture.WriteToFile();
}

