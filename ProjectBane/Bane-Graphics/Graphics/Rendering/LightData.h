#pragma once

#include "KieckerMath.h"


typedef struct DIRECTIONAL_LIGHT_DATA {
	float3 Direction;
	float Range;
	float3 Color;
	float Intensity;
} DIRECTIONAL_LIGHT_DATA;

typedef struct POINT_LIGHT_DATA {
	float3 Position;
	float Range;
	float3 Color;
	float Intensity;
} POINT_LIGHT_DATA;

typedef struct SPOTLIGHT_DATA {
	float3 Position;
	float Range;
	float3 Direction;
	float Intensity;
	float3 Color;
	float SpotAngle;
} SPOTLIGHT_DATA;
