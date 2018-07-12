#include "PipelineCache.h"
#include <fstream>
#include "Core/Systems/File/JsonCPP.h"

using json = nlohmann::json;

/*
	Format:

	[
		{
			"PipelineName",

		}
	]
*/

void SerializePipelines(const std::vector<IGraphicsPipelineState*>& GFXPipelines, const std::vector<IComputePipelineState*>& CMPPipelines)
{
	UNUSED(GFXPipelines);
	UNUSED(CMPPipelines);
}



void DeserializePipelines(std::vector<IGraphicsPipelineState*>& GFXPipelines, const std::vector<IComputePipelineState*>& CMPPipelines)
{
	UNUSED(GFXPipelines);
	UNUSED(CMPPipelines);
}
