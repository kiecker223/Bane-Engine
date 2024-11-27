#include <Platform/System/File/PlatformFile.h>
#include <IL/devil_cpp_wrapper.hpp>
#include "BaneMath.h"
#include <Common/BaneMacros.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <JSON/JsonCPP.h>
#include <limits>
#include <fstream>


static const uint32 WIDTH = 512;
static const uint32 HEIGHT = 512;
static const uint32 SLACK = 20;
static const uint32 NUM_PASSES = 10;
static const float SQUARE_OF_2 = 1.41421356237f;
static const float RADIUS = 5.0f;

struct Color
{
	Color() : R(0), G(0), B(0), A(0) { }
	union
	{
		struct { uint8 R, G, B, A; };
		uint32 Mask;
	};

	float NormalizedCol() const
	{
		return static_cast<float>(R) / 255.f;
	}

	Color& operator = (const Color& Rhs)
	{
		Mask = Rhs.Mask;
		return *this;
	}

	bool IsZero() const
	{
		return !R && !G && !B && !A;
	}
};

struct Image
{
public:
	Color Buffer[WIDTH*HEIGHT];

	Image()
	{
		memset(reinterpret_cast<void*>(Buffer), 0, sizeof(Buffer));
	}

	void WriteToFile(const std::string& FileName)
	{
		ilImage Result;
		Result.Bind();
		ilEnable(IL_FILE_OVERWRITE);
		Result.TexImage(WIDTH, HEIGHT, 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, reinterpret_cast<void*>(Buffer));
		iluFlipImage();
		Result.Save(FileName.c_str());
		Result.Close();
	}

	Color& GetColor(uint32 X, uint32 Y)
	{
		return Buffer[(Y * WIDTH) + X];
	}

	void CopyPixelRegion(uint32 X, uint32 Y, Color* OtherBuff, uint32 OtherX, uint32 OtherY)
	{
		for (uint32 y = 0; y < OtherY; y++)
		{
			uint32 RowStart = ((Y + y) * WIDTH) + X;
			memcpy(&Buffer[RowStart], &OtherBuff[y * OtherX], OtherX * 4);
		}
	}
};

class SDFBuilderInformation
{
public:

	fvec2 GradientDirections[WIDTH*HEIGHT];
	float GradientDistances[WIDTH*HEIGHT];

	SDFBuilderInformation()
	{
		for (uint32 i = 0; i < WIDTH*HEIGHT; i++)
		{
			auto Val = std::numeric_limits<float>::max() - 1.f;
			GradientDistances[i] = Val;
		}
	}

	fvec2& GetDirection(uint32 X, uint32 Y)
	{
		return GradientDirections[(Y * WIDTH) + X];
	}

	fvec2& GetDirection(const uvec2& Pos)
	{
		return GetDirection(Pos.x, Pos.y);
	}

	float& GetDistances(uint32 X, uint32 Y)
	{
		return GradientDistances[(Y * WIDTH) + X];
	}

	float& GetDistances(const uvec2& Pos)
	{
		return GetDistances(Pos.x, Pos.y);
	}
};

using namespace nlohmann;

class PerCharacterInfo
{
public:

	struct Character
	{
		uvec2 PixelStartLocation;
		uvec2 Dimensions, Bearing;
		uint32 Advance;
	};

	Character Characters[127];

	Character& GetCharacter(char Char)
	{
		return Characters[static_cast<uint32>(Char)];
	}

	void SetCharacterInfo(char Char, const FT_Face& InFont)
	{
		auto& CharInfo = Characters[Char];
		CharInfo.Bearing.x = InFont->glyph->bitmap_left;
		CharInfo.Bearing.y = InFont->glyph->bitmap_top;
		CharInfo.Advance = InFont->glyph->advance.x;
	}

	void WriteToFile(const std::string& FileToWriteTo, const std::string& FontName, const std::string& TextureLocationName)
	{
		json Parent = json::object();
		json Array = json::array();
		for (uint32 i = 0; i < 127; i++)
		{
			json FontObject = json::object();
			json DimensionsObject = json::object();
			auto CurrentChar = GetCharacter(static_cast<char>(i));
			DimensionsObject["x"] = CurrentChar.Dimensions.x;
			DimensionsObject["y"] = CurrentChar.Dimensions.y;
			json BearingObject = json::object();
			BearingObject["x"] = CurrentChar.Bearing.x;
			BearingObject["y"] = CurrentChar.Bearing.y;
			json LocationObject = json::object();
			LocationObject["x"] = CurrentChar.PixelStartLocation.x;
			LocationObject["y"] = CurrentChar.PixelStartLocation.y;
			FontObject["Dimensions"] = DimensionsObject;
			FontObject["Bearing"] = BearingObject;
			FontObject["Advance"] = CurrentChar.Advance;
			FontObject["Location"] = LocationObject;
			Array.push_back(FontObject);
		}
		Parent[FontName] = Array;
		Parent["TextureLocation"] = TextureLocationName;
		std::ofstream Output(FileToWriteTo);
		if (Output.is_open())
		{
			Output << Parent.dump(4);
		}
		Output.close();
	}
}; 

float DistanceToEdge(float gradientX, float gradientY, float color)
{
	float distance = 0.0f;
	float calculatedCol;
	if (gradientX == 0.0f || gradientY == 0.0f)
	{
		distance = 0.5f - color;
	}
	else
	{
		gradientX = fabsf(gradientX);
		gradientY = fabsf(gradientY);

		if (gradientX < gradientY)
		{
			// Swap, the calculation is dependent on x being bigger
			float storage = gradientX;
			gradientX = gradientY;
			gradientY = storage;
		}
		calculatedCol = 0.5f * gradientY / gradientX;

		if (color < calculatedCol)
		{
			distance = 0.5f * (gradientX + gradientY) - sqrtf(2.0f * gradientX * gradientY * color);
		}
		else if (color < (1.0f - calculatedCol))
		{
			distance = (0.5f - color) * gradientX;
		}
		else
		{
			distance = -0.5f * (gradientX + gradientY) + sqrtf(2.0f * gradientX * gradientY * (1.0f - color));
		}
	}
	return distance;
}

float DistanceSquared(const fvec2& r, const fvec2& l)
{
	float x = fabsf(l.x - r.x);
	float y = fabsf(l.y - r.y);
	return x * x + y * y;
}

enum SearchDir
{
	Up, Down, Left, Right, None
};

void GenerateImageInfo(Image* ResultingImage, SDFBuilderInformation* BuildInfo, uvec2 Position)
{
	static const float DistSlack = 1e-3f;
	fvec2 Point((float)Position.x, (float)Position.y);
	float PointDist = BuildInfo->GetDistances(Position);
	fvec2 PointDir = BuildInfo->GetDirection(Position);
	uint32 cX = Position.x;
	uint32 cY = Position.y;
	float Distance;
	bool bChanged = false;

	SearchDir Dir = SearchDir::None;

	float LeastDist = std::numeric_limits<float>::max() - 1;
	BANE_CHECK(!isnan(LeastDist));
	float L = BuildInfo->GetDistances(cX - 1, cY);
	float D = BuildInfo->GetDistances(cX, cY + 1);
	float R = BuildInfo->GetDistances(cX + 1, cY);
	float U = BuildInfo->GetDistances(cX, cY - 1);
	if (L < LeastDist)
	{
		Dir = Left;
		LeastDist = L;
	}
	if (D < LeastDist)
	{
		Dir = Down;
		LeastDist = D;
	}
	if (R < LeastDist)
	{
		Dir = Right;
		LeastDist = R;
	}
	if (U < LeastDist)
	{
		Dir = Up;
		LeastDist = U;
	}
	if (Dir == SearchDir::None)
	{
		return;
	}

	if (Dir == Left)
	{
		fvec2 Dir = BuildInfo->GetDirection(cX - 1, cY);
		fvec2 DirDist = (Dir * BuildInfo->GetDistances(cX - 1, cY));
		fvec2 OtherPoint((float)cX - 1, (float)cY);
		fvec2 OtherResultPoint = OtherPoint + DirDist;
		fvec2 ToResult = OtherResultPoint - Point;
		Distance = length(ToResult);

		if (Distance + DistSlack < PointDist)
		{
			Point = normalized(ToResult);
			bChanged = true;
		}
	}
	else if (Dir == Down)
	{
		fvec2 Dir = BuildInfo->GetDirection(cX, cY + 1);
		fvec2 DirDist = (Dir * BuildInfo->GetDistances(cX, cY + 1));
		fvec2 OtherPoint((float)cX, (float)cY + 1);
		fvec2 OtherResultPoint = OtherPoint + DirDist;
		fvec2 ToResult = OtherResultPoint - Point;
		Distance = length(ToResult);

		if (Distance + DistSlack < PointDist)
		{
			Point = normalized(ToResult);
			bChanged = true;
		}
	}
	else if (Dir == Right)
	{
		fvec2 Dir = BuildInfo->GetDirection(cX + 1, cY);
		fvec2 DirDist = (Dir * BuildInfo->GetDistances(cX + 1, cY));
		fvec2 OtherPoint((float)cX + 1, (float)cY);
		fvec2 OtherResultPoint = OtherPoint + DirDist;
		fvec2 ToResult = OtherResultPoint - Point;
		Distance = length(ToResult);

		if (Distance + DistSlack < PointDist)
		{
			Point = normalized(ToResult);
			bChanged = true;
		}
	}
	else if (Dir == Up)
	{
		fvec2 Dir = BuildInfo->GetDirection(cX, cY - 1);
		fvec2 DirDist = (Dir * BuildInfo->GetDistances(cX, cY - 1));
		fvec2 OtherPoint((float)cX, (float)cY - 1);
		fvec2 OtherResultPoint = OtherPoint + DirDist;
		fvec2 ToResult = OtherResultPoint - Point;
		Distance = length(ToResult);

		if (Distance + DistSlack < PointDist)
		{
			Point = normalized(ToResult);
			bChanged = true;
		}
	}
	if (bChanged)
	{
		BuildInfo->GetDirection(Position) = Point;
		BuildInfo->GetDistances(Position) = Distance;
	}
}

void MakeImageIntoSDF(Image* ResultingImage, SDFBuilderInformation* BuildInfo)
{
	for (uint32 y = 1; y < HEIGHT - 1; y++)
	{
		for (uint32 x = 1; x < WIDTH - 1; x++)
		{
			auto& Col = ResultingImage->GetColor(x, y);
			if (Col.R >= 200)
			{
				continue;
			}
			if (Col.R <= 1)
			{
				if (ResultingImage->GetColor(x - 1, y).IsZero() && ResultingImage->GetColor(x + 1, y).IsZero() &&
					ResultingImage->GetColor(x, y - 1).IsZero() && ResultingImage->GetColor(x, y + 1).IsZero())
				{
					continue;
				}
			}
			fvec2 Direction;
			Direction.x = (-ResultingImage->GetColor(x - 1, y - 1).NormalizedCol()) - (ResultingImage->GetColor(x - 1, y).NormalizedCol() * SQUARE_OF_2) - 
							(ResultingImage->GetColor(x - 1, y + 1).NormalizedCol()) + (ResultingImage->GetColor(x + 1, y - 1).NormalizedCol()) +
							(ResultingImage->GetColor(x + 1, y).NormalizedCol() * SQUARE_OF_2) + (ResultingImage->GetColor(x + 1, y + 1).NormalizedCol());
			Direction.y = (-ResultingImage->GetColor(x - 1, y + 1).NormalizedCol()) - (ResultingImage->GetColor(x, y + 1).NormalizedCol() * SQUARE_OF_2) -
							(ResultingImage->GetColor(x + 1, y + 1).NormalizedCol()) + (ResultingImage->GetColor(x - 1, y - 1).NormalizedCol()) +
							(ResultingImage->GetColor(x, y - 1).NormalizedCol() * SQUARE_OF_2) + (ResultingImage->GetColor(x + 1, y - 1).NormalizedCol());

			if (fabsf(Direction.x) < 0.001f && fabsf(Direction.y) < 0.001f)
			{
				continue;
			}

			normalize(Direction);
			float Distance = DistanceToEdge(Direction.x, Direction.y, ResultingImage->GetColor(x, y).NormalizedCol());
			BuildInfo->GetDirection(x, y) = Direction;
			BuildInfo->GetDistances(x, y) = Distance;
		}
	}

	for (uint32 Pass = 0; Pass < NUM_PASSES; Pass++)
	{
		for (uint32 y = HEIGHT - 1; y > 1; y--)
		{
			for (uint32 x = WIDTH - 1; x > 1; x--)
			{
				GenerateImageInfo(ResultingImage, BuildInfo, uvec2(x, y));
			}
		}

		for (uint32 y = 1; y < HEIGHT - 1; y++)
		{
			for (uint32 x = 1; x < WIDTH - 1; x++)
			{
				GenerateImageInfo(ResultingImage, BuildInfo, uvec2(x, y));
			}
		}
	}

	for (uint32 y = 0; y < HEIGHT; y++)
	{
		for (uint32 x = 0; x < WIDTH; x++)
		{
			float Distance = (BuildInfo->GetDistances(x, y)) / RADIUS;
			if (ResultingImage->GetColor(x, y).R > 127)
			{
				Distance = -Distance;
			}
 			auto& Col = ResultingImage->GetColor(x, y);
			Col.R = (uint8)((saturate(0.5f - (Distance * 0.5f))) * 255.f);

		}
	}
}

int main(int argc, char** argv)
{
	Image* ResultingImage = new Image();
	SDFBuilderInformation* BuildInfo = new SDFBuilderInformation();
	PerCharacterInfo* CharInfo = new PerCharacterInfo();

	BANE_CHECK(argc == 4);

	std::string FontNameToLoad = argv[1];
	std::string FileToSaveTo = argv[2];
	std::string FontJsonFileName = argv[3];

	FT_Library FTLib;
	FT_Init_FreeType(&FTLib);

	FT_Face FTFace;
	FT_New_Face(FTLib, FontNameToLoad.c_str(), 0, &FTFace);

	FT_Set_Pixel_Sizes(FTFace, 0, 32);

	uint32 CurX = SLACK;
	uint32 CurY = SLACK;
	uint32 MaxRowHeight = 0;
	for (char c = 0; c < 127; c++)
	{
		FT_Load_Char(FTFace, c, FT_LOAD_RENDER);

		auto& Char = CharInfo->GetCharacter(c);
		Char.Dimensions = uvec2(FTFace->glyph->bitmap.width, FTFace->glyph->bitmap.rows);
		CharInfo->SetCharacterInfo(c, FTFace);
		if ((CurX + Char.Dimensions.x) + SLACK > WIDTH)
		{
			CurX = SLACK;
			CurY += MaxRowHeight + SLACK;
			MaxRowHeight = 0;
		}

		Color* ColorBuff = new Color[Char.Dimensions.x * Char.Dimensions.y];
		for (uint32 y = 0; y < Char.Dimensions.y; y++)
		{
			for (uint32 x = 0; x < Char.Dimensions.x; x++)
			{
				const uint32 memPos = (y * Char.Dimensions.x) + x;
				const auto val = FTFace->glyph->bitmap.buffer[memPos];
				ColorBuff[memPos].R = val;
			}
		}
		
		Char.PixelStartLocation = uvec2(CurX, CurY);
		ResultingImage->CopyPixelRegion(CurX, CurY, ColorBuff, Char.Dimensions.x, Char.Dimensions.y);
		delete[] ColorBuff;

		CurX += Char.Dimensions.x + SLACK;
		if (Char.Dimensions.y > MaxRowHeight)
		{
			MaxRowHeight = Char.Dimensions.y;
		}
	}
	MakeImageIntoSDF(ResultingImage, BuildInfo);
	ResultingImage->WriteToFile(FileToSaveTo);
	CharInfo->WriteToFile(FontJsonFileName, FontNameToLoad, FileToSaveTo);

	delete ResultingImage;
	delete BuildInfo;
	delete CharInfo;
}

