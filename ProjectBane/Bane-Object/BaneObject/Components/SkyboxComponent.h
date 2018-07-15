#pragma once

#include "RenderComponent.h"
#include "Graphics/Interfaces/ApiRuntime.h"


class SkyboxComponent : public RenderComponent
{
	IMPLEMENT_COMPONENT(SkyboxComponent)
public:

	~SkyboxComponent();

	void SubmitFeature(SceneRenderer* Renderer);

	void SetSkybox(const std::string& Skybox);
	void SetSkybox(
		const std::string& Name,
		const std::string& NX,
		const std::string& NY,
		const std::string& NZ,
		const std::string& PX,
		const std::string& PY,
		const std::string& PZ);
	void SetSkyboxShader(const std::string& ShaderFile);
	void SetCameraConstants(IConstantBuffer* Buffer);
	inline ITexture3D* GetSkyboxTexture() { return m_Skybox; }

	void Draw(IGraphicsCommandContext* Ctx);

private:

	std::string m_SkyboxName;
	ITexture3D* m_Skybox;
	IVertexBuffer* m_VertexBuffer;
	IIndexBuffer* m_IndexBuffer;
	uint m_IndexCount;
	IShaderResourceTable* m_Table;
	ISamplerState* m_Sampler;
	IGraphicsPipelineState* m_Pipeline;

};


