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

Material::~Material()
{
	if (m_Table)
		delete m_Table;
}

void Material::SetDiffuseTexture(const std::string& FileName)
{
	SetTexture(GetTextureCache()->LoadTexture(FileName), 0, 0);
}

void Material::SetNormalTexture(const std::string& FileName)
{
	SetTexture(GetTextureCache()->LoadTexture(FileName), 1, 0);
}

void Material::SetSpecularTexture(const std::string& FileName)
{
	SetTexture(GetTextureCache()->LoadTexture(FileName), 2, 0);
}

void Material::SetTexture(ITextureBase* Texture, uint32 TextureRegister, uint32 SamplerRegister)
{
	IRuntimeGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	Device->CreateShaderResourceView(m_Table, Texture, TextureRegister);
}

void Material::SetTexture(ITextureBase* Texture, uint32 TextureRegister)
{
	SetTexture(Texture, TextureRegister, 0);
}

void Material::SetTexture(const std::string& Texture, uint32 Register)
{
	SetTexture(GetTextureCache()->LoadTexture(Texture), Register);
}

void Material::SetConstantBuffer(IConstantBuffer* ConstantBuffer, uint32 Register)
{
	GetApiRuntime()->GetGraphicsDevice()->CreateConstantBufferView(m_Table, ConstantBuffer, Register);
}

void Material::SetMaterialParameters(const MATERIAL_PARAMETERS& Params)
{
	m_Parameters = Params;
}

void Material::InitializeMaterial(const std::string& ShaderName)
{
	IRuntimeGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	m_MaterialName = ShaderName;
	m_Pipeline = GetShaderCache()->LoadGraphicsPipeline(ShaderName);
	m_Table = Device->CreateShaderTable(m_Pipeline);
}
