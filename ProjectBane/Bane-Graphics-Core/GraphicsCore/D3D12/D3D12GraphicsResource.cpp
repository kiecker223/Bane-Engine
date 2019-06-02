#include "D3D12GraphicsResource.h"
#include "D3D12GraphicsDevice.h"
#include "D3D12Translator.h"
#include "Platform/System/Logging/Logger.h"
#include "D3D12Helper.h"



void D3D12GPUResource::TransitionResource(D3D12GraphicsCommandBuffer* Ctx, D3D12_RESOURCE_STATES NewState)
{
	if (CheckResourceTransitionValid(NewState))
	{
		D3D12_RESOURCE_BARRIER Transition = { };
		Transition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Transition.Transition.StateBefore = PromotedState;
		Transition.Transition.StateAfter = NewState;
		PendingState = NewState;
		Transition.Transition.pResource = Resource.D3DResource;
		Ctx->PendingTransitions.push_back(Transition);
		Ctx->TransitionedResources.push_back(this);
	}
}

void D3D12GPUResource::TransitionResource(D3D12ComputeCommandContext* Ctx, D3D12_RESOURCE_STATES NewState)
{
	if (CheckResourceTransitionValid(NewState))
	{
		D3D12_RESOURCE_BARRIER Transition = { };
		Transition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Transition.Transition.StateBefore = PromotedState;
		Transition.Transition.StateAfter = NewState;
		PendingState = NewState;
		Transition.Transition.pResource = Resource.D3DResource;
		Ctx->PendingTransitions.push_back(Transition);
		Ctx->TransitionedResources.push_back(this);
	}
}

void D3D12GPUResource::PushUAVBarrier(D3D12GraphicsCommandBuffer* Ctx)
{
	D3D12_RESOURCE_BARRIER Transition = { };
	Transition.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	Transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	Transition.UAV.pResource = Resource.D3DResource;
	Ctx->PendingTransitions.push_back(Transition);
}

void D3D12GPUResource::PushUAVBarrier(D3D12ComputeCommandContext* Ctx)
{
	D3D12_RESOURCE_BARRIER Transition = { };
	Transition.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	Transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	Transition.UAV.pResource = Resource.D3DResource;
	Ctx->PendingTransitions.push_back(Transition);
}

D3D12Buffer::D3D12Buffer(D3D12GraphicsDevice* InDevice, uint64 InSize, EBUFFER_USAGE InUsage) :
	SizeInBytes(InSize),
	Usage(InUsage)
{
	Resource.SetParentDevice(InDevice);
	
	D3D12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(InSize);
	D3D12_HEAP_PROPERTIES HeapProps;

	ID3D12Device1* Device = InDevice->GetDevice();

	D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_INVALID_STATE;

	switch (Usage)
	{
	case BUFFER_USAGE_GPU:
	{
		HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		InitialState = D3D12_RESOURCE_STATE_PRESENT;
	} break;
	case BUFFER_USAGE_CPU:
	{
		HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		InitialState = D3D12_RESOURCE_STATE_GENERIC_READ;
	} break;
	case BUFFER_USAGE_UPLOAD:
	{
		HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		InitialState = D3D12_RESOURCE_STATE_GENERIC_READ;
	} break;
	}

	D3D12ERRORCHECK(
		Device->CreateCommittedResource(
			&HeapProps,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			InitialState,
			nullptr,
			IID_PPV_ARGS(&Resource.D3DResource)
		)
	);
	Resource.Location = Resource.D3DResource->GetGPUVirtualAddress();
	Resource.SRVDimension = D3D12_SRV_DIMENSION_BUFFER;
	Resource.UAVDimension = D3D12_UAV_DIMENSION_BUFFER;
	if (HeapProps.Type == D3D12_HEAP_TYPE_UPLOAD)
	{
		Resource.D3DResource->Map(0, nullptr, &MappedPointer);
	}
	else
	{
		MappedPointer = nullptr;
	}
	PromotedState = InitialState;
	CurrentState = InitialState;
}

void D3D12Buffer::UploadDataToGPU(D3D12GraphicsCommandContext* Ctx, uint8* Buffer)
{
	D3D12Buffer* UploadBuffer = new D3D12Buffer(Resource.GetParentDevice(), SizeInBytes, BUFFER_USAGE_UPLOAD);

	memcpy(UploadBuffer->MappedPointer, Buffer, static_cast<uint32>(SizeInBytes));
	Ctx->CopyBuffers(UploadBuffer, this);
	Ctx->CurrentCommandBuffer->CommandList->EnqueueUploadResourceToDestroy(dynamic_cast<D3D12GPUResource*>(UploadBuffer));
}

void* D3D12Buffer::Map()
{
	if (MappedPointer != nullptr)
	{
		return MappedPointer;
	}
	if (FAILED(Resource.D3DResource->Map(0, nullptr, &MappedPointer)))
	{
		__debugbreak();
	}
	return MappedPointer;
}

void D3D12Buffer::Unmap()
{
	if (MappedPointer == nullptr)
	{
		Resource.D3DResource->Unmap(0, nullptr);
	}
	else
	{
		return;
	}
}

bool D3D12Buffer::CheckResourceTransitionValid(D3D12_RESOURCE_STATES NewState)
{
	if (NewState == CurrentState || NewState == PendingState || PromotedState == NewState || Usage == BUFFER_USAGE_UPLOAD)
	{
		return false;
	}
	return true;
}


D3D12TextureBase::D3D12TextureBase(D3D12GraphicsDevice* InDevice, uint32 InWidth, uint32 InHeight, uint32 InDepth, uint32 InCount, 
									EFORMAT InFormat, const SAMPLER_DESC& InSampleDesc, const D3D12_SAMPLER_DESC& InD3DSampler, ETEXTURE_USAGE InUsage) :
	Width(InWidth),
	Height(InHeight),
	Depth(InDepth),
	ArrayCount(InCount),
	Format(InFormat),
	SamplerDesc(InSampleDesc),
	D3DSampleDesc(InD3DSampler),
	Usage(InUsage)
{
	Resource.SetParentDevice(InDevice);
	MipCount = 1;
	D3D12_RESOURCE_DESC ResourceDesc = { };
	D3D12_RESOURCE_FLAGS AdditionalFlags = D3D12_RESOURCE_FLAG_NONE;
	D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_COMMON;

	D3D12_CLEAR_VALUE ClearValue;

	if (IsShaderResource())
	{
		MipCount = static_cast<uint32>(std::floor(std::log2(max(InWidth, InHeight))) + 1);
	}
	if (IsUnorderedAccess())
	{
		AdditionalFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		InitialState = D3D12_RESOURCE_STATE_COMMON;
	}
	if (IsRenderTarget())
	{
		AdditionalFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		InitialState = D3D12_RESOURCE_STATE_RENDER_TARGET;
		MipCount = 1;
		static const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		ClearValue = CD3DX12_CLEAR_VALUE(D3D_TranslateFormat(InFormat), ClearColor);
	}
	else if (IsDepthStencil())
	{
		AdditionalFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		InitialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		ClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0);
	}

	if (InDepth == 1)
	{
		uint32 DepthOrArraySize;
		if (InCount > 1)
		{
			DepthOrArraySize = InCount;
		}
		else
		{
			DepthOrArraySize = InDepth;
		}
		ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(D3D_TranslateFormat(Format), Width, Height, static_cast<uint16>(DepthOrArraySize), static_cast<uint16>(MipCount));
	}
	else if (InDepth > 1)
	{
		ResourceDesc = CD3DX12_RESOURCE_DESC::Tex3D(D3D_TranslateFormat(Format), Width, Height, static_cast<uint16>(Depth), static_cast<uint16>(MipCount));
	}

	ResourceDesc.Flags = AdditionalFlags;
	ID3D12Device1* Device = InDevice->GetDevice();

	D3D12_HEAP_PROPERTIES HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_CLEAR_VALUE* pClearValue = ((Usage & TEXTURE_USAGE_DEPTH_STENCIL) == TEXTURE_USAGE_DEPTH_STENCIL || 
									 ((Usage & TEXTURE_USAGE_RENDER_TARGET) == TEXTURE_USAGE_RENDER_TARGET)) ? &ClearValue : nullptr;
	
 	D3D12ERRORCHECK(
 		Device->CreateCommittedResource(
 			&HeapProps,
 			D3D12_HEAP_FLAG_NONE,
 			&ResourceDesc,
 			InitialState,
 			pClearValue,
 			IID_PPV_ARGS(&Resource.D3DResource)
 		)
 	);
	CurrentState = InitialState;
	PromotedState = InitialState;
}

static uint32 UploadResourceId = 0;

void D3D12TextureBase::SetSamplerDesc(const SAMPLER_DESC& InSampleDesc)
{
	SamplerDesc = InSampleDesc;
	D3DSampleDesc = D3D12_TranslateSamplerDesc(InSampleDesc);
}

void D3D12TextureBase::UploadToGPU(D3D12GraphicsCommandContext* Ctx, const void* Pointer, const uint32 InWidth, const uint32 InHeight, const uint32 InDepth, const uint32 StepSize)
{
	// Special treatment of arrays
	if (ArrayCount > 1)
	{
		const uint8* Ptr = (const uint8*)Pointer;
		auto Desc = Resource.D3DResource->GetDesc();
		const uint32 Size = (InWidth * InHeight * InDepth) * StepSize;
		const uint32 ImgSize = (InWidth * InHeight * StepSize);
		D3D12Buffer* UploadBuffer = new D3D12Buffer(Resource.GetParentDevice(), Size, BUFFER_USAGE_UPLOAD);
		memcpy(UploadBuffer->MappedPointer, Pointer, Size);
		for (uint32 i = 0; i < ArrayCount; i++)
		{
			D3D12_SUBRESOURCE_DATA ResourceData = { };
			ResourceData.pData = (const void*)&Ptr[ImgSize * i];
			ResourceData.RowPitch = Width * StepSize;
			ResourceData.SlicePitch = ImgSize;
			UpdateSubresources<1>(Ctx->CurrentCommandBuffer->D3DCL, Resource.D3DResource, UploadBuffer->Resource.D3DResource, ImgSize * i, Desc.MipLevels * i, 1, &ResourceData);
			Ctx->CurrentCommandBuffer->NumCopies++;
		}
		PromotedState = D3D12_RESOURCE_STATE_COPY_DEST;
		Ctx->CurrentCommandBuffer->CommandList->EnqueueUploadResourceToDestroy(dynamic_cast<D3D12GPUResource*>(UploadBuffer));
	}
	else
	{
		uint64 RequiredSize = GetRequiredIntermediateSize(Resource.D3DResource, 0, 1);
		D3D12Buffer* UploadBuffer = new D3D12Buffer(Resource.GetParentDevice(), RequiredSize, BUFFER_USAGE_UPLOAD);
		UploadBuffer->SetDebugName("Upload Resource: " + std::to_string(UploadResourceId++));
		D3D12_SUBRESOURCE_DATA ResourceData = { };
		ResourceData.pData = Pointer;
		ResourceData.RowPitch = Width * StepSize;
		ResourceData.SlicePitch = (Width * Height) * StepSize;
		UpdateSubresources<1>(Ctx->CurrentCommandBuffer->CommandList->GetGraphicsCommandList(), Resource.D3DResource, UploadBuffer->Resource.D3DResource, 0, 0, 1, &ResourceData);
		Ctx->CurrentCommandBuffer->NumCopies++;
		PromotedState = D3D12_RESOURCE_STATE_COPY_DEST;
		Ctx->CurrentCommandBuffer->CommandList->EnqueueUploadResourceToDestroy(dynamic_cast<D3D12GPUResource*>(UploadBuffer));
	}
}

void* D3D12TextureBase::Map()
{
	if (MappedPointer != nullptr)
	{
		return MappedPointer;
	}
	if (FAILED(Resource.D3DResource->Map(0, nullptr, &MappedPointer)))
	{
		__debugbreak();
	}
	return MappedPointer;
}

void D3D12TextureBase::Unmap()
{
	if (MappedPointer == nullptr)
	{
		Resource.D3DResource->Unmap(0, nullptr);
	}
	else
	{
		return;
	}
}

bool D3D12TextureBase::CheckResourceTransitionValid(D3D12_RESOURCE_STATES NewState)
{
	if (NewState == CurrentState || NewState == PendingState || NewState == PromotedState)
	{
		return false;
	}
	return true;
}
