#include "GraphicsCommandList.h"

using namespace InternalCommands;

GraphicsCommandBuffer::GraphicsCommandBuffer()
{
	Reallocate(2048);
}

void GraphicsCommandBuffer::BeginPass(IRenderPassInfo* RenderPass)
{
	AllocateCommand<BeginPassCommand>(RenderPass);
}

void GraphicsCommandBuffer::EndPass()
{
	AllocateCommand<EndPassCommand>();
}

void GraphicsCommandBuffer::Flush()
{
	AllocateCommand<FlushCommand>();
}

void GraphicsCommandBuffer::Reset()
{

}

void GraphicsCommandBuffer::SetGraphicsPipelineState(const IGraphicsPipelineState* PipelineState)
{
	AllocateCommand<SetGraphicsPipelineStateCommand>(PipelineState);
}

void GraphicsCommandBuffer::SetVertexBuffer(const IVertexBuffer* VertexBuffer)
{
	AllocateCommand<SetVertexBufferCommand>(VertexBuffer);
}

void GraphicsCommandBuffer::SetIndexBuffer(const IIndexBuffer* IndexBuffer)
{
	AllocateCommand<SetIndexBufferCommand>(IndexBuffer);
}

void GraphicsCommandBuffer::SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY Topology)
{
	AllocateCommand<SetPrimitiveTopologyCommand>(Topology);
}

void GraphicsCommandBuffer::SetGraphicsResourceTable(const IShaderResourceTable* InTable)
{
	AllocateCommand<SetGraphicsResourceTableCommand>(InTable);
}

void GraphicsCommandBuffer::CopyBuffers(IBuffer* Src, IBuffer* Dst)
{
	AllocateCommand<CopyBuffersCommand>(Src, Dst);
}

void GraphicsCommandBuffer::CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst)
{
	AllocateCommand<CopyBufferToTextureCommand>(Src, Dst);
}

void GraphicsCommandBuffer::CopyTextures(ITextureBase* Src, uint32 SrcSubresource, ITextureBase* Dst, uint32 DstSubresource)
{
	AllocateCommand<CopyTextures1Command>(Src, SrcSubresource, Dst, DstSubresource);
}

void GraphicsCommandBuffer::CopyTextures(ITextureBase* Src, int3 SrcLocation, ITextureBase* Dst, int3 DstLocation, int3 DstSize)
{
	AllocateCommand<CopyTextures2Command>(Src, SrcLocation, Dst, DstLocation, DstSize);
}

void GraphicsCommandBuffer::Draw(uint32 VertexCount, uint32 StartVertexLocation)
{
	AllocateCommand<DrawCommand>(VertexCount, StartVertexLocation);
}

void GraphicsCommandBuffer::DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation)
{
	AllocateCommand<DrawIndexedCommand>(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void GraphicsCommandBuffer::DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation)
{
	AllocateCommand<DrawInstancedCommand>(VertexCount, InstanceCount, StartVertexLocation);
}

void GraphicsCommandBuffer::DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount)
{
	AllocateCommand<DrawIndexedInstancedCommand>(IndexCount, StartIndexLocation, BaseVertexLocation, InstanceCount);
}

void GraphicsCommandBuffer::SetComputePipelineState(const IComputePipelineState* PipelineState)
{
	AllocateCommand<SetComputePipelineStateCommand>(PipelineState);
}

void GraphicsCommandBuffer::SetComputeResourceTable(const IShaderResourceTable* InTable)
{
	AllocateCommand<SetComputeResourceTableCommand>(InTable);
}

void GraphicsCommandBuffer::Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ)
{
	AllocateCommand<DispatchCommand>(ThreadX, ThreadY, ThreadZ);
}

uint8* GraphicsCommandBuffer::Allocate(ptrdiff_t NewOffset)
{
	if (PtrCurrent + NewOffset > PtrEnd)
	{
		uint8* OldPtr = PtrStart;
		ptrdiff_t OldSize = PtrCurrent - OldPtr;

		Reallocate(OldSize * 2);
		for (uint32 i = 0; i < Commands.GetCount(); i++)
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

void GraphicsCommandBuffer::Reallocate(size_t NewSize)
{
	PtrStart = new byte[NewSize];
	PtrCurrent = PtrStart;
	PtrEnd = PtrStart + NewSize;
}
