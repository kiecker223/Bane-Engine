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

	IGraphicsPipelineState* LoadGraphicsPipeline(const std::string& ShaderFile, uint32& OutHash);
	inline IGraphicsPipelineState* LoadGraphicsPipeline(const std::string& ShaderFile)
	{
		uint32 Dummy = 0;
		UNUSED(Dummy);
		return LoadGraphicsPipeline(ShaderFile, Dummy);
	}

	IComputePipelineState* LoadComputePipeline(const std::string& ShaderFile, uint32& OutHash);
	inline IComputePipelineState* LoadComputePipeline(const std::string& ShaderFile)
	{
		uint32 Dummy = 0;
		UNUSED(Dummy);
		return LoadComputePipeline(ShaderFile, Dummy);
	}

	void ReloadAllShaders();
	
	void InitCache(const std::string& JsonLocation);
	void DestroyCache();

private:

	std::map<std::string, std::pair<IGraphicsPipelineState*, uint32>> m_GraphicsPipelines;
	std::map<std::string, std::pair<IComputePipelineState*, uint32>> m_ComputePipelines;
	
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


