#pragma once

#include "../Interfaces/GraphicsResources.h"
#include "../Interfaces/ShaderResourceView.h"
#include "../Interfaces/GraphicsCommandList.h"
#include "Graphics/Rendering/RendererInterface.h"


typedef enum ERENDER_FLAGS {
	RENDER_FLAG_NONE		= 0x00000000,
	RENDER_FLAG_NO_SHADOWS	= 0x00000010
} ERENDER_FLAGS;

typedef struct MATERIAL_PARAMETERS
{
	float3 Color;
	float SpecularFactor;
private:
	float2 P0;
public:
	float Roughness;
	float Metallic;
} MATERIAL_PARAMETERS;


class Material
{
public:

	Material();

	void Bind(IGraphicsCommandContext* Context);
	void Initialize(SceneRenderer* Renderer);

	void SetDiffuseTexture(const std::string& FileName);
	void SetNormalTexture(const std::string& FileName);
	void SetSpecularTexture(const std::string& FileName);

	void SetTexture(ITextureBase* Texture, ISamplerState* Sampler, uint TextureRegister, uint SamplerRegister);
	void SetTexture(ITextureBase* Texture, uint TextureRegister);
	void SetTexture(const std::string& Texture, uint Register);
	void SetConstantBuffer(IConstantBuffer* ConstantBuffer, uint Register);

	void SetMaterialParameters(const MATERIAL_PARAMETERS& Params);
	void UpdateMaterialParameters(IGraphicsCommandContext* Ctx);

	ForceInline IShaderResourceTable* GetTable() const { return m_Table; }

	void LoadFromFile(const std::string& ShaderName);
	ForceInline std::string GetMaterialName() const { return m_MaterialName; }

	ForceInline MATERIAL_PARAMETERS& GetMaterialParameters()
	{
		return m_Parameters;
	}
	
	ERENDER_FLAGS RenderFlags;

private:
	std::string m_MaterialName;
	MATERIAL_PARAMETERS m_Parameters;
	IConstantBuffer* m_MatCB;
	IShaderResourceTable* m_Table;
	IGraphicsPipelineState* m_Pipeline;
};

