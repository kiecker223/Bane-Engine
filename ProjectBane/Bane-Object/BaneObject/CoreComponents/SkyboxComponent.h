#pragma once

#include "RenderComponent.h"
#include "Graphics/Interfaces/ApiRuntime.h"


class SkyboxComponent : public RenderComponent
{
	IMPLEMENT_COMPONENT(SkyboxComponent)
public:

	~SkyboxComponent();

	void GraphicsUpdate(RenderLoop& Loop);

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
	
private:

	std::string m_SkyboxName;
	ITexture3D* m_Skybox;
	IVertexBuffer* m_VertexBuffer;
	IIndexBuffer* m_IndexBuffer;
	uint32 m_IndexCount;
	IShaderResourceTable* m_Table;
	IGraphicsPipelineState* m_Pipeline;

};


