#pragma once

#include "KieckerMath.h"
#include "../Interfaces/ShaderResourceView.h"

typedef struct CAMERA_DATA {
	IRenderPassInfo* CurrentRenderPass;
	matrix View;
	matrix Projection;
	float3 Position;
} CAMERA_DATA;