#pragma once

#include "KieckerMath.h"
#include "../Interfaces/ShaderResourceView.h"

typedef struct CAMERA_DATA {
	fmat4x4 View;
	fmat4x4 Projection;
	vec3 Position;
	float ZResolution;
	float FarPlane;
} CAMERA_DATA;

typedef struct CAMERA_CBUFFER_DATA {
	fmat4x4 View;
	fmat4x4 Projection;
	fvec3 Position;
	float ZResolution;
	float FarPlane;
} CAMERA_CBUFFER_DATA;

inline CAMERA_CBUFFER_DATA GetCBufferCameraDataFromAcquireData(const CAMERA_DATA& InData)
{
	CAMERA_CBUFFER_DATA Result = { InData.View, InData.Projection, fromDouble3(InData.Position), InData.ZResolution, InData.FarPlane };
	return Result;
}