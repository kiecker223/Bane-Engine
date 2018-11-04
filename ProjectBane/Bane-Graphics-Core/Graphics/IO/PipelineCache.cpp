#include "PipelineCache.h"
#include <fstream>
//#include "Core/Systems/File/JsonCPP.h"
//
//using json = nlohmann::json;

/*
	Format:

	[
		{
			"PipelineName",

		}
	]
*/

void SerializePipelines(const TArray<IGraphicsPipelineState*>& GFXPipelines, const TArray<IComputePipelineState*>& CMPPipelines)
{
	UNUSED(GFXPipelines);
	UNUSED(CMPPipelines);
}



void DeserializePipelines(TArray<IGraphicsPipelineState*>& GFXPipelines, const TArray<IComputePipelineState*>& CMPPipelines)
{
	UNUSED(GFXPipelines);
	UNUSED(CMPPipelines);
}
