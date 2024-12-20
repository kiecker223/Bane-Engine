#pragma once

#include "../Interfaces/GraphicsResources.h"
#include "../Interfaces/ShaderResourceView.h"
#include "../Interfaces/GraphicsCommandList.h"


typedef enum ERENDER_FLAGS {
	RENDER_FLAG_NONE		= 0x00000000,
	RENDER_FLAG_NO_SHADOWS	= 0x00000010
} ERENDER_FLAGS;

typedef SHADER_ALIGNMENT struct MATERIAL_PARAMETERS
{
	fvec3 Color;
	float Padding0;
	float SpecularFactor;
	float Roughness;
	float Metallic;
} MATERIAL_PARAMETERS;


class Material
{
public:

	Material();
	~Material();

	void SetDiffuseTexture(const std::string& FileName);
	void SetNormalTexture(const std::string& FileName);
	void SetSpecularTexture(const std::string& FileName);

	void SetTexture(ITextureBase* Texture, uint32 TextureRegister, uint32 SamplerRegister);
	void SetTexture(ITextureBase* Texture, uint32 TextureRegister);
	void SetTexture(const std::string& Texture, uint32 Register);
	void SetConstantBuffer(IConstantBuffer* ConstantBuffer, uint32 Register);

	void SetMaterialParameters(const MATERIAL_PARAMETERS& Params);

	inline IGraphicsPipelineState* GetShader() const { return m_Pipeline; }

	ITexture2D* Diffuse;
	ITexture2D* Normal;

	void InitializeMaterial(const std::string& ShaderName);
	inline std::string GetMaterialName() const { return m_MaterialName; }

	inline MATERIAL_PARAMETERS GetMaterialParameters() const
	{
		return m_Parameters;
	}
	
	inline uint32 GetShaderHash() const
	{
		return m_MaterialHash;
	}

	ERENDER_FLAGS RenderFlags;

private:
	uint32 m_MaterialHash;
	std::string m_MaterialName;
	MATERIAL_PARAMETERS m_Parameters;
	IGraphicsPipelineState* m_Pipeline;
};

