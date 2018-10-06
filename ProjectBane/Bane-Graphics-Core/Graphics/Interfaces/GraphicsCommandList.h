#pragma once

#include "GraphicsResources.h"
#include "PipelineState.h"
#include "ShaderResourceView.h"
#include <utility>



// Purely internal use
typedef enum ECOMMAND_CONTEXT_TYPE {
	COMMAND_CONTEXT_TYPE_INVALID = -1,
	COMMAND_CONTEXT_TYPE_GRAPHICS = 0,
	COMMAND_CONTEXT_TYPE_COMPUTE = 1,
	COMMAND_CONTEXT_TYPE_COPY = 2,
	COMMAND_CONTEXT_TYPE_NUM_TYPES = 3
} ECOMMAND_CONTEXT_TYPE;

typedef enum EPRIMITIVE_TOPOLOGY {
	PRIMITIVE_TOPOLOGY_POINTLIST,
	PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	PRIMITIVE_TOPOLOGY_TRIANGLESTRIPS
} EPRIMITIVE_TOPOLOGY;

class IGraphicsCommandContext
{
public:
	virtual ~IGraphicsCommandContext() { }

	virtual void BeginPass(IRenderPassInfo* RenderPass) = 0;
	virtual void EndPass() = 0; // This will submit the workload for execution, no need to manually release it, it will be taken care of on a per-thread basis

	virtual void Flush() = 0; // Stall until execution is finished

	virtual void Begin() { BeginPass(nullptr); } // No graphics tasks are allowed to be done
	virtual void End() { EndPass(); }

	virtual void SetGraphicsPipelineState(const IGraphicsPipelineState* PipelineState) = 0;
	virtual void SetVertexBuffer(const IVertexBuffer* VertexBuffer) = 0;
	virtual void SetIndexBuffer(const IIndexBuffer* IndexBuffer) = 0;
	virtual void SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY Topology) = 0;

	virtual void SetGraphicsResourceTable(const IShaderResourceTable* InTable) = 0;

	virtual void CopyBuffers(IBuffer* Src, IBuffer* Dst) = 0;
	virtual void CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst) = 0;
	virtual void CopyTextures(ITextureBase* Src, uint32 SrcSubresource, ITextureBase* Dst, uint32 DstSubresource) = 0;
	virtual void CopyTextures(ITextureBase* Src, int3 SrcLocation, ITextureBase* Dst, int3 DstLocation, int3 DstSize) = 0;

	virtual void Draw(uint32 VertexCount, uint32 StartVertexLocation) = 0;
	virtual void DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation) = 0;
	virtual void DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation) = 0;
	virtual void DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount) = 0;

	virtual void SetComputePipelineState(const IComputePipelineState* PipelineState) = 0;
	virtual void SetComputeResourceTable(const IShaderResourceTable* InTable) = 0;

	virtual void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ) = 0;
};


class IComputeCommandContext
{
public:
	virtual ~IComputeCommandContext() { }

	virtual void Begin() = 0;
	virtual void End() = 0;

	virtual void StallToEnd() = 0;
	virtual void Flush() = 0;

	virtual void SetComputePipelineState(const IComputePipelineState* PipelineState) = 0;
	virtual void SetComputeResourceTable(const IShaderResourceTable* InTable) = 0;

	virtual void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ) = 0;
};

class IGraphicsCommand
{
public:
	virtual ~IGraphicsCommand() { } 
	virtual void Execute(IGraphicsCommandContext* Context) = 0;
};

namespace InternalCommands
{
	class BeginPassCommand : public IGraphicsCommand
	{
	public:
		explicit BeginPassCommand(IRenderPassInfo* InRenderPass) :
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
		EndPassCommand() { }
		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->EndPass();
		}
	};
	class FlushCommand : public IGraphicsCommand
	{
	public:
		FlushCommand() { }

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->Flush();
		}
	};
	class BeginCommand : public IGraphicsCommand
	{
	public:
		BeginCommand() { }

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->Begin();
		}
	};
	class EndCommand : public IGraphicsCommand
	{
	public:
		EndCommand() { }

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->End();
		}
	};
	class SetGraphicsPipelineStateCommand : public IGraphicsCommand
	{
	public:
		SetGraphicsPipelineStateCommand() { }
		SetGraphicsPipelineStateCommand(const IGraphicsPipelineState* InPipelineState) :
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

		SetVertexBufferCommand() { }

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

		SetIndexBufferCommand() { }

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

		SetPrimitiveTopologyCommand() { }

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

		SetGraphicsResourceTableCommand() { }

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

		CopyBuffersCommand() { }

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

		CopyBufferToTextureCommand() { }

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
		CopyTextures1Command() { }

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
	public:
		CopyTextures2Command() { }

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

		DrawCommand() { }

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

		DrawIndexedCommand() { }

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

		DrawInstancedCommand() { }

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

		DrawIndexedInstancedCommand() { }

		DrawIndexedInstancedCommand(uint32 InIndexCount, uint32 InStartIndexLocation, int InBaseVertexLocation, uint32 InInstanceCount) :
			IndexCount(InIndexCount), StartIndexLocation(InStartIndexLocation), BaseVertexLocation(InBaseVertexLocation), InstanceCount(InInstanceCount)
		{
		}

		void Execute(IGraphicsCommandContext* Context) final override
		{
			Context->DrawIndexedInstanced(IndexCount, StartIndexLocation, BaseVertexLocation, InstanceCount);
		}

		uint32 IndexCount;
		uint32 StartIndexLocation;
		int BaseVertexLocation;
		uint32 InstanceCount;
	};
	class SetComputePipelineStateCommand : public IGraphicsCommand
	{
	public:

		SetComputePipelineStateCommand() { }

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

		SetComputeResourceTableCommand() { }

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

		DispatchCommand() { }

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


class GraphicsCommandBuffer : public IGraphicsCommand
{
public:
	GraphicsCommandBuffer();
	GraphicsCommandBuffer(const GraphicsCommandBuffer& Other) { UNUSED(Other); }

	void Execute(IGraphicsCommandContext* Context) final override
	{
		for (IGraphicsCommand*& Command : Commands)
		{
			Command->Execute(Context);
		}
	}

	void BeginPass(IRenderPassInfo* RenderPass);
	void EndPass();

	void Flush();
	void Reset();

	void Begin() { BeginPass(nullptr); } // No graphics tasks are allowed to be done
	void End() { EndPass(); }

	void SetGraphicsPipelineState(const IGraphicsPipelineState* PipelineState);
	void SetVertexBuffer(const IVertexBuffer* VertexBuffer);
	void SetIndexBuffer(const IIndexBuffer* IndexBuffer);
	void SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY Topology);

	void SetGraphicsResourceTable(const IShaderResourceTable* InTable);

	void CopyBuffers(IBuffer* Src, IBuffer* Dst);
	void CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst);
	void CopyTextures(ITextureBase* Src, uint32 SrcSubresource, ITextureBase* Dst, uint32 DstSubresource);
	void CopyTextures(ITextureBase* Src, int3 SrcLocation, ITextureBase* Dst, int3 DstLocation, int3 DstSize);

	void Draw(uint32 VertexCount, uint32 StartVertexLocation);
	void DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation);
	void DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation);
	void DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount);

	void SetComputePipelineState(const IComputePipelineState* PipelineState);
	void SetComputeResourceTable(const IShaderResourceTable* InTable);

	void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ);

	template<typename T, class... U>
	inline void AllocateCommand(U&& ...Args)
	{
		Commands.push_back((IGraphicsCommand*)(new ((void*)Allocate(sizeof(T))) T(std::forward<U>(Args)...)));
	}
	template<typename T>
	inline void AllocateCommand()
	{
		Commands.push_back((IGraphicsCommand*)(new ((void*)Allocate(sizeof(T))) T()));
	}

	byte* Allocate(ptrdiff_t NewOffset);

	byte* PtrStart = nullptr;
	byte* PtrCurrent = nullptr;
	byte* PtrEnd = nullptr;
	std::vector<IGraphicsCommand*> Commands;
private:
	void Reallocate(size_t NewSize);
};
