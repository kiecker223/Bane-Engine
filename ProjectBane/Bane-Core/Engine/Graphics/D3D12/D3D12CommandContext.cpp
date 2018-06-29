#include "D3D12CommandContext.h"
#include "D3D12GraphicsDevice.h"
#include "D3D12GraphicsResource.h"
#include "D3D12Translator.h"
#include "D3D12ShaderResourceViews.h"
#include <DirectXColors.h>


void D3D12CommandList::FlushDestructionQueue()
{
	if (UploadResourcesToDestroy.empty())
	{
		return;
	}
	for (int i = 0; i < UploadResourcesToDestroy.size(); i++)
	{
		auto& Resource = UploadResourcesToDestroy[i];
		Resource.Destroy();
	}

	UploadResourcesToDestroy.clear();
}

void D3D12GraphicsCommandContext::BeginPass(IRenderPassInfo* InRenderPass)
{
	CommandList = ParentDevice->GetCommandList(ContextType);
	D3DCL = CommandList->GetGraphicsCommandList();
	if (InRenderPass)
	{
		D3D12RenderPassInfo* RenderPass = (D3D12RenderPassInfo*)InRenderPass;
		RenderPass->TransitionResourcesToWrite(this);
		FlushResourceTransitions();
		RenderPass->SetRenderTargets(D3DCL);
		RenderPass->Clear(D3DCL);
		ID3D12DescriptorHeap* ppHeaps[] = { ParentDevice->m_SrvAllocator.GetDescriptorHeap(), ParentDevice->m_SmpAllocator.GetDescriptorHeap() };
		D3DCL->SetDescriptorHeaps(2, ppHeaps);
		D3DCL->RSSetScissorRects(1, &ScissorRect);
		D3DCL->RSSetViewports(1, &Viewport);
		CurrentRenderPass = RenderPass;
	}
}

void D3D12GraphicsCommandContext::EndPass()
{
	if (CurrentRenderPass)
	{
		CurrentRenderPass->TransitionResourcesToRead(this);
		FlushResourceTransitions();
	}
	PipelineState = nullptr;
	RootSignature = nullptr;
	CommandList->Close();
	ParentDevice->GetCommandQueue(ContextType).ExecuteImmediate(CommandList); // Execute immediate, hopefully multiple passes per frame will hide the scheduler latency
	CommandList = nullptr;
}

void D3D12GraphicsCommandContext::Flush()
{
	ParentDevice->GetCommandQueue(ContextType).StallForFinish();
}

void D3D12GraphicsCommandContext::SetGraphicsPipelineState(const IGraphicsPipelineState* InPipelineState)
{
	D3D12GraphicsPipelineState* NewPipelineState = (D3D12GraphicsPipelineState*)InPipelineState;
	PipelineState = NewPipelineState;
	D3DCL->SetPipelineState(PipelineState->PipelineState);
	RootSignature = PipelineState->ShaderSignature.RootSignature;
	D3DCL->SetGraphicsRootSignature(PipelineState->ShaderSignature.RootSignature);
}

void D3D12GraphicsCommandContext::SetVertexBuffer(const IBuffer* InVertexBuffer)
{
	D3D12Buffer* Buffer = (D3D12Buffer*)InVertexBuffer;
	Buffer->TransitionResource(this, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	D3DCL->IASetVertexBuffers(0, 1, &Buffer->GetVBView(PipelineState->Desc.InputLayout));
}

void D3D12GraphicsCommandContext::SetIndexBuffer(const IBuffer* InIndexBuffer)
{
	D3D12Buffer* Buffer = (D3D12Buffer*)InIndexBuffer;
	Buffer->TransitionResource(this, D3D12_RESOURCE_STATE_INDEX_BUFFER);
	D3DCL->IASetIndexBuffer(&Buffer->GetIBView());
}

void D3D12GraphicsCommandContext::SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY InTopology)
{
	D3DCL->IASetPrimitiveTopology(D3D12_TranslatePrimitiveTopology(InTopology));
}

void D3D12GraphicsCommandContext::SetGraphicsResourceTable(const IShaderResourceTable* InTable)
{
	D3D12ShaderResourceTable* Table = (D3D12ShaderResourceTable*)InTable;
	CurrentTable = Table;

	bHasCheckedCurrentTable = false;
	if (Table->AssociatedSignature.RootSignature != RootSignature)
	{
		D3DCL->SetGraphicsRootSignature(Table->AssociatedSignature.RootSignature);
	}

	for (uint i = 0; i < Table->ConstantBuffers.size(); i++)
	{
		if (Table->ConstantBuffers[i] != nullptr)
		{
			D3DCL->SetGraphicsRootConstantBufferView(i, Table->ConstantBuffers[i]->GetGPUVirtualAddress());
		}
	}

	if (Table->HasTextures())
	{
		D3DCL->SetGraphicsRootDescriptorTable(Table->GetSRVDescriptorTableIndex(), Table->BaseSRVAllocation.GpuHandle);
		D3DCL->SetGraphicsRootDescriptorTable(Table->GetSamplerTableIndex(), Table->BaseSMPAllocation.GpuHandle);
	}
	if (Table->HasUAVs())
	{
		D3DCL->SetGraphicsRootDescriptorTable(Table->GetUAVDescriptorTableIndex(), Table->BaseUAVAllocation.GpuHandle);
	}
}


void* D3D12GraphicsCommandContext::Map(IGPUResource* InResource, uint Subresource)
{
	D3D12GPUResource* Resource = dynamic_cast<D3D12GPUResource*>(InResource);
	void* MappedPointer = nullptr;
	if (Resource->MappedPointer != nullptr)
	{
		return Resource->MappedPointer;
	}
	if (FAILED(Resource->Resource.D3DResource->Map(Subresource, nullptr, &MappedPointer)))
	{
		__debugbreak();
	}
	return MappedPointer;
}

void D3D12GraphicsCommandContext::Unmap(IGPUResource* InResource, uint Subresource)
{
	D3D12GPUResource* Resource = dynamic_cast<D3D12GPUResource*>(InResource);
	if (Resource->MappedPointer == nullptr)
	{
		Resource->Resource.D3DResource->Unmap(Subresource, nullptr);
	}
	else
	{
		return;
	}
}

void D3D12GraphicsCommandContext::Draw(uint VertexCount, uint StartVertexLocation)
{
	FlushResourceTransitions();
	CommitResources();
	D3DCL->DrawInstanced(VertexCount, 1, StartVertexLocation, 0);
}

void D3D12GraphicsCommandContext::DrawIndexed(uint IndexCount, uint StartIndexLocation, int BaseVertexLocation)
{
	FlushResourceTransitions();
	CommitResources();
	D3DCL->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
}

void D3D12GraphicsCommandContext::DrawInstanced(uint VertexCount, uint InstanceCount, uint StartVertexLocation)
{
	FlushResourceTransitions();
	CommitResources();
	D3DCL->DrawInstanced(VertexCount, InstanceCount, StartVertexLocation, 0);
}

void D3D12GraphicsCommandContext::DrawIndexedInstanced(uint IndexCount, uint StartIndexLocation, int BaseVertexLocation, uint InstanceCount)
{
	FlushResourceTransitions();
	CommitResources();
	D3DCL->DrawIndexedInstanced(IndexCount, InstanceCount, StartIndexLocation, BaseVertexLocation, 0);
}

void D3D12GraphicsCommandContext::CopyBuffers(IBuffer* Src, IBuffer* Dst)
{
	D3D12Buffer* SrcBuffer = (D3D12Buffer*)Src;
	D3D12Buffer* DstBuffer = (D3D12Buffer*)Dst;
	SrcBuffer->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	DstBuffer->TransitionResource(this, D3D12_RESOURCE_STATE_COMMON);
	D3DCL->CopyResource(DstBuffer->Resource.D3DResource, SrcBuffer->Resource.D3DResource);
}

void D3D12GraphicsCommandContext::CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst)
{
	D3D12TextureBase* D3DTex = (D3D12TextureBase*)Dst;
	D3D12_TEXTURE_COPY_LOCATION TexCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(D3DTex->Resource.D3DResource, 0);

	D3D12Buffer* D3DBuf = (D3D12Buffer*)Src;
	D3D12_TEXTURE_COPY_LOCATION BufferSrcLocation = CD3DX12_TEXTURE_COPY_LOCATION(D3DBuf->Resource.D3DResource, 0);

	D3DTex->TransitionResource(this, D3D12_RESOURCE_STATE_COMMON);
	D3DBuf->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	FlushResourceTransitions();
	D3DCL->CopyTextureRegion(&TexCopyLocation, D3DTex->Width, D3DTex->Height, D3DTex->Depth, &BufferSrcLocation, nullptr);
}

void D3D12GraphicsCommandContext::CopyTextures(ITextureBase* InSrc, uint SrcSubresource, ITexture2D* InDst, uint DstSubresource)
{
	D3D12TextureBase* Src = (D3D12TextureBase*)InSrc;
	D3D12TextureBase* Dst = (D3D12TextureBase*)InDst;

	D3D12_TEXTURE_COPY_LOCATION SrcCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(Src->Resource.D3DResource, SrcSubresource);
	D3D12_TEXTURE_COPY_LOCATION DstCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(Dst->Resource.D3DResource, DstSubresource);

	Src->TransitionResource(this, D3D12_RESOURCE_STATE_COPY_SOURCE);
	Dst->TransitionResource(this, D3D12_RESOURCE_STATE_COPY_DEST);
	FlushResourceTransitions();
	D3DCL->CopyTextureRegion(&DstCopyLocation, 0, 0, 0, &SrcCopyLocation, nullptr);
}

void D3D12GraphicsCommandContext::CopyTextures(ITextureBase* InSrc, int3 SrcLocation, ITextureBase* InDst, int3 DstLocation, int3 DstSize)
{
	D3D12TextureBase* Src = (D3D12TextureBase*)InSrc;
	D3D12TextureBase* Dst = (D3D12TextureBase*)InDst;

	D3D12_TEXTURE_COPY_LOCATION SrcCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(Src->Resource.D3DResource, 0);
	D3D12_TEXTURE_COPY_LOCATION	DstCopyLocation = CD3DX12_TEXTURE_COPY_LOCATION(Dst->Resource.D3DResource, 0);

	Src->TransitionResource(this, D3D12_RESOURCE_STATE_GENERIC_READ);
	Dst->TransitionResource(this, D3D12_RESOURCE_STATE_COMMON);
	FlushResourceTransitions();

	D3D12_BOX SrcBox = { };
	SrcBox.left = DstLocation.x;
	SrcBox.top = DstLocation.y;
	SrcBox.front = DstLocation.z;
	SrcBox.right = DstLocation.x + DstSize.x;
	SrcBox.bottom = DstLocation.y + DstSize.y;
	SrcBox.back = DstLocation.z + DstSize.z;
	D3DCL->CopyTextureRegion(&DstCopyLocation, DstLocation.x, DstLocation.y, DstLocation.z, &SrcCopyLocation, &SrcBox);
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

void D3D12GraphicsCommandContext::FlushResourceTransitions()
{
	ParentDevice->EnsureAllUploadsOccured();
	if (PendingTransitions.GetNumElements() > 0)
	{
		D3DCL->ResourceBarrier(PendingTransitions.GetNumElements(), PendingTransitions.Data.data());
		PendingTransitions.Reset();
	}
	while (TransitionedResources.GetNumElements() != 0)
	{
		D3D12GPUResource* Res = TransitionedResources.Pop();
		Res->CurrentState = Res->PendingState;
	}
}

void D3D12GraphicsCommandContext::CommitResources()
{
	if (bHasCheckedCurrentTable || CurrentTable == nullptr)
	{
		return;
	}

	bHasCheckedCurrentTable = true;
	if (CurrentRenderPass != nullptr && CurrentRenderPass->NumRenderTargets > 0)
	{
		for (uint i = 0; i < CurrentRenderPass->NumRenderTargets; i++)
		{
			D3D12TextureBase* Texture = CurrentRenderPass->RenderTargets[i]->GetCurrentFrame().Texture;
			if (Texture->RegisterDependency(COMMAND_CONTEXT_TYPE_GRAPHICS))
			{
				CommandList->bNeedsWaitForComputeQueue = true;
			}
		}
	}
	if (!CurrentTable->ShaderResources.empty())
	{
		for (uint i = 0; i < CurrentTable->ShaderResources.size(); i++)
		{
			D3D12GPUResource* pResource = CurrentTable->ShaderResources[i];
			if (pResource && pResource->RegisterDependency(COMMAND_CONTEXT_TYPE_GRAPHICS))
			{
				CommandList->bNeedsWaitForComputeQueue = true;
			}
		}
	}
	if (!CurrentTable->UnorderedAccessResources.empty())
	{
		for (uint i = 0; i < CurrentTable->UnorderedAccessResources.size(); i++)
		{
			D3D12GPUResource* pResource = CurrentTable->UnorderedAccessResources[i];
			if (pResource && pResource->RegisterDependency(COMMAND_CONTEXT_TYPE_GRAPHICS))
			{
				CommandList->bNeedsWaitForComputeQueue = true;
			}
		}
	}
}

void D3D12ComputeCommandContext::Begin()
{
	CommandList = ParentDevice->GetCommandList(COMMAND_CONTEXT_TYPE_COMPUTE);
	D3DCL = CommandList->GetGraphicsCommandList(); 
	ID3D12DescriptorHeap* ppHeaps[] = { ParentDevice->m_SrvAllocator.GetDescriptorHeap(), ParentDevice->m_SmpAllocator.GetDescriptorHeap() };
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

void* D3D12ComputeCommandContext::Map(IGPUResource* InResource, uint Subresource)
{
	D3D12GPUResource* Resource = dynamic_cast<D3D12GPUResource*>(InResource);
	void* MappedPointer = nullptr;
	if (Resource->MappedPointer != nullptr)
	{
		return Resource->MappedPointer;
	}
	if (FAILED(Resource->Resource.D3DResource->Map(Subresource, nullptr, &MappedPointer)))
	{
		__debugbreak();
	}
	return MappedPointer;
}

void D3D12ComputeCommandContext::Unmap(IGPUResource* InResource, uint Subresource)
{
	D3D12GPUResource* Resource = dynamic_cast<D3D12GPUResource*>(InResource);
	if (Resource->MappedPointer == nullptr)
	{
		Resource->Resource.D3DResource->Unmap(Subresource, nullptr);
	}
	else
	{
		return;
	}
}

void D3D12ComputeCommandContext::SetComputePipelineState(const IComputePipelineState* InState)
{
	D3D12ComputePipelineState* Pipeline = (D3D12ComputePipelineState*)InState;
	PipelineState = Pipeline;
	D3DCL->SetPipelineState(PipelineState->PipelineState);
	RootSignature = Pipeline->ShaderSignature.RootSignature;
	D3DCL->SetComputeRootSignature(RootSignature);
}

void D3D12ComputeCommandContext::SetComputeResourceTable(const IShaderResourceTable* InTable)
{
	D3D12ShaderResourceTable* Table = (D3D12ShaderResourceTable*)InTable;
	CurrentTable = Table;

	bHasCheckedCurrentTable = false;

	if (Table->AssociatedSignature.RootSignature != RootSignature)
	{
		D3DCL->SetComputeRootSignature(Table->AssociatedSignature.RootSignature);
	}

	for (uint i = 0; i < Table->ConstantBuffers.size(); i++)
	{
		if (Table->ConstantBuffers[i] != nullptr)
		{
			D3DCL->SetComputeRootConstantBufferView(i, Table->ConstantBuffers[i]->GetGPUVirtualAddress());
		}
	}

	if (Table->HasTextures())
	{
		D3DCL->SetComputeRootDescriptorTable(Table->GetSRVDescriptorTableIndex(), Table->BaseSRVAllocation.GpuHandle);
		D3DCL->SetComputeRootDescriptorTable(Table->GetSamplerTableIndex(), Table->BaseSMPAllocation.GpuHandle);
	}
	if (Table->HasUAVs())
	{
		D3DCL->SetComputeRootDescriptorTable(Table->GetUAVDescriptorTableIndex(), Table->BaseUAVAllocation.GpuHandle);
	}
}

void D3D12ComputeCommandContext::Dispatch(uint ThreadX, uint ThreadY, uint ThreadZ)
{
	FlushResourceTransitions();
	CommitResources();
	D3DCL->Dispatch(ThreadX, ThreadY, ThreadZ);
}

void D3D12ComputeCommandContext::FlushResourceTransitions()
{
	ParentDevice->EnsureAllUploadsOccured();
	if (PendingTransitions.GetNumElements() > 0)
	{
		D3DCL->ResourceBarrier(PendingTransitions.GetNumElements(), PendingTransitions.Data.data());
	}
	for (uint i = 0; i < TransitionedResources.GetNumElements(); i++)
	{
		D3D12GPUResource* Res = TransitionedResources.Pop();
		Res->CurrentState = Res->PendingState;
	}
}

void D3D12ComputeCommandContext::CommitResources()
{
	if (bHasCheckedCurrentTable)
	{
		return;
	}

	bHasCheckedCurrentTable = true;
	// Leave out constant buffers because they are constant read?
	if (!CurrentTable->ShaderResources.empty())
	{
		for (uint i = 0; i < CurrentTable->ShaderResources.size(); i++)
		{
			D3D12GPUResource* pResource = CurrentTable->ShaderResources[i];
			if (pResource && pResource->RegisterDependency(COMMAND_CONTEXT_TYPE_COMPUTE))
			{
				CommandList->bNeedsWaitForGraphicsQueue = true;
			}
		}
	}
	if (!CurrentTable->UnorderedAccessResources.empty())
	{
		for (uint i = 0; i < CurrentTable->UnorderedAccessResources.size(); i++)
		{
			D3D12GPUResource* pResource = CurrentTable->UnorderedAccessResources[i];
			if (pResource && pResource->RegisterDependency(COMMAND_CONTEXT_TYPE_COMPUTE))
			{
				CommandList->bNeedsWaitForGraphicsQueue = true;
			}
		}
	}
}
