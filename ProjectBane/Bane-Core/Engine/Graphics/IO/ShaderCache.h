#pragma once

#include "../Interfaces/PipelineState.h"
#include "../Interfaces/GraphicsResources.h"
#include <map>

typedef struct SHADER_PARAMETERS {
	using Register = uint;
	std::map<std::string, Register> CBVS;
} SHADER_PARAMETERS;

class ShaderCache
{
public:

	static ShaderCache* GInstance;

	IGraphicsPipelineState* LoadGraphicsPipeline(const std::string& ShaderFile);
	void ReloadGraphicsShader(const std::string& ShaderFile);

	IComputePipelineState* LoadComputePipeline(const std::string& ShaderFile);
	void ReloadComputeShader(const std::string& ShaderFile);

	void TransitionToReloadState();
	
	void InitCache();
	void DestroyCache();

private:

	GFX_PIPELINE_STATE_DESC InternalParseGraphicsShader(const std::string& InFile, const std::string& FileName);
	COMPUTE_PIPELINE_STATE_DESC InternalParseComputeShader(const std::string& InFile, const std::string& FileName);

	IDepthStencilState* m_DisabledDepthStencil;
	IDepthStencilState* m_EnabledDepthStencil;
	std::map<std::string, IGraphicsPipelineState*> m_GraphicsPipelines;
	std::map<std::string, IComputePipelineState*> m_ComputePipelines;
	
};

ForceInline ShaderCache* GetShaderCache()
{
	return ShaderCache::GInstance;
}

ForceInline void InitShaderCache()
{
	ShaderCache::GInstance = new ShaderCache();
	ShaderCache::GInstance->InitCache();
}

ForceInline void DestroyShaderCache()
{
	GetShaderCache()->DestroyCache();
	delete GetShaderCache();
}


