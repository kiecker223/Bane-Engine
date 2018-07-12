
#include <thread>
#include "stdafx.h"

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
	uint Mask;
};

struct IntCol
{
	IntCol() : r(0), g(0), b(0), a(0) { }
	IntCol(byte R, byte G, byte B, byte A) : r(static_cast<uint>(R)), g(static_cast<uint>(G)), b(static_cast<uint>(B)), a(static_cast<uint>(A)) { }
	IntCol(const Color& C) : r(C.r), g(C.g), b(C.b), a(C.a) { }
	uint r, g, b, a;
};

IntCol operator + (const IntCol& Lhs, const IntCol& Rhs)
{
	return IntCol(Lhs.r + Rhs.r, Lhs.g + Rhs.g, Lhs.b + Rhs.b, Lhs.a + Rhs.a);
}

IntCol operator / (const IntCol& Lhs, uint Rhs)
{
	return IntCol(Lhs.r / Rhs, Lhs.g / Rhs, Lhs.b / Rhs, Lhs.a / Rhs);
}

Color operator / (const Color& Lhs, uint Rhs)
{
	return Color (Lhs.r / Rhs, Lhs.g / Rhs, Lhs.b / Rhs, Lhs.a / Rhs);
}

IntCol& operator += (IntCol& Lhs, const Color& Rhs)
{
	Lhs.r += static_cast<uint>(Rhs.r);
	Lhs.g += static_cast<uint>(Rhs.g);
	Lhs.b += static_cast<uint>(Rhs.b);
	Lhs.a += static_cast<uint>(Rhs.a);
	return Lhs;
}

Color FindAverage(IntCol c)
{
	static uint A = 180 / 30;
	static uint B = A * A;
	static uint D = B - 10;
	return Color(c.r / D, c.g / D, c.b / D, 255);
	//return Color(c.r, c.g, c.b, 255);
}

struct ImageSide
{
	uint2 Dimensions;
	Color* ColorBuffer;
};

struct Image
{
	ImageSide Sides[6];
};


struct PixelNormal
{
	float3 Normal;
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

		for (uint i = 0; i < 6; i++)
		{
			const wchar_t* Name = Names[i];
			ilImage LoadedImage;
			if (!LoadedImage.Load(Name))
			{
				std::cout << "Failed to load image: " << Name << std::endl;
				std::cin.get();
				abort();
			}
			ILenum Origin = LoadedImage.GetOrigin();
			if (Origin == IL_ORIGIN_UPPER_LEFT && !NeedsFlip(static_cast<GeneratorDirection>(i)))
			{
				std::cout << "\n==========================\nFlipping image\n==========================\n" << std::endl;
				LoadedImage.Bind();
				iluFlipImage();
			}
			if (i == static_cast<uint>(GeneratorDirection::PosY) || i == static_cast<uint>(GeneratorDirection::NegY))
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
		for (uint i = 0; i < 6; i++)
		{
			Img.Sides[i].Dimensions = InHandle.Img.Sides[i].Dimensions;
			Img.Sides[i].ColorBuffer = new Color[InHandle.Img.Sides[i].Dimensions.x * InHandle.Img.Sides[i].Dimensions.y];
		}
	}

	void WriteToFile()
	{
		for (uint i = 0; i < 6; i++)
		{
			const wchar_t* Name = Names[i];
			uint Width  = Img.Sides[i].Dimensions.x;
			uint Height = Img.Sides[i].Dimensions.y;
			ilImage LoadedImg;
			LoadedImg.TexImage(Width, Height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, static_cast<void*>(Img.Sides[i].ColorBuffer));
			ilEnable(IL_FILE_OVERWRITE);
			LoadedImg.Save(Name);
			LoadedImg.Close();
			delete[] Img.Sides[i].ColorBuffer;
		}
	}
};

PixelNormal GenPixelNormal(GeneratorDirection Direction, const ImageSide& Img, const uint2& Position)
{
	float3 Result(0.f, 0.f, -1.f);
	Quaternion Rot;
	float PosOfX = ((static_cast<float>(Position.x) / static_cast<float>(Img.Dimensions.x)) * 90.f);
	float PosOfY = ((static_cast<float>(Position.y) / static_cast<float>(Img.Dimensions.y)) * 90.f);

	switch (Direction)
	{
		case PosX: 
		{
			Rot = Quaternion(float3(PosOfY - 45.f, PosOfX + 45.f, 0.f));
		} break;
		case NegX: 
		{
			Rot = Quaternion(float3(PosOfY - 45.f, PosOfX + 225.f, 0.f));
		} break;
		case PosY: 
		{
			Rot = Quaternion(float3(PosOfY + 45.f, 0.f, PosOfX - 45.f));
		} break;
		case NegY: 
		{
			Rot = Quaternion(float3(PosOfY + 225.f, 0.f, PosOfX - 45.f));
		} break;
		case PosZ: 
		{
			Rot = Quaternion(float3(PosOfY - 45.f, PosOfX - 45.f, 0.f));
		} break;
		case NegZ:
		{
			Rot = Quaternion(float3(-PosOfY + 225.f, -PosOfX + 225.f, 0.f));
		} break;
	}

	Result = (float3x3)Rot.RotationMatrix() * Result;
	return { Result, Rot };
}

static const float3 UpDir(0.f, 1.f, 0.f);
float3 GetRightVector(const PixelNormal& Norm)
{
	return cross(Norm.Normal, UpDir);
}

float3 GetUpVector(const PixelNormal& Norm)
{
	return cross(cross(normalized(Norm.Normal), UpDir), normalized(Norm.Normal));
}

Color PixelFromNormal(const float3& InNorm, const Image& Img)
{
	GeneratorDirection Direction = GeneratorDirection::NUM;
	float3 Norm = normalized(float3(abs(InNorm.x), abs(InNorm.y), abs(InNorm.z)));
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
		return Color(128, 128, 128, 128);
	}

	float2 Uv;
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
	
	uint PositionX = static_cast<uint>(Uv.x * static_cast<float>(Img.Sides[Direction].Dimensions.x - 1));
	uint PositionY = static_cast<uint>(Uv.y * static_cast<float>(Img.Sides[Direction].Dimensions.y - 1));
	if (Direction >= GeneratorDirection::NUM)
	{
		std::cout << "Failed to get the proper direction" << std::endl;
	}
	if ((PositionX + (PositionY * Img.Sides[Direction].Dimensions.x)) > (Img.Sides[Direction].Dimensions.x * Img.Sides[Direction].Dimensions.y))
	{
		std::cout << "Tried to find position past the end of the image buffer" << std::endl;
		std::cout << "PositionX: " << PositionX << " PositionY: " << PositionY << "\nImg : Sides[" << static_cast<uint>(Direction) << "].Dimensions : X : " << Img.Sides[Direction].Dimensions.x << " Y: " << Img.Sides[Direction].Dimensions.y << std::endl;
		std::cin.get();
	}
	Color Result = Img.Sides[Direction].ColorBuffer[PositionX + (PositionY * Img.Sides[Direction].Dimensions.x)];
	return Result;
}

void ProcessImage(uint Face, const ImageHandle& SrcTexture, ImageHandle& DstTexture)
{
	uint i = Face;
	GeneratorDirection Dir = static_cast<GeneratorDirection>(i);
	std::cout << "Begin Side: " << i + 1 << std::endl;
	for (uint y = 0; y < SrcTexture.Img.Sides[i].Dimensions.y; y++)
	{
		for (uint x = 0; x < SrcTexture.Img.Sides[i].Dimensions.x; x++)
		{
			PixelNormal BaseNorm = GenPixelNormal(Dir, SrcTexture.Img.Sides[i], uint2(x, y));
			float3 RightVector = normalized(GetRightVector(BaseNorm));
			float3 UpVector = normalized(GetUpVector(BaseNorm));
			IntCol Col;
			for (float yy = 0; yy < 180.1f; yy += 30.f)
			{
				float3 UpLerp = lerp(-UpVector, UpVector, yy / 180.f);
				for (float xx = 0; xx < 180.1f; xx += 30.f)
				{
					float3 RightLerp = lerp(-RightVector, RightVector, xx / 180.f);
					float3 NewNormal = lerp(UpLerp, RightLerp, 0.5f);
					Col += PixelFromNormal(normalized(NewNormal), SrcTexture.Img);
				}
			}
			Color Avg = FindAverage(Col);
			DstTexture.Img.Sides[i].ColorBuffer[x + (y * SrcTexture.Img.Sides[i].Dimensions.x)] = Avg;
		}
	}
	std::cout << "End Side: " << i + 1 << " On thread: " << std::this_thread::get_id() << std::endl;
}

int main(int argc, wchar_t** argv)
{
	if (argc < 13)
	{
		std::cout << "No name of textures passed in, aborting" << std::endl;
		return 1;
	}

	const wchar_t** Src = const_cast<const wchar_t**>(&argv[1]);
	const wchar_t** Dst = const_cast<const wchar_t**>(&argv[7]);

	ImageHandle SrcTexture;
	SrcTexture.OpenRead(Src);

	ImageHandle DstTexture;
	DstTexture.OpenWrite(Dst, SrcTexture);
	for (uint i = 0; i < 2; i++)
	{
		std::thread Thread1([&]() { ProcessImage(i + 2, SrcTexture, DstTexture); });
		Sleep(2);
		std::thread Thread2([&]() { ProcessImage(i + 4, SrcTexture, DstTexture); });
		Sleep(2);
		ProcessImage(i, SrcTexture, DstTexture);
		if (Thread1.joinable())
		{
			Thread1.join();
		}
		if (Thread2.joinable())
		{
			Thread2.join();
		}
	}
	DstTexture.WriteToFile();
}

