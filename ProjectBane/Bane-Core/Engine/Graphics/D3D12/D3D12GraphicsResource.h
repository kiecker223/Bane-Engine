#pragma once

#include <d3d12.h>
#include "../Interfaces/GraphicsResources.h"
#include "../Interfaces/GraphicsCommandList.h"
#include "D3D12PipelineState.h"



inline bool D3D12ResourceStateIsRead(D3D12_RESOURCE_STATES ResourceStates)
{
	switch (ResourceStates)
	{
	case D3D12_RESOURCE_STATE_COMMON: // D3D12_RESOURCE_STATE_PRESENT
	case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER:
	case D3D12_RESOURCE_STATE_INDEX_BUFFER:
	case D3D12_RESOURCE_STATE_DEPTH_READ:
	case D3D12_RESOURCE_STATE_COPY_SOURCE:
	case D3D12_RESOURCE_STATE_GENERIC_READ:
	case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT: // D3D12_RESOURCE_STATE_PREDICATION
	case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE:
	case D3D12_RESOURCE_STATE_RESOLVE_SOURCE:
		return true;
	case D3D12_RESOURCE_STATE_RENDER_TARGET:
	case D3D12_RESOURCE_STATE_COPY_DEST:
	case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE:
	case D3D12_RESOURCE_STATE_STREAM_OUT:
	case D3D12_RESOURCE_STATE_DEPTH_WRITE:
	case D3D12_RESOURCE_STATE_RESOLVE_DEST:
	case D3D12_RESOURCE_STATE_UNORDERED_ACCESS:
		return false;
	default:
		return false;
	}
}

inline bool D3D12ResourceStateIsWrite(D3D12_RESOURCE_STATES ResourceStates)
{
	switch (ResourceStates)
	{
	case D3D12_RESOURCE_STATE_COMMON: // D3D12_RESOURCE_STATE_PRESENT
	case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER:
	case D3D12_RESOURCE_STATE_INDEX_BUFFER:
	case D3D12_RESOURCE_STATE_DEPTH_READ:
	case D3D12_RESOURCE_STATE_COPY_SOURCE:
	case D3D12_RESOURCE_STATE_GENERIC_READ:
	case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE:
	case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT: // D3D12_RESOURCE_STATE_PREDICATION
	case D3D12_RESOURCE_STATE_UNORDERED_ACCESS:
		return false;
	case D3D12_RESOURCE_STATE_RENDER_TARGET:
	case D3D12_RESOURCE_STATE_COPY_DEST:
	case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE:
	case D3D12_RESOURCE_STATE_STREAM_OUT:
	case D3D12_RESOURCE_STATE_DEPTH_WRITE:
		return true;
	default:
		return false;
	}
}

inline bool D3D12ResourceStateIsReadWrite(D3D12_RESOURCE_STATES ResourceStates)
{
	switch (ResourceStates)
	{
	case D3D12_RESOURCE_STATE_UNORDERED_ACCESS:
		return true;
	default:
		return false;
	}
}


class D3D12GraphicsDevice;
class D3D12GraphicsCommandContext;
class D3D12ComputeCommandContext;
class D3D12GPUResource;

typedef struct D3D12_RESOURCE_TRANSITION
{
	D3D12_RESOURCE_STATES PrevState;
	D3D12_RESOURCE_STATES AfterState;
	D3D12GPUResource* Resource;
} D3D12_RESOURCE_TRANSITION;

typedef struct D3D12_UAV_BARRIER
{
	D3D12GPUResource* Resource;
} D3D12_UAV_BARRIER;

// This is if we need to reference the command list when we for instance, call a copy call
class D3D12DeviceChild
{
public:

	D3D12DeviceChild() { }

	D3D12DeviceChild(D3D12GraphicsDevice* InDevice) : m_Device(InDevice) { }

	D3D12GraphicsDevice* GetParentDevice() { return m_Device; }

	void SetParentDevice(D3D12GraphicsDevice* InDevice)
	{
		m_Device = InDevice;
	}

private:

	D3D12GraphicsDevice* m_Device;
};

class D3D12ResourceLocation : public D3D12DeviceChild
{

public:

	D3D12ResourceLocation() : D3DResource(nullptr) { }

	D3D12ResourceLocation(uint InSizeInBytes, ID3D12Resource* InResource, D3D12GraphicsDevice* InDevice) :
		SizeInBytes(InSizeInBytes),
		D3DResource(InResource),
		D3D12DeviceChild(InDevice)
	{
		// TODO: Debug me
		D3DResource->AddRef();
	}

	~D3D12ResourceLocation()
	{
		if (D3DResource)
			D3DResource->Release();
	}

	inline void SetResource(ID3D12Resource* InResource)
	{
		if (D3DResource)
			D3DResource->Release();
		D3DResource = InResource;
	}

	inline void SetDebugName(const std::string& InDebugName)
	{
		DebugName = InDebugName;
		{
			std::wstring WDebugName(DebugName.begin(), DebugName.end());
			if (D3DResource)
			{
				D3DResource->SetName(WDebugName.c_str());
			}
		}
	}

	D3D12_GPU_VIRTUAL_ADDRESS Location;
	D3D12_SRV_DIMENSION SRVDimension;
	D3D12_UAV_DIMENSION UAVDimension;
	ID3D12Resource* D3DResource;
	uint SizeInBytes;

	std::string DebugName;
};

#define D3D12_RESOURCE_STATE_INVALID_STATE (D3D12_RESOURCE_STATES)-1

class NO_VTABLE D3D12GPUResource : public IGPUResource
{
public:
	D3D12_RESOURCE_STATES PendingState;
	D3D12_RESOURCE_STATES CurrentState;
	D3D12ResourceLocation Resource;
	void* MappedPointer;

	D3D12GPUResource() : CurrentState(D3D12_RESOURCE_STATE_COMMON), PendingState(D3D12_RESOURCE_STATE_INVALID_STATE), ResourceOwnership(COMMAND_CONTEXT_TYPE_INVALID) { }

	D3D12GPUResource(ID3D12Resource* InResource, D3D12_RESOURCE_STATES InitialState, D3D12GraphicsDevice* InDevice) 
	{
	}

	inline bool CheckResourceTransitionValid(D3D12_RESOURCE_STATES NewState)
	{
		if (PendingState == NewState)
		{
			return false; // Invalid transition
		}
		if (CurrentState == NewState)
		{
			return false; // No need
		}
		return true; // Go ahead and transition
	}

	inline bool HasContextDependency() const
	{
		return ResourceOwnership != COMMAND_CONTEXT_TYPE_INVALID;
	}

	inline bool HasGraphicsDependency() const
	{
		return ResourceOwnership == COMMAND_CONTEXT_TYPE_GRAPHICS;
	}

	inline bool HasComputeDependency() const
	{
		return ResourceOwnership == COMMAND_CONTEXT_TYPE_COMPUTE;
	}

	// Returns true if there was already a dependency
	// Returns false otherwise
	// Essentially:  if(RegisterDependency(...)) { IsADependencyAndNeedsWaitCommand = true; }
	inline bool RegisterDependency(ECOMMAND_CONTEXT_TYPE NewDependency)
	{
		if (ResourceOwnership == NewDependency)
		{
			// We commited this resource to the same gpu engine, rely on resource barriers?
			return false;
		}
		if (ResourceOwnership == COMMAND_CONTEXT_TYPE_INVALID)
		{
			ResourceOwnership = NewDependency;
			return false;
		}
		if (ResourceOwnership != COMMAND_CONTEXT_TYPE_INVALID)
		{
			ResourceOwnership = NewDependency;
			return true;
		}
		return false;
	}

	inline void Uncommit()
	{
		ResourceOwnership = COMMAND_CONTEXT_TYPE_INVALID;
	}

	void TransitionResource(D3D12GraphicsCommandContext* Ctx, D3D12_RESOURCE_STATES NewState);
	void TransitionResource(D3D12ComputeCommandContext* Ctx, D3D12_RESOURCE_STATES NewState);
	void PushUAVBarrier(D3D12GraphicsCommandContext* Ctx);
	void PushUAVBarrier(D3D12ComputeCommandContext* Ctx);

protected:

	ECOMMAND_CONTEXT_TYPE ResourceOwnership;
};

class D3D12Buffer : public IBuffer, public D3D12GPUResource
{
public:

	D3D12Buffer(D3D12GraphicsDevice* InDevice, uint InSize, EBUFFER_USAGE InUsage);

	void UploadDataToGPU(D3D12GraphicsCommandContext* Ctx, uint8* Buffer);

	virtual EBUFFER_USAGE GetUsage() const final override { return Usage; }
	virtual uint GetSizeInBytes() const final override { return SizeInBytes; }

	void SetDebugName(const std::string& DebugName) override
	{
		Resource.SetDebugName(DebugName);
	}

	std::string GetDebugName() const override
	{
		return Resource.DebugName;
	}

	D3D12_VERTEX_BUFFER_VIEW GetVBView(IInputLayout* Layout) const
	{
		D3D12_VERTEX_BUFFER_VIEW View = { };
		View.BufferLocation = GetGPUVirtualAddress();
		View.SizeInBytes = SizeInBytes;
		View.StrideInBytes = GetLayoutStride(Layout);
		return View;
	}

	D3D12_INDEX_BUFFER_VIEW GetIBView() const
	{
		D3D12_INDEX_BUFFER_VIEW View = { };
		View.BufferLocation = GetGPUVirtualAddress();
		View.Format = DXGI_FORMAT_R32_UINT;
		View.SizeInBytes = SizeInBytes;
		return View;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
	{
		return Resource.Location;
	}

	uint SizeInBytes;
	EBUFFER_USAGE Usage;
};



class D3D12TextureBase : public ITextureBase, public D3D12GPUResource
{
	friend class D3D12GraphicsDevice;

	D3D12TextureBase() { }

public:
	
	D3D12TextureBase(D3D12GraphicsDevice* InDevice, uint InWidth, uint InHeight, uint InDepth, uint InCount, EFORMAT InFormat, ETEXTURE_USAGE InUsage);

	void UploadToGPU(D3D12GraphicsCommandContext* Ctx, const void* Pointer, const uint Width, const uint Height, const uint Depth, const uint StepSize);

	virtual uint GetWidth() const final override { return Width; }
	virtual uint GetHeight() const final override { return Height; }
	virtual uint GetDepth() const final override { return Depth; }
	virtual uint GetArraySize() const final override { return ArrayCount; }
	virtual ETEXTURE_USAGE GetUsage() const final override { return Usage; }
	virtual EFORMAT GetFormat() const final override { return Format; }
	virtual uint GetMipCount() const final override { return MipCount; }

	void SetDebugName(const std::string& DebugName) override
	{
		Resource.SetDebugName(DebugName);
	}

	std::string GetDebugName() const override
	{
		return Resource.DebugName;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
	{
		return Resource.Location;
	}
	
	uint Width;
	uint Height;
	uint Depth;
	uint ArrayCount;
	uint MipCount;
	EFORMAT Format;
	ETEXTURE_USAGE Usage;
};


class D3D12SamplerState : public ISamplerState
{
public:

	D3D12SamplerState(D3D12_SAMPLER_DESC InCreationDesc, SAMPLER_DESC InDesc) :
		Desc(InDesc),
		CreationDesc(InCreationDesc)
	{
	}

	virtual SAMPLER_DESC GetDesc() const final override { return Desc; }

	SAMPLER_DESC Desc;
	D3D12_SAMPLER_DESC CreationDesc;
};

