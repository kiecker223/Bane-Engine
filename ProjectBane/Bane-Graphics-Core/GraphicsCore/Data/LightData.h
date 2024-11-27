#pragma once
#include "BaneMath.h" 

#define MAX_LIGHTS 100

typedef struct SHADER_ALIGNMENT DIRECTIONAL_LIGHT_DATA {
	fvec3 Direction;
	float Range;
	fvec3 Color;
	float Intensity;
} DIRECTIONAL_LIGHT_DATA;

typedef struct SHADER_ALIGNMENT POINT_LIGHT_DATA {
	fvec3 Position;
	float Range;
	fvec3 Color;
	float Intensity;
} POINT_LIGHT_DATA;

typedef struct SHADER_ALIGNMENT SPOTLIGHT_DATA {
	fvec3 Position;
	float Range;
	fvec3 Direction;
	float Intensity;
	fvec3 Color;
	float SpotAngle;
} SPOTLIGHT_DATA;
