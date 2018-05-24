#pragma once

#include "Core/ExpanseMath.h"


typedef struct DIRECTIONAL_LIGHT_DATA {
	XMFLOAT3 Direction;
	float Range;
	XMFLOAT3 Color;
	float Intensity;
} DIRECTIONAL_LIGHT_DATA;

typedef struct POINT_LIGHT_DATA {
	XMFLOAT3 Position;
	float Range;
	XMFLOAT3 Color;
	float Intensity;
} POINT_LIGHT_DATA;

typedef struct SPOTLIGHT_DATA {
	XMFLOAT3 Position;
	float Range;
	XMFLOAT3 Direction;
	float Intensity;
	XMFLOAT3 Color;
	float SpotAngle;
} SPOTLIGHT_DATA;
