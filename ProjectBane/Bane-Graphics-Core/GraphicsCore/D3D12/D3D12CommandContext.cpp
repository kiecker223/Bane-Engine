#include "D3D12CommandContext.h"
#include "D3D12GraphicsDevice.h"
#include "D3D12GraphicsResource.h"
#include "D3D12Translator.h"
#include "D3D12ShaderResourceViews.h"
#include <DirectXColors.h>



void D3D12CommandList::FlushDestructionQueue()
{
	if (UploadResourcesToDestroy.empty() || !GetParentDevice()->GetCopyQueue().IsFinished())
	{
		return;
	}
	for (uint32 i = 0; i < UploadResourcesToDestroy.size(); i++)
	{
		auto& Resource = UploadResourcesToDestroy[i];
		Resource.Destroy();
	}
	UploadResourcesToDestroy.clear();
}

///--------- D3D12GraphicsCommandBuffer ---------///

/*
void D3D12GraphicsCommandBuffer::BeginPass(IRenderPass* InRenderPass)
{
	if (CommandList == nullptr)
	{
		FetchNewCommandList();
	}
	if (CommandList->bCanReset)
	{
		CommandList->Reset();
	}
	if (InRenderPass)
	{
		D3D12RenderPassInfo* RenderPass = (D3D12RenderPassInfo*)InRenderPass;
		ID3D12DescriptorHeap* ppHeaps[] = { ParentDevice->m_SrvAllocator.DescriptorHeap, ParentDevice->m_SmpAllocator.DescriptorHeap };
		D3DCL->SetDescriptorHeaps(2, ppHeaps);
		RenderPass->TransitionResourcesToWrite(this);
		FlushResourceTransitions();
		RenderPass->SetRenderTargets(D3DCL);
		RenderPass->Clear(D3DCL);
		D3DCL->RSSetScissorRects(1, &ScissorRect);
		D3DCL->RSSetViewports(1, &Viewport);
		CurrentRenderPass = RenderPass;
	}
}

void D3D12GraphicsCommandBuffer::EndPass()
{
	if (CurrentRenderPass)
	{
		CurrentRenderPass->TransitionResourcesToRead(this);
		FlushResourceTransitions();
		CurrentRenderPass = nullptr;
	}
	GraphicsPipelineState = nullptr;
	ComputePipelineState = nullptr;
}
*/

void D3D12GraphicsCommandBuffer::Begin()
{
	if (CommandList == nullptr)
	{
		FetchNewCommandList();
	}
	if (CommandList->bCanReset)
	{
		CommandList->Reset();
	}
	if (ContextType == COMMAND_CONTEXT_TYPE_GRAPHICS)
	{
		ID3D12DescriptorHeap* ppHeaps[] = { ParentDevice->m_SrvAllocator.DescriptorHeap, ParentDevice->m_SmpAllocator.DescriptorHeap };
		D3DCL->SetDescriptorHeaps(2, ppHeaps);
		D3DCL->RSSetScissorRects(1, &ScissorRect);
		D3DCL->RSSetViewports(1, &Viewport);
	}
	NumCurrentRenderTargetViews = 0;
}

void D3D12GraphicsCommandBuffer::SetRenderTarget(IRenderTargetView* pRenderTargetView, IDepthStencilView* pDepthStencilView)
{
	SetRenderTargets({ pRenderTargetView }, pDepthStencilView);
}

void D3D12GraphicsCommandBuffer::SetRenderTargets(const std::vector<IRenderTargetView*>& RenderTargetViews, IDepthStencilView* pDepthStencilView)
{
	if (NumCurrentRenderTargetViews > 0)
	{
		TransitionRenderTargetsToRead(CurrentRenderTargetViews, NumCurrentRenderTargetViews, CurrentDepthStencilView);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CpuDescriptors[32] = { };
	for (uint32 i = 0; i < RenderTargetViews.size(); i++)
	{
		CurrentRenderTargetViews[i] = reinterpret_cast<D3D12RenderTargetView*>(RenderTargetViews[i]);
		CpuDescriptors[i] = CurrentRenderTargetViews[i]->GetCurrentFrame().CpuHandle;
	}
	CurrentDepthStencilView = reinterpret_cast<D3D12DepthStencilView*>(pDepthStencilView);

	NumCurrentRenderTargetViews = static_cast<uint32>(RenderTargetViews.size());

	D3D12_CPU_DESCRIPTOR_HANDLE* DsvCpuDescriptor = nullptr;
	if (CurrentDepthStencilView)
	{
		DsvCpuDescriptor = &CurrentDepthStencilView->GetCurrentFrame().CpuHandle;
	}
	TransitionRenderTargetsToWrite(CurrentRenderTargetViews, NumCurrentRenderTargetViews, CurrentDepthStencilView);
	D3DCL->OMSetRenderTargets(NumCurrentRenderTargetViews, CpuDescriptors, FALSE, DsvCpuDescriptor);
}

void D3D12GraphicsCommandBuffer::ClearRenderTargets()
{
	BANE_CHECK(NumCurrentRenderTargetViews > 0);

	for (uint32 i = 0; i < NumCurrentRenderTargetViews; i++)
	{
		D3DCL->ClearRenderTargetView(CurrentRenderTargetViews[i]->GetCurrentFrame().CpuHandle, DirectX::Colors::Black, 0, nullptr);
	}

	if (CurrentDepthStencilView)
	{
		D3DCL->ClearDepthStencilView(CurrentDepthStencilView->GetCurrentFrame().CpuHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}
}

void D3D12GraphicsCommandBuffer::CloseCommandBuffer()
{
	if (NumCurrentRenderTargetViews > 0)
	{
		TransitionRenderTargetsToRead(CurrentRenderTargetViews, NumCurrentRenderTargetViews, CurrentDepthStencilView);
		for (uint32 i = 0; i < NumCurrentRenderTargetViews; i++)
		{
			CurrentRenderTargetViews[i] = nullptr;
		}
	}

	if (CommandList->bCanClose)
	{
		CommandList->Close();
	}
	NumDrawCalls = 0;
	NumCopies = 0;
	NumDispatches = 0;
	NumCurrentRenderTargetViews = 0;
	CurrentDepthStencilView = nullptr;
}

void D3D12GraphicsCommandBuffer::SetGraphicsPipelineState(const IGraphicsPipelineState* InPipelineState)
{
	D3D12GraphicsPipelineState* NewPipelineState = (D3D12GraphicsPipelineState*)InPipelineState;
	GraphicsPipelineState = NewPipelineState;
	D3DCL->SetPipelineState(GraphicsPipelineState->PipelineState);
	RootSignature = GraphicsPipelineState->ShaderSignature;
	D3DCL->SetGraphicsRootSignature(GraphicsPipelineState->ShaderSignature.RootSignature);
}

void D3D12GraphicsCommandBuffer::SetTexture(uint32 Slot, const ITextureBase* InTexture, uint32 Subresource)
{
	const D3D12TextureBase* Texture = (D3D12TextureBase*)InTexture;
	GraphicsResources.SetSRV(Texture, Slot, Subresource);
}

void D3D12GraphicsCommandBuffer::SetStructuredBuffer(uint32 Slot, const IBuffer* InBuffer, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
{
	const D3D12Buffer* Buffer = (D3D12Buffer*)InBuffer;
	GraphicsResources.SetSRV(Buffer, Slot, IndexToStart, NumElements, StructureByteStride);
}

void D3D12GraphicsCommandBuffer::SetUnorderedAccessView(uint32 Slot, const ITextureBase* InResource, uint32 Subresource)
{
	const D3D12TextureBase* Texture = (D3D12TextureBase*)InResource;
	GraphicsResources.SetUAV(Texture, Subresource, Slot);
}

void D3D12GraphicsCommandBuffer::SetUnorderedAccessView(uint32 Slot, const IBuffer* InResource, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
{
	const D3D12Buffer* Buffer = (D3D12Buffer*)InResource;
	GraphicsResources.SetUAV(Buffer, Slot, IndexToStart, NumElements, StructureByteStride);
}

void D3D12GraphicsCommandBuffer::SetConstantBuffer(uint32 Slot, const IBuffer* InBuffer, uint64 Offset)
{
	const D3D12Buffer* Buffer = (D3D12Buffer*)InBuffer;
	GraphicsResources.SetCBV(Buffer, Offset, Slot);
}

void D3D12GraphicsCommandBuffer::SetVertexBuffer(const IBuffer* InVertexBuffer, uint64 Offset)
{
	D3D12Buffer* Buffer = (D3D12Buffer*)InVertexBuffer;
	Buffer->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	Buffer->PromotedState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	D3D12_VERTEX_BUFFER_VIEW VbView = Buffer->GetVBView(GraphicsPipelineState->Desc.InputLayout);
	VbView.BufferLocation += Offset;
	VbView.SizeInBytes -= static_cast<uint32>(Offset);
	BANE_CHECK(Offset < Buffer->SizeInBytes - 1);
	FlushResourceTransitions();
	D3DCL->IASetVertexBuffers(0, 1, &VbView);
}

void D3D12GraphicsCommandBuffer::SetVertexBuffer(const IBuffer* InVertexBuffer)
{
	D3D12Buffer* Buffer = (D3D12Buffer*)InVertexBuffer;
	Buffer->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	Buffer->PromotedState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	D3D12_VERTEX_BUFFER_VIEW VbView = Buffer->GetVBView(GraphicsPipelineState->Desc.InputLayout);
	FlushResourceTransitions();
	D3DCL->IASetVertexBuffers(0, 1, &VbView);
}

void D3D12GraphicsCommandBuffer::SetIndexBuffer(const IBuffer* InIndexBuffer)
{
	D3D12Buffer* Buffer = (D3D12Buffer*)InIndexBuffer;
	Buffer->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	Buffer->PromotedState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
	D3D12_INDEX_BUFFER_VIEW IbView = Buffer->GetIBView();
	FlushResourceTransitions();
	D3DCL->IASetIndexBuffer(&IbView);
}

void D3D12GraphicsCommandBuffer::SetIndexBuffer(const IBuffer* InIndexBuffer, uint64 Offset)
{
	D3D12Buffer* Buffer = (D3D12Buffer*)InIndexBuffer;
	Buffer->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	Buffer->PromotedState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
	D3D12_INDEX_BUFFER_VIEW IbView = Buffer->GetIBView();
	IbView.BufferLocation += Offset;
	IbView.SizeInBytes -= static_cast<uint32>(Offset);
	FlushResourceTransitions();
	D3DCL->IASetIndexBuffer(&IbView);
}

void D3D12GraphicsCommandBuffer::SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY InTopology)
{
	D3DCL->IASetPrimitiveTopology(D3D12_TranslatePrimitiveTopology(InTopology));
}

// void D3D12GraphicsCommandBuffer::SetGraphicsResourceTable(const IShaderResourceTable* InTable)
// {
// 	D3D12ShaderResourceTable* Table = (D3D12ShaderResourceTable*)InTable;
// 	CurrentTable = Table;
// 
// 	bHasCheckedCurrentTable = false;
// 	if (Table->AssociatedSignature.RootSignature != RootSignature)
// 	{
// 		D3DCL->SetGraphicsRootSignature(Table->AssociatedSignature.RootSignature);
// 	}
// 
// 	for (uint32 i = 0; i < Table->ConstantBuffers.size(); i++)
// 	{
// 		if (Table->ConstantBuffers[i].ConstantBuffer != nullptr)
// 		{
// 			const auto& ConstBuffOffsetPair = Table->ConstantBuffers[i];
// 			D3DCL->SetGraphicsRootConstantBufferView(i, ConstBuffOffsetPair.ConstantBuffer->GetGPUVirtualAddress() + ConstBuffOffsetPair.Offset);
// 		}
// 	}
// 
// 	if (Table->HasTextures())
// 	{
// 		D3DCL->SetGraphicsRootDescriptorTable(Table->GetSRVDescriptorTableIndex(), Table->BaseSRVAllocation.GpuHandle);
// 		D3DCL->SetGraphicsRootDescriptorTable(Table->GetSamplerTableIndex(), Table->BaseSMPAllocation.GpuHandle);
// 	}
// 	if (Table->HasUAVs())
// 	{
// 		D3DCL->SetGraphicsRootDescriptorTable(Table->GetUAVDescriptorTableIndex(), Table->BaseUAVAllocation.GpuHandle);
// 	}
// }

void D3D12GraphicsCommandBuffer::Draw(uint32 VertexCount, uint32 StartVertexLocation)
{
	FlushResourceTransitions();
	CommitGraphicsResourcesToExecution();
	NumDrawCalls++;
	D3DCL->DrawInstanced(VertexCount, 1, StartVertexLocation, 0);
}

void D3D12GraphicsCommandBuffer::DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation)
{
	FlushResourceTransitions();
	CommitGraphicsResourcesToExecution();
	NumDrawCalls++;
	D3DCL->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
}

void D3D12GraphicsCommandBuffer::DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation)
{
	FlushResourceTransitions();
	CommitGraphicsResourcesToExecution();
	NumDrawCalls++;
	D3DCL->DrawInstanced(VertexCount, InstanceCount, StartVertexLocation, 0);
}

void D3D12GraphicsCommandBuffer::DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount)
{
	FlushResourceTransitions();
	CommitGraphicsResourcesToExecution();
	NumDrawCalls++;
	D3DCL->DrawIndexedInstanced(IndexCount, InstanceCount, StartIndexLocation, BaseVertexLocation, 0);
}

void D3D12GraphicsCommandBuffer::CopyBufferLocations(IBuffer* Src, uint64 SrcOffset, IBuffer* Dst, uint64 DstOffset, uint64 NumBytes)
{
	D3D12Buffer* SrcBuffer = (D3D12Buffer*)Src;
	D3D12Buffer* DstBuffer = (D3D12Buffer*)Dst;
	SrcBuffer->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	SrcBuffer->PromotedState = D3D12_RESOURCE_STATE_COPY_SOURCE;
	DstBuffer->TransitionResource(this, D3D12_RESOURCE_STATE_COMMON);
	DstBuffer->PromotedState = D3D12_RESOURCE_STATE_COPY_DEST;
	FlushResourceTransitions();
	NumCopies++;
	D3DCL->CopyBufferRegion(DstBuffer->Resource.D3DResource, DstOffset, SrcBuffer->Resource.D3DResource, SrcOffset, NumBytes);
}

void D3D12GraphicsCommandBuffer::CopyBuffers(IBuffer* Src, IBuffer* Dst)
{
	D3D12Buffer* SrcBuffer = (D3D12Buffer*)Src;
	D3D12Buffer* DstBuffer = (D3D12Buffer*)Dst;
	SrcBuffer->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	SrcBuffer->PromotedState = D3D12_RESOURCE_STATE_COPY_SOURCE;
	DstBuffer->TransitionResource(this, D3D12_RESOURCE_STATE_COMMON);
	DstBuffer->PromotedState = D3D12_RESOURCE_STATE_COPY_DEST;
	FlushResourceTransitions();
	NumCopies++;
	D3DCL->CopyResource(DstBuffer->Resource.D3DResource, SrcBuffer->Resource.D3DResource);
}

void D3D12GraphicsCommandBuffer::CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst)
{
	D3D12TextureBase* D3DTex = (D3D12TextureBase*)Dst;
	D3D12_TEXTURE_COPY_LOCATION TexCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(D3DTex->Resource.D3DResource, 0);

	D3D12Buffer* D3DBuf = (D3D12Buffer*)Src;
	D3D12_TEXTURE_COPY_LOCATION BufferSrcLocation = CD3DX12_TEXTURE_COPY_LOCATION(D3DBuf->Resource.D3DResource, 0);

	D3DTex->TransitionResource(this, D3D12_RESOURCE_STATE_COMMON);
	D3DTex->PromotedState = D3D12_RESOURCE_STATE_COPY_DEST;
	D3DBuf->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	D3DBuf->PromotedState = D3D12_RESOURCE_STATE_COPY_SOURCE;
	FlushResourceTransitions();
	NumCopies++;
	D3DCL->CopyTextureRegion(&TexCopyLocation, D3DTex->Width, D3DTex->Height, D3DTex->Depth, &BufferSrcLocation, nullptr);
}

void D3D12GraphicsCommandBuffer::CopyTextures(ITextureBase* InSrc, uint32 SrcSubresource, ITexture2D* InDst, uint32 DstSubresource)
{
	D3D12TextureBase* Src = (D3D12TextureBase*)InSrc;
	D3D12TextureBase* Dst = (D3D12TextureBase*)InDst;

	D3D12_TEXTURE_COPY_LOCATION SrcCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(Src->Resource.D3DResource, SrcSubresource);
	D3D12_TEXTURE_COPY_LOCATION DstCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(Dst->Resource.D3DResource, DstSubresource);

	Src->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	Src->PromotedState = D3D12_RESOURCE_STATE_COPY_SOURCE;
	Dst->TransitionResource(this, D3D12_RESOURCE_STATE_COMMON);
	Dst->PromotedState = D3D12_RESOURCE_STATE_COPY_DEST;
	FlushResourceTransitions();
	NumCopies++;
	D3DCL->CopyTextureRegion(&DstCopyLocation, 0, 0, 0, &SrcCopyLocation, nullptr);
}

void D3D12GraphicsCommandBuffer::CopyTextures(ITextureBase* InSrc, ivec3 SrcLocation, ITextureBase* InDst, ivec3 DstLocation, ivec3 DstSize)
{
	D3D12TextureBase* Src = (D3D12TextureBase*)InSrc;
	D3D12TextureBase* Dst = (D3D12TextureBase*)InDst;

	D3D12_TEXTURE_COPY_LOCATION SrcCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(Src->Resource.D3DResource, 0);
	D3D12_TEXTURE_COPY_LOCATION	DstCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(Dst->Resource.D3DResource, 0);

	Src->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	Src->PromotedState = D3D12_RESOURCE_STATE_COPY_SOURCE;
	Dst->TransitionResource(this, D3D12_RESOURCE_STATE_COMMON);
	Dst->PromotedState = D3D12_RESOURCE_STATE_COPY_DEST;
	FlushResourceTransitions();

	D3D12_BOX SrcBox = { };
	SrcBox.left = SrcLocation.x;
	SrcBox.top = SrcLocation.y;
	SrcBox.front = SrcLocation.z;
	SrcBox.right = SrcLocation.x + DstSize.x;
	SrcBox.bottom = SrcLocation.y + DstSize.y;
	SrcBox.back = SrcLocation.z + DstSize.z;
	NumCopies++;
	D3DCL->CopyTextureRegion(&DstCopyLocation, DstLocation.x, DstLocation.y, DstLocation.z, &SrcCopyLocation, &SrcBox);
}

void D3D12GraphicsCommandBuffer::SetComputePipelineState(const IComputePipelineState* InState)
{
	D3D12ComputePipelineState* Pipeline = (D3D12ComputePipelineState*)InState;
	ComputePipelineState = Pipeline;
	D3DCL->SetPipelineState(GraphicsPipelineState->PipelineState);
	RootSignature = Pipeline->ShaderSignature;
	D3DCL->SetComputeRootSignature(RootSignature.RootSignature);
}

void D3D12GraphicsCommandBuffer::SetComputeTexture(uint32 Slot, const ITextureBase* InTexture, uint32 Subresource)
{
	const D3D12TextureBase* Texture = (D3D12TextureBase*)InTexture;
	ComputeResources.SetSRV(Texture, Slot, Subresource);
}

void D3D12GraphicsCommandBuffer::SetComputeStructuredBuffer(uint32 Slot, const IBuffer* InBuffer, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
{
	const D3D12Buffer* Buffer = (D3D12Buffer*)InBuffer;
	ComputeResources.SetSRV(Buffer, Slot, IndexToStart, NumElements, StructureByteStride);
}

void D3D12GraphicsCommandBuffer::SetComputeUnorderedAccessView(uint32 Slot, const ITextureBase* InResource, uint32 Subresource)
{
	const D3D12TextureBase* Texture = (D3D12TextureBase*)InResource;
	ComputeResources.SetUAV(Texture, Subresource, Slot);
}

void D3D12GraphicsCommandBuffer::SetComputeUnorderedAccessView(uint32 Slot, const IBuffer* InResource, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
{
	const D3D12Buffer* Buffer = (D3D12Buffer*)InResource;
	ComputeResources.SetUAV(Buffer, Slot, IndexToStart, NumElements, StructureByteStride);
}

void D3D12GraphicsCommandBuffer::SetComputeConstantBuffer(uint32 Slot, const IBuffer* InBuffer, uint64 Offset)
{
	const D3D12Buffer* Buffer = (D3D12Buffer*)InBuffer;
	ComputeResources.SetCBV(Buffer, Offset, Slot);
}

ITexture2D* D3D12GraphicsCommandBuffer::CreateTemporaryTexture(uint32 Width, uint32 Height, EFORMAT Format, const SAMPLER_DESC& SampleDesc, ETEXTURE_USAGE Usage)
{
	return ParentDevice->CreateTemporaryRenderTexture(Width, Height, Format, SampleDesc, Usage);
}

IRenderTargetView* D3D12GraphicsCommandBuffer::CreateTemporaryRenderTargetView(ITexture2D* InTexture)
{
	return ParentDevice->CreateTemporaryRenderTargetView(reinterpret_cast<D3D12TextureBase*>(InTexture));
}

IDepthStencilView* D3D12GraphicsCommandBuffer::CreateTemporaryDepthStencilView(ITexture2D* InTexture)
{
	return ParentDevice->CreateTemporaryDepthStencilView(reinterpret_cast<D3D12TextureBase*>(InTexture));
}

void D3D12GraphicsCommandBuffer::DestroyTemporaryTexture(ITexture2D* pTexture)
{
	CommandList->TemporaryTextures.push_back({ reinterpret_cast<D3D12TextureBase*>(pTexture) });
}

void D3D12GraphicsCommandBuffer::DestroyTemporaryRenderTargetView(IRenderTargetView* pView)
{
	delete pView;
}

void D3D12GraphicsCommandBuffer::DestroyTemporaryDepthStencilView(IDepthStencilView* pView)
{
	delete pView;
}

// void D3D12GraphicsCommandBuffer::SetComputeResourceTable(const IShaderResourceTable* InTable)
// {
// 	D3D12ShaderResourceTable* Table = (D3D12ShaderResourceTable*)InTable;
// 	CurrentTable = Table;
// 
// 	bHasCheckedCurrentTable = false;
// 
// 	if (Table->AssociatedSignature.RootSignature != RootSignature)
// 	{
// 		D3DCL->SetComputeRootSignature(Table->AssociatedSignature.RootSignature);
// 	}
// 
// 	for (uint32 i = 0; i < Table->ConstantBuffers.size(); i++)
// 	{
// 		if (Table->ConstantBuffers[i].ConstantBuffer != nullptr)
// 		{
// 			const auto& ConstBuffOffsetPair = Table->ConstantBuffers[i];
// 			D3DCL->SetComputeRootConstantBufferView(i, ConstBuffOffsetPair.ConstantBuffer->GetGPUVirtualAddress() + ConstBuffOffsetPair.Offset);
// 		}
// 	}
// 
// 	// Note: Remove this slow path right here
// //	for (uint32 i = 0; i < Table->GetNumTextures(); i++)
// //	{
// //		auto BaseSMPAlloc = Table->BaseSMPAllocation;
// //		auto* Resource = Table->ShaderResources[i];
// //		if (Resource && Resource->GetResourceType() == D3D12_RESOURCE_TYPE_TEXTURE)
// //		{
// //			auto* Texture = dynamic_cast<D3D12TextureBase*>(Resource);
// //			if (Texture->bSamplerStateDirty)
// //			{
// //				BANE_CHECK(Texture != nullptr);
// //				BANE_CHECK(Texture->D3DSampleDesc.MaxAnisotropy != 1065353216);
// //				ParentDevice->GetDevice()->CreateSampler(&Texture->D3DSampleDesc, BaseSMPAlloc.OffsetFromStart(i).CpuHandle);
// //				Texture->bSamplerStateDirty = false;
// //			}
// //		}
// //	}
// 
// 	if (Table->HasTextures())
// 	{
// 		D3DCL->SetComputeRootDescriptorTable(Table->GetSRVDescriptorTableIndex(), Table->BaseSRVAllocation.GpuHandle);
// 		D3DCL->SetComputeRootDescriptorTable(Table->GetSamplerTableIndex(), Table->BaseSMPAllocation.GpuHandle);
// 	}
// 	if (Table->HasUAVs())
// 	{
// 		D3DCL->SetComputeRootDescriptorTable(Table->GetUAVDescriptorTableIndex(), Table->BaseUAVAllocation.GpuHandle);
// 	}
// }

void D3D12GraphicsCommandBuffer::Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ)
{
	FlushResourceTransitions();
	CommitComputeResourcesToExecution();
	NumDispatches++;
	D3DCL->Dispatch(ThreadX, ThreadY, ThreadZ);
}

void D3D12GraphicsCommandBuffer::InitializeAllocators(D3D12LinearDescriptorAllocator& ParentSrvAllocator, D3D12LinearDescriptorAllocator& ParentSmpAllocator)
{
	D3D12DescriptorAllocation SrvAllocation = ParentSrvAllocator.AllocateMultiple(512);
	D3D12DescriptorAllocation SmpAllocation = ParentSmpAllocator.AllocateMultiple(512);
	SrvDescriptorAllocator.InitializeSubAllocator(
		SrvAllocation.BaseCpuHandle,
		ParentSrvAllocator.IncrementSize,
		{ SrvAllocation.BaseCpuHandle.ptr + (static_cast<SIZE_T>(ParentSrvAllocator.IncrementSize) * 512) },
		SrvAllocation.GpuHandle
	);
	SmpDescriptorAllocator.InitializeSubAllocator(
		SmpAllocation.BaseCpuHandle,
		ParentSmpAllocator.IncrementSize,
		{ SmpAllocation.BaseCpuHandle.ptr + (static_cast<SIZE_T>(ParentSmpAllocator.IncrementSize) * 512) },
		SmpAllocation.GpuHandle
	);
}

D3D12_RESOURCE_BARRIER TranslateResourceTransition(const D3D12_RESOURCE_TRANSITION& ResourceTransition)
{
	D3D12_RESOURCE_BARRIER Result = CD3DX12_RESOURCE_BARRIER::Transition
	(
		ResourceTransition.Resource->Resource.D3DResource,
		ResourceTransition.PrevState,
		ResourceTransition.AfterState
	);
	return Result;
}

void D3D12GraphicsCommandBuffer::FlushResourceTransitions()
{
	if (!PendingTransitions.empty())
	{
		D3DCL->ResourceBarrier(static_cast<uint32>(PendingTransitions.size()), PendingTransitions.data());
		PendingTransitions.clear();
		PendingTransitions.reserve(32);
	}
	if (!TransitionedResources.empty())
	{
		for (uint32 i = 0; i < TransitionedResources.size(); i++)
		{
			D3D12GPUResource* Res = TransitionedResources[i];
			Res->CurrentState = Res->PendingState;
		}
		TransitionedResources.clear();
		TransitionedResources.reserve(32);
	}
}

void D3D12GraphicsCommandBuffer::CommitGraphicsResourcesToExecution()
{
	// Allow state inheritance	
	if (GraphicsResources.bAnySrvDirty || GraphicsResources.bAnyUavDirty)
	{
		GraphicsResources.Initialize(GraphicsPipelineState, ParentDevice, SrvDescriptorAllocator, SmpDescriptorAllocator);
	}
	GraphicsResources.ApplyGraphicsResources(this);
}

void D3D12GraphicsCommandBuffer::CommitComputeResourcesToExecution()
{
	if (ComputeResources.bAnySrvDirty || ComputeResources.bAnyUavDirty)
	{
		ComputeResources.Initialize(ComputePipelineState, ParentDevice, SrvDescriptorAllocator, SmpDescriptorAllocator);
	}
	ComputeResources.ApplyComputeResources(this);
}

void D3D12GraphicsCommandBuffer::CommitResources()
{
}

void D3D12GraphicsCommandBuffer::FetchNewCommandList()
{
	CommandList = ParentDevice->GetCommandList(ContextType);
	D3DCL = CommandList->GetGraphicsCommandList();
}

void D3D12GraphicsCommandBuffer::ReturnCommandList()
{
	CommandList = nullptr;
	D3DCL = nullptr;
}

void D3D12GraphicsCommandBuffer::TransitionRenderTargetsToRead(D3D12RenderTargetView** ppViews, uint32 NumRenderTargets, D3D12DepthStencilView* pDepthStencilView)
{
	for (uint32 i = 0; i < NumRenderTargets; i++)
	{
		D3D12RenderTargetView* View = ppViews[i];
		D3D12TextureBase* Tex = View->GetCurrentFrame().Texture;
		Tex->TransitionResource(this, D3D12_RESOURCE_STATE_PRESENT);
		Tex->PromotedState = D3D12_RESOURCE_STATE_PRESENT;
	}
	if (pDepthStencilView)
	{
		D3D12TextureBase* Tex = pDepthStencilView->GetCurrentFrame().Texture;
		Tex->TransitionResource(this, D3D12_RESOURCE_STATE_DEPTH_READ);
		Tex->PromotedState = D3D12_RESOURCE_STATE_DEPTH_READ;
	}
	FlushResourceTransitions();
}

void D3D12GraphicsCommandBuffer::TransitionRenderTargetsToWrite(D3D12RenderTargetView** ppViews, uint32 NumRenderTargets, D3D12DepthStencilView* pDepthStencilView)
{
	for (uint32 i = 0; i < NumRenderTargets; i++)
	{
		D3D12RenderTargetView* View = ppViews[i];
		D3D12TextureBase* Tex = View->GetCurrentFrame().Texture;
		Tex->TransitionResource(this, D3D12_RESOURCE_STATE_RENDER_TARGET);
		Tex->PromotedState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}
	if (pDepthStencilView)
	{
		D3D12TextureBase* Tex = pDepthStencilView->GetCurrentFrame().Texture;
		Tex->TransitionResource(this, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		Tex->PromotedState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}
	FlushResourceTransitions();
}

///--------- End D3D12GraphicsCommandBuffer ---------///

// void D3D12GraphicsCommandContext::BeginPass(IRenderPass* InRenderPass)
// {
// 	if (InRenderPass)
// 	{
// 		CurrentCommandBuffer->InitializeAllocators(ParentDevice->GetSrvAllocator(), ParentDevice->GetSmpAllocator());
// 	}
// 	CurrentCommandBuffer->BeginPass(InRenderPass);
// }
// 
// void D3D12GraphicsCommandContext::EndPass()
// {
// 	ParentDevice->EnsureAllUploadsOccured();
// 	CurrentCommandBuffer->EndPass();
// 	CurrentCommandBuffer->CloseCommandBuffer();
// 	ParentDevice->GetCommandQueue(ContextType).ExecuteImmediate(CurrentCommandBuffer->CommandList);
// 	CurrentCommandBuffer->FetchNewCommandList();
// }

void D3D12GraphicsCommandContext::Begin()
{
	CurrentCommandBuffer->Begin();
	if (ContextType == COMMAND_CONTEXT_TYPE_GRAPHICS)
	{
		CurrentCommandBuffer->InitializeAllocators(ParentDevice->GetSrvAllocator(), ParentDevice->GetSmpAllocator());
	}
}

void D3D12GraphicsCommandContext::SetRenderTarget(IRenderTargetView* pRenderTarget, IDepthStencilView* pDepthStencil)
{
	CurrentCommandBuffer->SetRenderTarget(pRenderTarget, pDepthStencil);
}

void D3D12GraphicsCommandContext::SetRenderTargets(const std::vector<IRenderTargetView*>& RenderTargetViews, IDepthStencilView* pDepthStencil)
{
	CurrentCommandBuffer->SetRenderTargets(RenderTargetViews, pDepthStencil);
}

void D3D12GraphicsCommandContext::ClearRenderTargets()
{
	CurrentCommandBuffer->ClearRenderTargets();
}

void D3D12GraphicsCommandContext::Flush()
{
	ParentDevice->EnsureAllUploadsOccured();
	CurrentCommandBuffer->CloseCommandBuffer();
	ParentDevice->GetCommandQueue(ContextType).ExecuteImmediate(CurrentCommandBuffer->CommandList);
	CurrentCommandBuffer->FetchNewCommandList();
	ParentDevice->GetCommandQueue(ContextType).StallForFinish();
}

IGraphicsCommandBuffer* D3D12GraphicsCommandContext::GetUnderlyingCommandBuffer()
{
	return CurrentCommandBuffer;
}

IGraphicsCommandBuffer* D3D12GraphicsCommandContext::CreateCommandBuffer()
{
	D3D12GraphicsCommandBuffer* Result;
	if (CommandPool.GetNumElements() == 0)
	{
		// Error?
		// Needs to be handled when this is nullptr
		return nullptr;
	}
	Result = CommandPool.Pop();
	Result->InitializeAllocators(ParentDevice->GetSrvAllocator(), ParentDevice->GetSmpAllocator());
	Result->Begin();
	return Result;
}

void D3D12GraphicsCommandContext::ExecuteCommandBuffer(IGraphicsCommandBuffer* InCommandBuffer)
{
	D3D12GraphicsCommandBuffer* CmdBuff = (D3D12GraphicsCommandBuffer*)InCommandBuffer;
	//BANE_CHECK(CmdBuff->CommandList != nullptr);
	//BANE_CHECK(CmdBuff->CommandList->bCanReset);
	ParentDevice->EnsureAllUploadsOccured();
	ParentDevice->GetCommandQueue(ContextType).ExecuteImmediate(CmdBuff->CommandList);
	CmdBuff->ReturnCommandList();
	CommandPool.Push(CmdBuff);
}

void D3D12GraphicsCommandContext::ExecuteCommandBuffers(const std::vector<IGraphicsCommandBuffer*>& InCommandBuffers)
{
	auto& CmdQueue = ParentDevice->GetCommandQueue(ContextType);
	for (IGraphicsCommandBuffer* ICmdBuff : InCommandBuffers)
	{
		D3D12GraphicsCommandBuffer* CmdBuff = (D3D12GraphicsCommandBuffer*)ICmdBuff;
		BANE_CHECK(CmdBuff->CommandList != nullptr);
		BANE_CHECK(CmdBuff->CommandList->bCanReset);
		CmdQueue.ScheduleCommandList(CmdBuff->CommandList);
		CmdBuff->ReturnCommandList();
		CommandPool.Push(CmdBuff);
	}
	ParentDevice->EnsureAllUploadsOccured();
	CmdQueue.ExecuteCommandLists();
}

void D3D12GraphicsCommandContext::SetGraphicsPipelineState(const IGraphicsPipelineState* InPipelineState)
{
	CurrentCommandBuffer->SetGraphicsPipelineState(InPipelineState);
}

void D3D12GraphicsCommandContext::SetTexture(uint32 Slot, const ITextureBase* InTexture, uint32 Subresource)
{
	CurrentCommandBuffer->SetTexture(Slot, InTexture, Subresource);
}

void D3D12GraphicsCommandContext::SetStructuredBuffer(uint32 Slot, const IBuffer* InBuffer, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
{
	CurrentCommandBuffer->SetStructuredBuffer(Slot, InBuffer, IndexToStart, NumElements, StructureByteStride);
}

void D3D12GraphicsCommandContext::SetUnorderedAccessView(uint32 Slot, const ITextureBase* InResource, uint32 Subresource)
{
	CurrentCommandBuffer->SetUnorderedAccessView(Slot, InResource, Subresource);
}

void D3D12GraphicsCommandContext::SetUnorderedAccessView(uint32 Slot, const IBuffer* InResource, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
{
	CurrentCommandBuffer->SetUnorderedAccessView(Slot, InResource, IndexToStart, NumElements, StructureByteStride);
}

void D3D12GraphicsCommandContext::SetConstantBuffer(uint32 Slot, const IBuffer* InBuffer, uint64 Offset)
{
	CurrentCommandBuffer->SetConstantBuffer(Slot, InBuffer, Offset);
}

void D3D12GraphicsCommandContext::SetVertexBuffer(const IBuffer* InVertexBuffer)
{
	CurrentCommandBuffer->SetVertexBuffer(InVertexBuffer);
}

void D3D12GraphicsCommandContext::SetVertexBuffer(const IBuffer* InBuffer, uint64 Offset)
{
	CurrentCommandBuffer->SetVertexBuffer(InBuffer, Offset);
}

void D3D12GraphicsCommandContext::SetIndexBuffer(const IBuffer* InIndexBuffer)
{
	CurrentCommandBuffer->SetIndexBuffer(InIndexBuffer);
}

void D3D12GraphicsCommandContext::SetIndexBuffer(const IBuffer* InIndexBuffer, uint64 Offset)
{
	CurrentCommandBuffer->SetIndexBuffer(InIndexBuffer, Offset);
}

void D3D12GraphicsCommandContext::SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY InTopology)
{
	CurrentCommandBuffer->SetPrimitiveTopology(InTopology);
}

void D3D12GraphicsCommandContext::Draw(uint32 VertexCount, uint32 StartVertexLocation)
{
	CurrentCommandBuffer->Draw(VertexCount, StartVertexLocation);
}

void D3D12GraphicsCommandContext::DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation)									
{
	CurrentCommandBuffer->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void D3D12GraphicsCommandContext::DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation)								
{
	CurrentCommandBuffer->DrawInstanced(VertexCount, InstanceCount, StartVertexLocation);
}

void D3D12GraphicsCommandContext::DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount)	
{
	CurrentCommandBuffer->DrawIndexedInstanced(IndexCount, StartIndexLocation, BaseVertexLocation, InstanceCount);
}

void D3D12GraphicsCommandContext::CopyBufferLocations(IBuffer* Src, uint64 SrcOffset, IBuffer* Dst, uint64 DstOffset, uint64 NumBytes)				
{
	CurrentCommandBuffer->CopyBufferLocations(Src, SrcOffset, Dst, DstOffset, NumBytes);
}

void D3D12GraphicsCommandContext::CopyBuffers(IBuffer* Src, IBuffer* Dst)															
{
	CurrentCommandBuffer->CopyBuffers(Src, Dst);
}

void D3D12GraphicsCommandContext::CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst)																
{
	CurrentCommandBuffer->CopyBufferToTexture(Src, Dst);
}

void D3D12GraphicsCommandContext::CopyTextures(ITextureBase* InSrc, uint32 SrcSubresource, ITexture2D* InDst, uint32 DstSubresource)				
{
	CurrentCommandBuffer->CopyTextures(InSrc, SrcSubresource, InDst, DstSubresource);
}

void D3D12GraphicsCommandContext::CopyTextures(ITextureBase* Src, ivec3 SrcLocation, ITextureBase* Dst, ivec3 DstLocation, ivec3 DstSize)				
{
	CurrentCommandBuffer->CopyTextures(Src, SrcLocation, Dst, DstLocation, DstSize);
}

void D3D12GraphicsCommandContext::SetComputePipelineState(const IComputePipelineState* InState)														
{
	CurrentCommandBuffer->SetComputePipelineState(InState);
}

void D3D12GraphicsCommandContext::SetComputeTexture(uint32 Slot, const ITextureBase* InTexture, uint32 Subresource)
{
	CurrentCommandBuffer->SetComputeTexture(Slot, InTexture, Subresource);
}

void D3D12GraphicsCommandContext::SetComputeStructuredBuffer(uint32 Slot, const IBuffer* InBuffer, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
{
	CurrentCommandBuffer->SetComputeStructuredBuffer(Slot, InBuffer, IndexToStart, NumElements, StructureByteStride);
}

void D3D12GraphicsCommandContext::SetComputeUnorderedAccessView(uint32 Slot, const ITextureBase* InResource, uint32 Subresource)
{
	CurrentCommandBuffer->SetComputeUnorderedAccessView(Slot, InResource, Subresource);
}

void D3D12GraphicsCommandContext::SetComputeUnorderedAccessView(uint32 Slot, const IBuffer* InResource, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
{
	CurrentCommandBuffer->SetComputeUnorderedAccessView(Slot, InResource, IndexToStart, NumElements, StructureByteStride);
}

void D3D12GraphicsCommandContext::SetComputeConstantBuffer(uint32 Slot, const IBuffer* InBuffer, uint64 Offset)
{
	CurrentCommandBuffer->SetComputeConstantBuffer(Slot, InBuffer, Offset);
}

ITexture2D* D3D12GraphicsCommandContext::CreateTemporaryTexture(uint32 Width, uint32 Height, EFORMAT Format, const SAMPLER_DESC& SampleDesc, ETEXTURE_USAGE Usage)
{
	return CurrentCommandBuffer->CreateTemporaryTexture(Width, Height, Format, SampleDesc, Usage);
}

IRenderTargetView* D3D12GraphicsCommandContext::CreateTemporaryRenderTargetView(ITexture2D* InTexture)
{
	return CurrentCommandBuffer->CreateTemporaryRenderTargetView(InTexture);
}

IDepthStencilView* D3D12GraphicsCommandContext::CreateTemporaryDepthStencilView(ITexture2D* InTexture)
{
	return CurrentCommandBuffer->CreateTemporaryDepthStencilView(InTexture);
}

void D3D12GraphicsCommandContext::DestroyTemporaryTexture(ITexture2D* pTexture)
{
	CurrentCommandBuffer->DestroyTemporaryTexture(pTexture);
}

void D3D12GraphicsCommandContext::DestroyTemporaryRenderTargetView(IRenderTargetView* pView)
{
	CurrentCommandBuffer->DestroyTemporaryRenderTargetView(pView);
}

void D3D12GraphicsCommandContext::DestroyTemporaryDepthStencilView(IDepthStencilView* pView)
{
	CurrentCommandBuffer->DestroyTemporaryDepthStencilView(pView);
}

void D3D12GraphicsCommandContext::Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ) 
{
	CurrentCommandBuffer->Dispatch(ThreadX, ThreadY, ThreadZ);
}



void D3D12ComputeCommandContext::Begin()
{
	CommandList = ParentDevice->GetCommandList(COMMAND_CONTEXT_TYPE_COMPUTE);
	D3DCL = CommandList->GetGraphicsCommandList(); 
	ID3D12DescriptorHeap* ppHeaps[] = { ParentDevice->m_SrvAllocator.DescriptorHeap, ParentDevice->m_SmpAllocator.DescriptorHeap };
	D3DCL->SetDescriptorHeaps(2, ppHeaps);
}

void D3D12ComputeCommandContext::End()
{
	PipelineState = nullptr;
	RootSignature = nullptr;
	CommandList->Close();
	ParentDevice->GetComputeQueue().ExecuteImmediate(CommandList); // Execute immediate, hopefully multiple passes per frame will hide the scheduler latency
	CommandList = nullptr;
}

void D3D12ComputeCommandContext::StallToEnd()
{
	ParentDevice->GetCommandQueue(COMMAND_CONTEXT_TYPE_COMPUTE).StallForFinish();
}

void D3D12ComputeCommandContext::SetComputePipelineState(const IComputePipelineState* InState)
{
	D3D12ComputePipelineState* Pipeline = (D3D12ComputePipelineState*)InState;
	PipelineState = Pipeline;
	D3DCL->SetPipelineState(PipelineState->PipelineState);
	RootSignature = Pipeline->ShaderSignature.RootSignature;
	D3DCL->SetComputeRootSignature(RootSignature);
}

//void D3D12ComputeCommandContext::SetComputeResourceTable(const IShaderResourceTable* InTable)
//{
//	D3D12ShaderResourceTable* Table = (D3D12ShaderResourceTable*)InTable;
//	CurrentTable = Table;
//
//	bHasCheckedCurrentTable = false;
//
//	if (Table->AssociatedSignature.RootSignature != RootSignature)
//	{
//		D3DCL->SetComputeRootSignature(Table->AssociatedSignature.RootSignature);
//	}
//
//	for (uint32 i = 0; i < Table->ConstantBuffers.size(); i++)
//	{
//		if (Table->ConstantBuffers[i].ConstantBuffer != nullptr)
//		{
//			const auto& ConstBuffOffsetPair = Table->ConstantBuffers[i];
//			D3DCL->SetComputeRootConstantBufferView(i, ConstBuffOffsetPair.ConstantBuffer->GetGPUVirtualAddress() + ConstBuffOffsetPair.Offset);
//		}
//	}
//
//	if (Table->HasTextures())
//	{
//		D3DCL->SetComputeRootDescriptorTable(Table->GetSRVDescriptorTableIndex(), Table->BaseSRVAllocation.GpuHandle);
//		D3DCL->SetComputeRootDescriptorTable(Table->GetSamplerTableIndex(), Table->BaseSMPAllocation.GpuHandle);
//	}
//	if (Table->HasUAVs())
//	{
//		D3DCL->SetComputeRootDescriptorTable(Table->GetUAVDescriptorTableIndex(), Table->BaseUAVAllocation.GpuHandle);
//	}
//}

void D3D12ComputeCommandContext::Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ)
{
	FlushResourceTransitions();
	CommitResources();
	D3DCL->Dispatch(ThreadX, ThreadY, ThreadZ);
}

void D3D12ComputeCommandContext::FlushResourceTransitions()
{
	if (!PendingTransitions.empty())
	{
		D3DCL->ResourceBarrier(static_cast<uint32>(PendingTransitions.size()), PendingTransitions.data());
		PendingTransitions.clear();
		PendingTransitions.reserve(32);
	}
	if (!TransitionedResources.empty())
	{
		for (uint32 i = 0; i < TransitionedResources.size(); i++)
		{
			D3D12GPUResource* Res = TransitionedResources[i];
			Res->CurrentState = Res->PendingState;
		}
		TransitionedResources.clear();
		TransitionedResources.reserve(32);
	}
}

void D3D12ComputeCommandContext::CommitResources()
{
// 	if (bHasCheckedCurrentTable)
// 	{
// 		return;
// 	}
// 
// 	bHasCheckedCurrentTable = true;
// 	// Leave out constant buffers because they are constant read?
// 	if (!CurrentTable->ShaderResources.empty())
// 	{
// 		for (uint32 i = 0; i < CurrentTable->ShaderResources.size(); i++)
// 		{
// 			D3D12GPUResource* pResource = CurrentTable->ShaderResources[i];
// 			if (pResource && pResource->RegisterDependency(COMMAND_CONTEXT_TYPE_COMPUTE))
// 			{
// 				CommandList->bNeedsWaitForGraphicsQueue = true;
// 			}
// 		}
// 	}
// 	if (!CurrentTable->UnorderedAccessResources.empty())
// 	{
// 		for (uint32 i = 0; i < CurrentTable->UnorderedAccessResources.size(); i++)
// 		{
// 			D3D12GPUResource* pResource = CurrentTable->UnorderedAccessResources[i];
// 			if (pResource && pResource->RegisterDependency(COMMAND_CONTEXT_TYPE_COMPUTE))
// 			{
// 				CommandList->bNeedsWaitForGraphicsQueue = true;
// 			}
// 		}
// 	}
}
