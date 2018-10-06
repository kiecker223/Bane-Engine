#include "GraphicsCommandList.h"

namespace
{
	class BeginPassCommand : public IGraphicsCommand
	{
	public:
		BeginPassCommand(IRenderPassInfo* InRenderPass) :
			RenderPass(InRenderPass)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->BeginPass(RenderPass);
		}

		IRenderPassInfo* RenderPass;

	};
	class EndPassCommand : public IGraphicsCommand
	{
	public:
		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->EndPass();
		}
	};
	class FlushCommand : public IGraphicsCommand
	{
	public:
		FlushCommand()
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->Flush();
		}
	};
	class BeginCommand : public IGraphicsCommand
	{
	public:
		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->Begin();
		}
	};
	class EndCommand : public IGraphicsCommand
	{
	public:
		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->End();
		}
	};
	class SetGraphicsPipelineStateCommand : public IGraphicsCommand
	{
	public:
		SetGraphicsPipelineStateCommand(IGraphicsPipelineState* InPipelineState) :
			PipelineState(InPipelineState)
		{
		}
		void Execute(IGraphicsCommandContext* Context) override final
		{
			Context->SetGraphicsPipelineState(PipelineState);
		}
		const IGraphicsPipelineState* PipelineState;
	};
	class SetVertexBufferCommand : public IGraphicsCommand
	{
	public:

		SetVertexBufferCommand(const IVertexBuffer* InVertexBuffer) : VertexBuffer(InVertexBuffer)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->SetVertexBuffer(VertexBuffer);
		}

		const IVertexBuffer* VertexBuffer;
	};
	class SetIndexBufferCommand : public IGraphicsCommand
	{
	public:

		SetIndexBufferCommand(const IIndexBuffer* InIndexBuffer) : IndexBuffer(InIndexBuffer)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->SetIndexBuffer(IndexBuffer);
		}

		const IIndexBuffer* IndexBuffer;

	};
	class SetPrimitiveTopologyCommand : public IGraphicsCommand
	{
	public:
		SetPrimitiveTopologyCommand(EPRIMITIVE_TOPOLOGY InTopology) : Topology(InTopology)
		{
		}

		void Execute(IGraphicsCommandContext* Context)
		{
			Context->SetPrimitiveTopology(Topology);
		}

		EPRIMITIVE_TOPOLOGY Topology;
	};
	class SetGraphicsResourceTableCommand : public IGraphicsCommand
	{
	public:
		SetGraphicsResourceTableCommand(const IShaderResourceTable* InTable) : Table(InTable)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->SetGraphicsResourceTable(Table);
		}

		const IShaderResourceTable* Table;
	};
	class CopyBuffersCommand : public IGraphicsCommand
	{
	public:
		CopyBuffersCommand(IBuffer* InSrc, IBuffer* InDst) : Src(InSrc), Dst(InDst)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->CopyBuffers(Src, Dst);
		}

		IBuffer* Src;
		IBuffer* Dst;
	};
	class CopyBufferToTextureCommand : public IGraphicsCommand
	{
	public:

		CopyBufferToTextureCommand(IBuffer* InSrc, ITextureBase* InDst) : Src(InSrc), Dst(InDst)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->CopyBufferToTexture(Src, Dst);
		}

		IBuffer* Src;
		ITextureBase* Dst;
	};
	class CopyTextures1Command : public IGraphicsCommand
	{
	public:
		CopyTextures1Command(ITextureBase* InSrc, uint32 InSrcSubresource, ITextureBase* InDst, uint32 InDstSubresource) :
			Src(InSrc),
			SrcSubresource(InSrcSubresource),
			Dst(InDst),
			DstSubresource(InDstSubresource)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->CopyTextures(Src, SrcSubresource, Dst, DstSubresource);
		}

		ITextureBase* Src;
		uint32 SrcSubresource;
		ITextureBase* Dst;
		uint32 DstSubresource;
	};
	class CopyTextures2Command : public IGraphicsCommand
	{
		CopyTextures2Command(ITextureBase* InSrc, int3 InSrcLocation, ITextureBase* InDst, int3 InDstLocation, int3 InDstSize) :
			Src(InSrc),
			SrcLocation(InSrcLocation),
			Dst(InDst),
			DstLocation(InDstLocation),
			DstSize(InDstSize)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->CopyTextures(Src, SrcLocation, Dst, DstLocation, DstSize);
		}

		ITextureBase* Src;
		int3 SrcLocation;
		ITextureBase* Dst;
		int3 DstLocation;
		int3 DstSize;
	};
	class DrawCommand : public IGraphicsCommand
	{
	public:
		DrawCommand(uint32 InVertexCount, uint32 InStartVertexLocation) :
			VertexCount(InVertexCount), StartVertexLocation(InStartVertexLocation)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->Draw(VertexCount, StartVertexLocation);
		}

		uint32 VertexCount, StartVertexLocation;
	};
	class DrawIndexedCommand : IGraphicsCommand
	{
	public:
		DrawIndexedCommand(uint32 InIndexCount, uint32 InStartIndexLocation, int InBaseVertexLocation) :
			IndexCount(InIndexCount), StartIndexLocation(InStartIndexLocation), BaseVertexLocation(InBaseVertexLocation)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
		}

		uint32 IndexCount;
		uint32 StartIndexLocation;
		int BaseVertexLocation;
	};
	class DrawInstancedCommand : IGraphicsCommand
	{
	public:
		DrawInstancedCommand(uint32 InVertexCount, uint32 InInstanceCount, uint32 InStartVertexLocation) :
			VertexCount(InVertexCount),
			InstanceCount(InInstanceCount),
			StartVertexLocation(InStartVertexLocation)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->DrawInstanced(VertexCount, InstanceCount, StartVertexLocation);
		}

		uint32 VertexCount;
		uint32 InstanceCount;
		uint32 StartVertexLocation;
	};
	class DrawIndexedInstancedCommand : public IGraphicsCommand
	{
	public:

		DrawIndexedInstancedCommand(uint32 InIndexCount, uint32 InStartIndexLocation, int InBaseVertexLocation, uint32 InInstanceCount) :
			IndexCount(InIndexCount), StartIndexLocation(InStartIndexLocation), BaseVertexLocation(InBaseVertexLocation), InstanceCount(InInstanceCount)
		{
		}

		uint32 IndexCount;
		uint32 StartIndexLocation;
		int BaseVertexLocation;
		uint32 InstanceCount;
	};
	class SetComputePipelineStateCommand : public IGraphicsCommand
	{
	public:
		SetComputePipelineStateCommand(const IComputePipelineState* InPipelineState) : PipelineState(InPipelineState)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->SetComputePipelineState(PipelineState);
		}

		const IComputePipelineState* PipelineState;

	};
	class SetComputeResourceTableCommand : public IGraphicsCommand
	{
	public:
		SetComputeResourceTableCommand(const IShaderResourceTable* InTable) : Table(InTable)
		{
		}

		void Execute(IGraphicsCommandContext* Context) override final
		{
			Context->SetComputeResourceTable(Table);
		}

		const IShaderResourceTable* Table;
	};
	class DispatchCommand : public IGraphicsCommand
	{
	public:
		DispatchCommand(uint32 InThreadX, uint32 InThreadY, uint32 InThreadZ) :
			ThreadX(InThreadX),
			ThreadY(InThreadY),
			ThreadZ(InThreadZ)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->Dispatch(ThreadX, ThreadY, ThreadZ);
		}

		uint32 ThreadX;
		uint32 ThreadY;
		uint32 ThreadZ;
	};
}


GraphicsCommandBuffer::GraphicsCommandBuffer()
{

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

byte* GraphicsCommandBuffer::Allocate(uint32 NewOffset)
{
	UNUSED(NewOffset);
	return nullptr;
}
