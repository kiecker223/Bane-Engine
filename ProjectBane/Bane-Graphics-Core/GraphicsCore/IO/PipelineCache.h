#pragma once

#include "../Interfaces/PipelineState.h"



void SerializePipelines(const std::vector<IGraphicsPipelineState*>& GFXPipelines, const std::vector<IComputePipelineState*>& CMPPipelines);

void DeserializePipelines(std::vector<IGraphicsPipelineState*>& GFXPipelines, const std::vector<IComputePipelineState*>& CMPPipelines);



