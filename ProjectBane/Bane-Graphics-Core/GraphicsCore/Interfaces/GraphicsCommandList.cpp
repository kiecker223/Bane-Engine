//#include "GraphicsCommandList.h"

/*
using namespace InternalGraphicsCommands;

DefaultGraphicsCommandBufferImpl::DefaultGraphicsCommandBufferImpl()
{
	Reallocate(2048);
}

void DefaultGraphicsCommandBufferImpl::BeginPass(IRenderPassInfo* RenderPass)
{
	AllocateCommand<BeginPassCommand>(RenderPass);
}

void DefaultGraphicsCommandBufferImpl::EndPass()
{
	AllocateCommand<EndPassCommand>();
}

void DefaultGraphicsCommandBufferImpl::Flush()
{
	AllocateCommand<FlushCommand>();
}

void DefaultGraphicsCommandBufferImpl::Reset()
{
	PtrCurrent = PtrStart;
	Commands.clear();
}

void DefaultGraphicsCommandBufferImpl::SetGraphicsPipelineState(const IGraphicsPipelineState* PipelineState)
{
	AllocateCommand<SetGraphicsPipelineStateCommand>(PipelineState);
}

void DefaultGraphicsCommandBufferImpl::SetVertexBuffer(const IVertexBuffer* VertexBuffer)
{
	AllocateCommand<SetVertexBufferCommand>(VertexBuffer);
}

void DefaultGraphicsCommandBufferImpl::SetIndexBuffer(const IIndexBuffer* IndexBuffer)
{
	AllocateCommand<SetIndexBufferCommand>(IndexBuffer);
}

void DefaultGraphicsCommandBufferImpl::SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY Topology)
{
	AllocateCommand<SetPrimitiveTopologyCommand>(Topology);
}

void DefaultGraphicsCommandBufferImpl::SetTexture(uint32 Slot, ITextureBase * InTexture)
{
	AllocateCommand<SetTextureCommand>(Slot, InTexture);
}

void DefaultGraphicsCommandBufferImpl::SetStructuredBuffer(uint32 Slot, IBuffer * InBuffer)
{
	AllocateCommand<SetStructuredBufferCommand>(Slot, InBuffer);
}

void DefaultGraphicsCommandBufferImpl::SetUnorderedAccessView(uint32 Slot, ITextureBase * InResource)
{
	AllocateCommand<SetUnorderedAccessView1Command>(Slot, InResource);
}

void DefaultGraphicsCommandBufferImpl::SetUnorderedAccessView(uint32 Slot, IBuffer * InResource)
{
	AllocateCommand<SetUnorderedAccessView2Commmand>(Slot, InResource);
}

void DefaultGraphicsCommandBufferImpl::SetConstantBuffer(uint32 Slot, IBuffer * InBuffer)
{
	AllocateCommand<SetConstantBufferCommand>(Slot, InBuffer);
}

void DefaultGraphicsCommandBufferImpl::CopyBuffers(IBuffer* Src, IBuffer* Dst)
{
	AllocateCommand<CopyBuffersCommand>(Src, Dst);
}

void DefaultGraphicsCommandBufferImpl::CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst)
{
	AllocateCommand<CopyBufferToTextureCommand>(Src, Dst);
}

void DefaultGraphicsCommandBufferImpl::CopyTextures(ITextureBase* Src, uint32 SrcSubresource, ITextureBase* Dst, uint32 DstSubresource)
{
	AllocateCommand<CopyTextures1Command>(Src, SrcSubresource, Dst, DstSubresource);
}

void DefaultGraphicsCommandBufferImpl::CopyTextures(ITextureBase* Src, int3 SrcLocation, ITextureBase* Dst, int3 DstLocation, int3 DstSize)
{
	AllocateCommand<CopyTextures2Command>(Src, SrcLocation, Dst, DstLocation, DstSize);
}

void DefaultGraphicsCommandBufferImpl::Draw(uint32 VertexCount, uint32 StartVertexLocation)
{
	AllocateCommand<DrawCommand>(VertexCount, StartVertexLocation);
}

void DefaultGraphicsCommandBufferImpl::DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation)
{
	AllocateCommand<DrawIndexedCommand>(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void DefaultGraphicsCommandBufferImpl::DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation)
{
	AllocateCommand<DrawInstancedCommand>(VertexCount, InstanceCount, StartVertexLocation);
}

void DefaultGraphicsCommandBufferImpl::DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount)
{
	AllocateCommand<DrawIndexedInstancedCommand>(IndexCount, StartIndexLocation, BaseVertexLocation, InstanceCount);
}

void DefaultGraphicsCommandBufferImpl::SetComputePipelineState(const IComputePipelineState* PipelineState)
{
	AllocateCommand<SetComputePipelineStateCommand>(PipelineState);
}

void DefaultGraphicsCommandBufferImpl::SetComputeTexture(uint32 Slot, ITextureBase * InTexture)
{
	AllocateCommand<SetComputeTextureCommand>(Slot, InTexture);
}

void DefaultGraphicsCommandBufferImpl::SetComputeStructuredBuffer(uint32 Slot, IBuffer * InBuffer)
{
	AllocateCommand<SetStructuredBufferCommand>(Slot, InBuffer);
}

void DefaultGraphicsCommandBufferImpl::SetComputeUnorderedAccessView(uint32 Slot, ITextureBase * InResource)
{
	AllocateCommand<SetComputeUnorderedAccessView1Command>(Slot, InResource);
}

void DefaultGraphicsCommandBufferImpl::SetComputeUnorderedAccessView(uint32 Slot, IBuffer * InResource)
{
	AllocateCommand<SetComputeUnorderedAccessView2Commmand>(Slot, InResource);
}

void DefaultGraphicsCommandBufferImpl::SetComputeConstantBuffer(uint32 Slot, IBuffer * InBuffer)
{
	AllocateCommand<SetComputeConstantBufferCommand>(Slot, InBuffer);
}

void DefaultGraphicsCommandBufferImpl::Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ)
{
	AllocateCommand<DispatchCommand>(ThreadX, ThreadY, ThreadZ);
}

uint8* DefaultGraphicsCommandBufferImpl::Allocate(ptrdiff_t NewOffset)
{
	if (PtrCurrent + NewOffset > PtrEnd)
	{
		uint8* OldPtr = PtrStart;
		ptrdiff_t OldSize = PtrCurrent - OldPtr;

		Reallocate(OldSize * 2);
		for (uint32 i = 0; i < Commands.size(); i++)
		{
			auto*& Command = Commands[i];
			Command = reinterpret_cast<IGraphicsCommand*>(PtrStart + (reinterpret_cast<uint8*>(Command) - OldPtr));
		}
	}
	else
	{
		PtrCurrent += NewOffset;
	}
	return PtrCurrent;
}

void DefaultGraphicsCommandBufferImpl::Reallocate(size_t NewSize)
{
	PtrStart = new byte[NewSize];
	PtrCurrent = PtrStart;
	PtrEnd = PtrStart + NewSize;
}
*/