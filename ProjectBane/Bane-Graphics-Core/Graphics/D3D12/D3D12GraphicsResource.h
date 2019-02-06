#pragma once

#include <d3d12.h>
#include "../Interfaces/GraphicsResources.h"
#include "../Interfaces/GraphicsCommandList.h"
#include "D3D12PipelineState.h"
#include <iostream>


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
class D3D12GraphicsCommandBuffer;
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

	D3D12ResourceLocation(uint32 InSizeInBytes, ID3D12Resource* InResource, D3D12GraphicsDevice* InDevice) :
		SizeInBytes(InSizeInBytes),
		D3DResource(InResource),
		D3D12DeviceChild(InDevice)
	{
	}

	~D3D12ResourceLocation()
	{
		if (D3DResource)
		{
			ULONG RefCount = D3DResource->Release();
			while (RefCount) 
			{
				std::cout << "RefCount: " << RefCount << std::endl; 
				RefCount = D3DResource->Release();
			}
			D3DResource = nullptr;
			std::cout << "Destroying" << std::endl;
		}
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
	uint32 SizeInBytes;

	std::string DebugName;
};

#define D3D12_RESOURCE_STATE_INVALID_STATE (D3D12_RESOURCE_STATES)-1

typedef enum ED3D12_RESOURCE_TYPE {
	D3D12_RESOURCE_TYPE_BUFFER,
	D3D12_RESOURCE_TYPE_TEXTURE
} ED3D12_RESOURCE_TYPE;

class NO_VTABLE D3D12GPUResource : public IGPUResource
{
public:
	// @note: pending state to transition to
	D3D12_RESOURCE_STATES PendingState;
	
	// @note: How the GPU actually views this resource
	D3D12_RESOURCE_STATES PromotedState;

	// @note: How the runtime interprets the resource
	D3D12_RESOURCE_STATES CurrentState;


	D3D12ResourceLocation Resource;
	void* MappedPointer;

	D3D12GPUResource() : CurrentState(D3D12_RESOURCE_STATE_COMMON), PendingState(D3D12_RESOURCE_STATE_INVALID_STATE), PromotedState(D3D12_RESOURCE_STATE_COMMON), ResourceOwnership(COMMAND_CONTEXT_TYPE_INVALID) { }
	virtual ~D3D12GPUResource() 
	{
	}

	virtual bool CheckResourceTransitionValid(D3D12_RESOURCE_STATES NewState) = 0;

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

	virtual ED3D12_RESOURCE_TYPE GetResourceType() const = 0;
	
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

	void TransitionResource(D3D12GraphicsCommandBuffer* Ctx, D3D12_RESOURCE_STATES NewState);
	void TransitionResource(D3D12ComputeCommandContext* Ctx, D3D12_RESOURCE_STATES NewState);
	void PushUAVBarrier(D3D12GraphicsCommandBuffer* Ctx);
	void PushUAVBarrier(D3D12ComputeCommandContext* Ctx);

protected:

	ECOMMAND_CONTEXT_TYPE ResourceOwnership;
};

class D3D12Buffer : public IBuffer, public D3D12GPUResource
{
public:

	D3D12Buffer(D3D12GraphicsDevice* InDevice, uint64 InSize, EBUFFER_USAGE InUsage);

	void UploadDataToGPU(D3D12GraphicsCommandContext* Ctx, uint8* Buffer);

	void* Map() final override;
	void Unmap() final override;

	virtual EBUFFER_USAGE GetUsage() const final override { return Usage; }
	virtual uint32 GetSizeInBytes() const final override { return static_cast<uint32>(SizeInBytes); }

	void SetDebugName(const std::string& DebugName) override
	{
		Resource.SetDebugName(DebugName);
	}

	bool CheckResourceTransitionValid(D3D12_RESOURCE_STATES NewState) override final;

	virtual ED3D12_RESOURCE_TYPE GetResourceType() const
	{
		return D3D12_RESOURCE_TYPE_BUFFER;
	}

	std::string GetDebugName() const override
	{
		return Resource.DebugName;
	}

	D3D12_VERTEX_BUFFER_VIEW GetVBView(IInputLayout* Layout) const
	{
		D3D12_VERTEX_BUFFER_VIEW View = { };
		View.BufferLocation = GetGPUVirtualAddress();
		View.SizeInBytes = GetSizeInBytes();
		View.StrideInBytes = GetLayoutStride(Layout);
		return View;
	}

	D3D12_INDEX_BUFFER_VIEW GetIBView() const
	{
		D3D12_INDEX_BUFFER_VIEW View = { };
		View.BufferLocation = GetGPUVirtualAddress();
		View.Format = DXGI_FORMAT_R32_UINT;
		View.SizeInBytes = GetSizeInBytes();
		return View;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
	{
		return Resource.Location;
	}

	uint64 SizeInBytes;
	EBUFFER_USAGE Usage;
};



class D3D12TextureBase : public ITextureBase, public D3D12GPUResource
{
	friend class D3D12GraphicsDevice;

	D3D12TextureBase() 
	{
	}

public:
	
	D3D12TextureBase(D3D12GraphicsDevice* InDevice, uint32 InWidth, uint32 InHeight, uint32 InDepth, uint32 InCount, 
					 EFORMAT InFormat, const SAMPLER_DESC& InSamplerDesc, const D3D12_SAMPLER_DESC& InD3DSampler, ETEXTURE_USAGE InUsage);

	void UploadToGPU(D3D12GraphicsCommandContext* Ctx, const void* Pointer, const uint32 Width, const uint32 Height, const uint32 Depth, const uint32 StepSize);

	void* Map() final override;
	void Unmap() final override;
	bool CheckResourceTransitionValid(D3D12_RESOURCE_STATES NewState) override final;

	virtual uint32 GetWidth() const final override { return Width; }
	virtual uint32 GetHeight() const final override { return Height; }
	virtual uint32 GetDepth() const final override { return Depth; }
	virtual uint32 GetArraySize() const final override { return ArrayCount; }
	virtual ETEXTURE_USAGE GetUsage() const final override { return Usage; }
	virtual EFORMAT GetFormat() const final override { return Format; }
	virtual uint32 GetMipCount() const final override { return MipCount; }
	virtual void SetSamplerDesc(const SAMPLER_DESC& InSampleDesc) final override;

	virtual ED3D12_RESOURCE_TYPE GetResourceType() const
	{
		return D3D12_RESOURCE_TYPE_TEXTURE;
	}

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
	
	uint32 Width;
	uint32 Height;
	uint32 Depth;
	uint32 ArrayCount;
	uint32 MipCount;
	EFORMAT Format;
	ETEXTURE_USAGE Usage;
	SAMPLER_DESC SamplerDesc;
	D3D12_SAMPLER_DESC D3DSampleDesc;
};
