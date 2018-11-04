#pragma once

#include "../Interfaces/PipelineState.h"



void SerializePipelines(const TArray<IGraphicsPipelineState*>& GFXPipelines, const TArray<IComputePipelineState*>& CMPPipelines);

void DeserializePipelines(TArray<IGraphicsPipelineState*>& GFXPipelines, const TArray<IComputePipelineState*>& CMPPipelines);



