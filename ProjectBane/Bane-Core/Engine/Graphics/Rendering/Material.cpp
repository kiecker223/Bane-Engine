#include "Material.h"
#include "../Interfaces/ApiRuntime.h"
#include "../IO/TextureCache.h"
#include "../IO/ShaderCache.h"


Material::Material() :
	m_Table(nullptr),
	m_Pipeline(nullptr)
{
	m_Parameters.Color = float3(1.0f, 1.0f, 1.0f);
	m_Parameters.SpecularFactor = 0.5f;
	m_Parameters.Roughness = 0.5f;
}

void Material::Initialize(SceneRenderer* Renderer)
{
}

void Material::Bind(IGraphicsCommandContext* Context)
{
	GetApiRuntime()->GetGraphicsDevice()->CreateShaderResourceView(m_Table, m_MatCB, 1);
	Context->SetGraphicsPipelineState(m_Pipeline);
	Context->SetGraphicsResourceTable(m_Table);
}

void Material::SetDiffuseTexture(const std::string& FileName)
{
	SetTexture(GetTextureCache()->LoadTexture(FileName), nullptr, 0, 0);
}

void Material::SetNormalTexture(const std::string& FileName)
{
	SetTexture(GetTextureCache()->LoadTexture(FileName), nullptr, 1, 0);
}

void Material::SetSpecularTexture(const std::string& FileName)
{
	SetTexture(GetTextureCache()->LoadTexture(FileName), nullptr, 2, 0);
}

void Material::SetTexture(ITextureBase* Texture, ISamplerState* Sampler, uint TextureRegister, uint SamplerRegister)
{
	IRuntimeGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	if (Sampler == nullptr)
	{
		Sampler = Device->GetDefaultSamplerState();
		SamplerRegister = 0;
	}
	Device->CreateShaderResourceView(m_Table, Texture, TextureRegister);
	Device->CreateSamplerView(m_Table, Sampler, SamplerRegister);
}

void Material::SetTexture(ITextureBase* Texture, uint TextureRegister)
{
	SetTexture(Texture, nullptr, TextureRegister, 0);
}

void Material::SetTexture(const std::string& Texture, uint Register)
{
	SetTexture(GetTextureCache()->LoadTexture(Texture), Register);
}

void Material::SetConstantBuffer(IConstantBuffer* ConstantBuffer, uint Register)
{
	GetApiRuntime()->GetGraphicsDevice()->CreateShaderResourceView(m_Table, ConstantBuffer, Register);
}

void Material::SetMaterialParameters(const MATERIAL_PARAMETERS& Params)
{
	m_Parameters = Params;
}

void Material::UpdateMaterialParameters(IGraphicsCommandContext* Ctx)
{
	void* pBuff = Ctx->Map(m_MatCB);
	memcpy(pBuff, (void*)&m_Parameters, sizeof(MATERIAL_PARAMETERS));
	Ctx->Unmap(m_MatCB);
}

void Material::LoadFromFile(const std::string& ShaderName)
{
	IRuntimeGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	m_MaterialName = ShaderName;
	m_Pipeline = GetShaderCache()->LoadGraphicsPipeline(ShaderName);
	m_Table = Device->CreateShaderTable(m_Pipeline);
	m_MatCB = Device->CreateConstBuffer<MATERIAL_PARAMETERS>();
}
