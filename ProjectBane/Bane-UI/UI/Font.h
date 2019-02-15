#pragma once

#include <string>
#include <Graphics/Interfaces/ApiRuntime.h>

typedef struct CHARACTER_DATA {
	float Advance;
} CHARACTER_DATA;

typedef struct FONT_VERTEX {
	struct {
		float2 Position;
		float2 UV;
	} Data[4];
} FONT_VERTEX;

class Font
{
public:

	inline FONT_VERTEX GetDataForCharacter(char Character)
	{
		return m_Characters[static_cast<uint32>(Character)];
	}

	inline uint64 GetVertexBufferOffset(char Character)
	{
		return sizeof(FONT_VERTEX) * static_cast<uint64>(Character);
	}

	ITexture2D* FontTexture;
	IBuffer* FontDataStructuredBuff;
	IConstantBuffer* FontParameters;
	IGraphicsPipelineState* FontShader;
	IVertexBuffer* VB;
	IIndexBuffer* IB;

	void LoadFont(const std::string& FontName);

private:

	std::vector<FONT_VERTEX> m_Characters;
	std::string m_Name;
};
