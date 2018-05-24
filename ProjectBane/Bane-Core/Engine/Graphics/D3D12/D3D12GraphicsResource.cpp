#include "D3D12GraphicsResource.h"
#include "D3D12GraphicsDevice.h"
#include "D3D12Translator.h"
#include "Core/Systems/Logging/Logger.h"
#include "D3D12Helper.h"


void D3D12GPUResource::TransitionResource(D3D12GraphicsCommandContext* Ctx, D3D12_RESOURCE_STATES NewState)
{
	if (CheckResourceTransitionValid(NewState))
	{
		D3D12_RESOURCE_BARRIER Transition = { };
		Transition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Transition.Transition.StateBefore = CurrentState;
		Transition.Transition.StateAfter = NewState;
		PendingState = NewState;
		Transition.Transition.pResource = Resource.D3DResource;
		Ctx->PendingTransitions.Push(Transition);
		Ctx->TransitionedResources.Push(this);
	}
}

void D3D12GPUResource::TransitionResource(D3D12ComputeCommandContext* Ctx, D3D12_RESOURCE_STATES NewState)
{
	if (CheckResourceTransitionValid(NewState))
	{
		D3D12_RESOURCE_BARRIER Transition = { };
		Transition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Transition.Transition.StateBefore = CurrentState;
		Transition.Transition.StateAfter = NewState;
		PendingState = NewState;
		Transition.Transition.pResource = Resource.D3DResource;
		Ctx->PendingTransitions.Push(Transition);
		Ctx->TransitionedResources.Push(this);
	}
}

void D3D12GPUResource::PushUAVBarrier(D3D12GraphicsCommandContext* Ctx)
{
	D3D12_RESOURCE_BARRIER Transition = { };
	Transition.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	Transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	Transition.UAV.pResource = Resource.D3DResource;
	Ctx->PendingTransitions.Push(Transition);
}

void D3D12GPUResource::PushUAVBarrier(D3D12ComputeCommandContext* Ctx)
{
	D3D12_RESOURCE_BARRIER Transition = { };
	Transition.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	Transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	Transition.UAV.pResource = Resource.D3DResource;
	Ctx->PendingTransitions.Push(Transition);
}

D3D12Buffer::D3D12Buffer(D3D12GraphicsDevice* InDevice, uint InSize, EBUFFER_USAGE InUsage) :
	SizeInBytes(InSize),
	Usage(InUsage)
{
	Resource.SetParentDevice(InDevice);
	
	D3D12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(InSize);
	D3D12_HEAP_PROPERTIES HeapProps;

	ID3D12Device1* Device = InDevice->GetDevice();

	D3D12_RESOURCE_STATES InitialState;

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
	
	if (HeapProps.Type == D3D12_HEAP_TYPE_UPLOAD)
	{
		Resource.D3DResource->Map(0, nullptr, &MappedPointer);
	}
	else
	{
		MappedPointer = nullptr;
	}

	CurrentState = InitialState;
}

void D3D12Buffer::UploadDataToGPU(D3D12GraphicsCommandContext* Ctx, uint8* Buffer)
{
	D3D12Buffer* UploadBuffer = new D3D12Buffer(Resource.GetParentDevice(), SizeInBytes, BUFFER_USAGE_UPLOAD);

	memcpy(UploadBuffer->MappedPointer, Buffer, SizeInBytes);
	Ctx->CopyBuffers(UploadBuffer, this);
	Ctx->CommandList->EnqueueUploadResourceToDestroy(dynamic_cast<D3D12GPUResource*>(UploadBuffer));
}


D3D12TextureBase::D3D12TextureBase(D3D12GraphicsDevice* InDevice, uint InWidth, uint InHeight, uint InDepth, uint InCount, EFORMAT InFormat, ETEXTURE_USAGE InUsage) :
	Width(InWidth),
	Height(InHeight),
	Depth(InDepth),
	ArrayCount(InCount),
	Format(InFormat),
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
		MipCount = std::floor(std::log2(max(InWidth, InHeight))) + 1;
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
		uint DepthOrArraySize;
		if (InCount > 1)
		{
			DepthOrArraySize = InCount;
		}
		else
		{
			DepthOrArraySize = InDepth;
		}
		ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(D3D_TranslateFormat(Format), Width, Height, DepthOrArraySize, MipCount);
	}
	else if (InDepth > 1)
	{
		ResourceDesc = CD3DX12_RESOURCE_DESC::Tex3D(D3D_TranslateFormat(Format), Width, Height, Depth, MipCount);
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
}


void D3D12TextureBase::UploadToGPU(D3D12GraphicsCommandContext* Ctx, const void* Pointer, const uint InWidth, const uint InHeight, const uint InDepth, const uint StepSize)
{
	// Special treatment of arrays
	if (ArrayCount > 1)
	{
		const uint8* Ptr = (const uint8*)Pointer;
		auto Desc = Resource.D3DResource->GetDesc();
		const uint Size = (InWidth * InHeight * InDepth) * StepSize;
		const uint ImgSize = (InWidth * InHeight * StepSize);
		D3D12Buffer* UploadBuffer = new D3D12Buffer(Resource.GetParentDevice(), Size, BUFFER_USAGE_UPLOAD);
		memcpy(UploadBuffer->MappedPointer, Pointer, Size);
		for (uint i = 0; i < ArrayCount; i++)
		{
			
			D3D12_SUBRESOURCE_DATA ResourceData = { };
			ResourceData.pData = (const void*)&Ptr[ImgSize * i];
			ResourceData.RowPitch = Width * StepSize;
			ResourceData.SlicePitch = ImgSize;
			UpdateSubresources<1>(Ctx->D3DCL, Resource.D3DResource, UploadBuffer->Resource.D3DResource, ImgSize * i, Desc.MipLevels * i, 1, &ResourceData);
		}
		Ctx->CommandList->EnqueueUploadResourceToDestroy(dynamic_cast<D3D12GPUResource*>(UploadBuffer));
	}
	else
	{
		uint RequiredSize = GetRequiredIntermediateSize(Resource.D3DResource, 0, 1);
		D3D12Buffer* UploadBuffer = new D3D12Buffer(Resource.GetParentDevice(), RequiredSize, BUFFER_USAGE_UPLOAD);
		D3D12_SUBRESOURCE_DATA ResourceData = { };
		ResourceData.pData = Pointer;
		ResourceData.RowPitch = Width * StepSize;
		ResourceData.SlicePitch = (Width * Height) * StepSize;
		UpdateSubresources<1>(Ctx->CommandList->GetGraphicsCommandList(), Resource.D3DResource, UploadBuffer->Resource.D3DResource, 0, 0, 1, &ResourceData);
		Ctx->CommandList->EnqueueUploadResourceToDestroy(dynamic_cast<D3D12GPUResource*>(UploadBuffer));
	}
}

