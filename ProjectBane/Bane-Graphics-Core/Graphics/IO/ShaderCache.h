#pragma once

#include "../Interfaces/PipelineState.h"
#include "../Interfaces/GraphicsResources.h"
#include <map>

typedef struct SHADER_PARAMETERS {
	using Register = uint32;
	std::map<std::string, Register> CBVS;
} SHADER_PARAMETERS;

class ShaderCache
{
public:

	static ShaderCache* GInstance;

	IGraphicsPipelineState* LoadGraphicsPipeline(const std::string& ShaderFile);
	IComputePipelineState* LoadComputePipeline(const std::string& ShaderFile);

	void ReloadAllShaders();
	
	void InitCache(const std::string& JsonLocation);
	void DestroyCache();

private:

	std::map<std::string, IGraphicsPipelineState*> m_GraphicsPipelines;
	std::map<std::string, IComputePipelineState*> m_ComputePipelines;
	
};

inline ShaderCache* GetShaderCache()
{
	return ShaderCache::GInstance;
}

inline void InitShaderCache(const std::string& JsonLocation)
{
	ShaderCache::GInstance = new ShaderCache();
	ShaderCache::GInstance->InitCache(JsonLocation);
}

inline void DestroyShaderCache()
{
	GetShaderCache()->DestroyCache();
	delete GetShaderCache();
}


