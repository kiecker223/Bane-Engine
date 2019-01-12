#pragma once

#include <string>
#include <Graphics/Interfaces/ApiRuntime.h>

typedef struct CHARACTER_DATA {
	float2 UV;
	float2 Size;
	float2 Center;
} CHARACTER_DATA;

class Font
{
public:

	inline CHARACTER_DATA GetDataForCharacter(char Character)
	{
		return m_Characters[static_cast<uint32>(Character)];
	}

	ITexture2D* FontTexture;
	IGraphicsPipelineState* FontShader;

	void LoadFont(const std::string& FontName);

private:

	std::vector<CHARACTER_DATA> m_Characters;
	std::string m_Name;
};
