#pragma once

#include "KieckerMath.h"
#include "../Interfaces/ShaderResourceView.h"

typedef struct CAMERA_DATA {
	IRenderPassInfo* CurrentRenderPass;
	float4x4 View;
	float4x4 Projection;
	double3 Position;
	float ZResolution;
	float FarPlane;
} CAMERA_DATA;