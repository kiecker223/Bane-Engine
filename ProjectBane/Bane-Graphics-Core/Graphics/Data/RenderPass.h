#pragma once

#include "../Interfaces/GraphicsDevice.h"

class RenderMeshMaterialBucket;

typedef struct ALIGN_FOR_GPU_BUFFER CAMERA_CONSTANT_BUFFER_DATA {
	float4x4 View;
	float4x4 Projection;
	float3 Position;
	float ZResolution;
	float FarPlane;
} CAMERA_CONSTANT_BUFFER_DATA;

class IRenderPass
{
	struct RenderUserData
	{
		void* pData;
		uint32 DataLength;

		template<typename T>
		T* GetData() { return reinterpret_cast<T*>(pData); }
	};

	// Let it peek in and analyze what is happening in the frame
	// UserData is a pointer to potentially data I'd want to pass 
	// For example UI stuff
	virtual void PreFrame(IRenderTargetInfo* Target, const CAMERA_CONSTANT_BUFFER_DATA& cbData, RenderMeshMaterialBucket& InBucket, const RenderUserData& UserData) = 0;

	virtual void RenderFrame(IGraphicsDevice* pDevice, IGraphicsCommandContext* CommandContext) = 0;

	virtual void PostFrame(IGraphicsDevice* pDevice, IGraphicsCommandContext* CommandContext) = 0;
};

