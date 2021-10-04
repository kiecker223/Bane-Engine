#pragma once

#include "../Interfaces/GraphicsCommandList.h"
#include "Core/Containers/StackQueue.h"
#include "D3D12GraphicsResource.h"
#include "D3D12ResourceCache.h"
#include <mutex>

class D3D12ResourceLocation;
class D3D12GraphicsDevice;
class D3D12RenderPassInfo;
class D3D12RenderTargetView;
class D3D12DepthStencilView;

class D3D12CommandList : public D3D12DeviceChild
{
public:

	struct UploadResource
	{
		D3D12GPUResource* Resource;
		void Destroy()
		{
			delete Resource;
		}
	};

	struct TemporaryTextureAllocation
	{
		D3D12TextureBase* Texture;

		void Destroy()
		{
			delete Texture;
		}
	};

	struct DedicatedResource
	{
	public:
		D3D12GPUResource* Resource;
		void Uncommit()
		{
			Resource->Uncommit();
		}
	};

	D3D12CommandList(ID3D12CommandAllocator* InCommandAllocator, ID3D12CommandList* InCommandList, D3D12GraphicsDevice* ParentDevice) :
		CommandAllocator(InCommandAllocator),
		CommandList(InCommandList)
	{
		SetParentDevice(ParentDevice);
	}



	inline void EnqueueUploadResourceToDestroy(D3D12GPUResource* Resource)
	{
		UploadResource Res;
		Res.Resource = Resource;
		UploadResourcesToDestroy.push_back(Res);
	}

	// See if this is even needed, because I imagine this incurs a significant perf cost
	inline void CommitResourceToExecution(D3D12GPUResource* Resource)
	{
		DedicatedResource Res;
		Res.Resource = Resource;
		CommitedResources.push_back(Res);
	}

	inline D3D12CommandList& operator = (const D3D12CommandList& Rhs)
	{
		CommandAllocator = Rhs.CommandAllocator;
		CommandList = Rhs.CommandList;
		return *this;
	}

	void FlushDestructionQueue();

	inline void FlushCommitQueue()
	{
		if (CommitedResources.empty())
		{
			return;
		}
		std::lock_guard<std::mutex> CommitQueueGuard(CommitQueueLock);
		for (uint32 i = 0; i < CommitedResources.size(); i++)
		{
			CommitedResources[i].Uncommit();
		}
		CommitedResources.clear();
	}

	inline void Reset()
	{
		if (bCanReset)
		{
			CommandAllocator->Reset();
			GetGraphicsCommandList()->Reset(CommandAllocator, nullptr);
			bCanReset = false;
			bCanClose = true;
		}
	}

	inline void Close()
	{
		if (bCanClose)
		{
			bCanClose = false;
			bCanReset = true;
			GetGraphicsCommandList()->Close();
		}
	}

	inline void DestroyRenderTargets()
	{
		for (uint32 i = 0; i < TemporaryTextures.size(); i++)
		{
			TemporaryTextures[i].Destroy();
		}
		TemporaryTextures.clear();
	}

	inline ID3D12GraphicsCommandList* GetGraphicsCommandList()
	{
		return (ID3D12GraphicsCommandList*)CommandList;
	}

	ID3D12CommandAllocator* CommandAllocator;
	ID3D12CommandList* CommandList;

	std::vector<UploadResource> UploadResourcesToDestroy;
	std::vector<DedicatedResource> CommitedResources;
	std::vector<TemporaryTextureAllocation> TemporaryTextures;

	std::mutex DeletionQueueLock;
	std::mutex CommitQueueLock;
	std::mutex ResetLock;
	bool bCanReset = false;
	bool bCanClose = true;
	bool bNeedsWaitForComputeQueue = false;
	bool bNeedsWaitForGraphicsQueue = false;
};

class D3D12GraphicsCommandBuffer : public IGraphicsCommandBuffer
{
public:

	D3D12GraphicsCommandBuffer() = delete;

	D3D12GraphicsCommandBuffer(D3D12GraphicsDevice* InDevice, ECOMMAND_CONTEXT_TYPE InContextType, D3D12_RECT InRect, D3D12_VIEWPORT InViewport) :
		CommandList(nullptr),
		ParentDevice(InDevice),
		ContextType(InContextType),
		ScissorRect(InRect),
		Viewport(InViewport),
		NumCurrentRenderTargetViews(0),
		CurrentRenderTargetViews{ nullptr },
		CurrentDepthStencilView(nullptr),
		NumDrawCalls(0),
		NumDispatches(0),
		NumCopies(0)
	{
		//FetchNewCommandList();
	}

	inline bool HasBegun() { return CommandList != nullptr; }

	void Begin();

	virtual void SetRenderTarget(IRenderTargetView* pRenderTargetView, IDepthStencilView* pDepthStencilView) override final;
	virtual void SetRenderTargets(const std::vector<IRenderTargetView*>& RenderTargetViews, IDepthStencilView* pDepthStencilView) override final;

	virtual void ClearRenderTargets() override final;

	virtual void CloseCommandBuffer() override final;

	virtual void SetGraphicsPipelineState(const IGraphicsPipelineState* InPipelineState) final override;

	virtual void SetTexture(uint32 Slot, const ITextureBase* InTexture, uint32 Subresource) override final;
	virtual void SetStructuredBuffer(uint32 Slot, const IBuffer* InBuffer, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride) override final;
	virtual void SetUnorderedAccessView(uint32 Slot, const ITextureBase* InResource, uint32 Subresource) override final;
	virtual void SetUnorderedAccessView(uint32 Slot, const IBuffer* InResource, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride) override final;
	virtual void SetConstantBuffer(uint32 Slot, const IBuffer* InBuffer, uint64 Offset) override final;

	virtual void SetVertexBuffer(const IBuffer* InVertexBuffer) final override;
	virtual void SetVertexBuffer(const IBuffer* InVertexBuffer, uint64 Offset) final override;
	virtual void SetIndexBuffer(const IBuffer* InIndexBuffer) final override;
	virtual void SetIndexBuffer(const IBuffer* InIndexBuffer, uint64 Offset) final override;
	virtual void SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY InTopology) final override;

	virtual void Draw(uint32 VertexCount, uint32 StartVertexLocation) final override;
	virtual void DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation) final override;
	virtual void DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation) final override;
	virtual void DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount) final override;

	virtual void CopyBufferLocations(IBuffer* Src, uint64 SrcOffset, IBuffer* Dst, uint64 DstOffset, uint64 NumBytes) final override;
	virtual void CopyBuffers(IBuffer* Src, IBuffer* Dst) final override;
	virtual void CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst) final override;
	virtual void CopyTextures(ITextureBase* InSrc, uint32 SrcSubresource, ITexture2D* InDst, uint32 DstSubresource) final override;
	virtual void CopyTextures(ITextureBase* Src, ivec3 SrcLocation, ITextureBase* Dst, ivec3 DstLocation, ivec3 DstSize) final override;

	virtual void SetComputePipelineState(const IComputePipelineState* InState) final override;

	virtual void SetComputeTexture(uint32 Slot, const ITextureBase* InTexture, uint32 Subresource) override final;
	virtual void SetComputeStructuredBuffer(uint32 Slot, const IBuffer* InBuffer, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride) override final;
	virtual void SetComputeUnorderedAccessView(uint32 Slot, const ITextureBase* InResource, uint32 Subresource) override final;
	virtual void SetComputeUnorderedAccessView(uint32 Slot, const IBuffer* InResource, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride) override final;
	virtual void SetComputeConstantBuffer(uint32 Slot, const IBuffer* InBuffer, uint64 Offset) override final;

	virtual ITexture2D* CreateTemporaryTexture(uint32 Width, uint32 Height, EFORMAT Format, const SAMPLER_DESC& SampleDesc, ETEXTURE_USAGE Usage) override final;
	virtual IRenderTargetView* CreateTemporaryRenderTargetView(ITexture2D* InTexture) override final;
	virtual IDepthStencilView* CreateTemporaryDepthStencilView(ITexture2D* InTexture) override final;

	virtual void DestroyTemporaryTexture(ITexture2D* pTexture) override final;
	virtual void DestroyTemporaryRenderTargetView(IRenderTargetView* pView) override final;
	virtual void DestroyTemporaryDepthStencilView(IDepthStencilView* pView) override final;

	virtual void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ) final override;

	void InitializeAllocators(D3D12LinearDescriptorAllocator& ParentSrvAllocator, D3D12LinearDescriptorAllocator& ParentSmpAllocator);

	void FlushResourceTransitions();

	void CommitGraphicsResourcesToExecution();
	void CommitComputeResourcesToExecution();
	void CommitResources();

	void FetchNewCommandList();
	void ReturnCommandList();

	void TransitionRenderTargetsToRead(D3D12RenderTargetView** ppViews, uint32 NumRenderTargets, D3D12DepthStencilView* pDepthStencilView);
	void TransitionRenderTargetsToWrite(D3D12RenderTargetView** ppViews, uint32 NumRenderTargets, D3D12DepthStencilView* pDepthStencilView);

	uint32 NumDrawCalls;
	uint32 NumDispatches;
	uint32 NumCopies;

	inline bool HasDoneWork() const { return (NumDrawCalls + NumDispatches + NumCopies) != 0; }
	inline uint32 GetTotalWorkDone() const { return (NumDrawCalls + NumDispatches + NumCopies); }

	bool bHasCheckedCurrentTable = false;
	
	ECOMMAND_CONTEXT_TYPE ContextType;
	D3D12CommandList* CommandList;
	
	D3D12GraphicsPipelineState* GraphicsPipelineState;
	D3D12ComputePipelineState* ComputePipelineState;
	
	D3D12RenderTargetView* CurrentRenderTargetViews[32];
	uint32 NumCurrentRenderTargetViews;
	D3D12DepthStencilView* CurrentDepthStencilView;
	ID3D12GraphicsCommandList* D3DCL;
	
	D3D12ShaderSignature RootSignature;

	D3D12LinearDescriptorAllocator SrvDescriptorAllocator;
	D3D12LinearDescriptorAllocator SmpDescriptorAllocator;
	
	D3D12ResourceCache GraphicsResources;
	D3D12ResourceCache ComputeResources;
	D3D12GraphicsDevice* ParentDevice;

	std::vector<D3D12_RESOURCE_BARRIER> PendingTransitions;
	std::vector<D3D12GPUResource*> TransitionedResources;
	
	D3D12_RECT ScissorRect;
	D3D12_VIEWPORT Viewport;

};

class D3D12GraphicsCommandContext : public IGraphicsCommandContext
{
public:

	D3D12GraphicsCommandContext(D3D12GraphicsDevice* InDevice, ECOMMAND_CONTEXT_TYPE InContextType, D3D12_RECT InRect, D3D12_VIEWPORT InViewport) :
		ParentDevice(InDevice),
		ContextType(InContextType),
		ScissorRect(InRect),
		Viewport(InViewport)
	{
		uint32 NumCommandListsAllowed = 0;
		switch (ContextType)
		{
		case COMMAND_CONTEXT_TYPE_GRAPHICS: NumCommandListsAllowed = max(std::thread::hardware_concurrency() - 1, 6); break;
		case COMMAND_CONTEXT_TYPE_COMPUTE: NumCommandListsAllowed = 3; break;
		case COMMAND_CONTEXT_TYPE_COPY: NumCommandListsAllowed = 5; break;
		}
		for (uint32 i = 0; i < NumCommandListsAllowed; i++)
		{
			CommandPool.Push(new D3D12GraphicsCommandBuffer(ParentDevice, ContextType, ScissorRect, Viewport));
		}
		CurrentCommandBuffer = CommandPool.Pop();
	}

	inline bool HasBegun() 
	{ 
		if (CurrentCommandBuffer->CommandList)
		{
			return CurrentCommandBuffer->CommandList->bCanClose; 
		}
		return false;
	}

	void Begin();

	virtual void SetRenderTarget(IRenderTargetView* pRenderTarget, IDepthStencilView* pDepthStencil) override final;
	virtual void SetRenderTargets(const std::vector<IRenderTargetView*>& RenderTargetViews, IDepthStencilView* pDepthStencil) override final;

	virtual void ClearRenderTargets() override final;

	virtual void Flush() override final;

	virtual IGraphicsCommandBuffer* GetUnderlyingCommandBuffer() final override;
	virtual IGraphicsCommandBuffer* CreateCommandBuffer() final override;
	virtual void ExecuteCommandBuffer(IGraphicsCommandBuffer* InCommandBuffer) final override;
	virtual void ExecuteCommandBuffers(const std::vector<IGraphicsCommandBuffer*>& InCommandBuffers) final override;

	virtual void SetGraphicsPipelineState(const IGraphicsPipelineState* InPipelineState) final override;

	virtual void SetTexture(uint32 Slot, const ITextureBase* InTexture, uint32 Subresource) override final;
	virtual void SetStructuredBuffer(uint32 Slot, const IBuffer* InBuffer, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride) override final;
	virtual void SetUnorderedAccessView(uint32 Slot, const ITextureBase* InResource, uint32 Subresource) override final;
	virtual void SetUnorderedAccessView(uint32 Slot, const IBuffer* InResource, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride) override final;
	virtual void SetConstantBuffer(uint32 Slot, const IBuffer* InBuffer, uint64 Offset) override final;

	virtual void SetVertexBuffer(const IBuffer* InVertexBuffer) final override;
	virtual void SetVertexBuffer(const IBuffer* InVertexBuffer, uint64 Offset) final override;
	virtual void SetIndexBuffer(const IBuffer* InIndexBuffer) final override;
	virtual void SetIndexBuffer(const IBuffer* InIndexBuffer, uint64 Offset) final override;
	virtual void SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY InTopology) final override;
	
	virtual void Draw(uint32 VertexCount, uint32 StartVertexLocation) final override;
	virtual void DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation) final override;
	virtual void DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation) final override;
	virtual void DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount) final override;

	virtual void CopyBufferLocations(IBuffer* Src, uint64 SrcOffset, IBuffer* Dst, uint64 DstOffset, uint64 NumBytes) final override;
	virtual void CopyBuffers(IBuffer* Src, IBuffer* Dst) final override;
	virtual void CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst) final override;
	virtual void CopyTextures(ITextureBase* InSrc, uint32 SrcSubresource, ITexture2D* InDst, uint32 DstSubresource) final override;
	virtual void CopyTextures(ITextureBase* Src, ivec3 SrcLocation, ITextureBase* Dst, ivec3 DstLocation, ivec3 DstSize) final override;

	virtual void SetComputePipelineState(const IComputePipelineState* InState) final override;

	virtual void SetComputeTexture(uint32 Slot, const ITextureBase* InTexture, uint32 Subresource) override final;
	virtual void SetComputeStructuredBuffer(uint32 Slot, const IBuffer* InBuffer, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride) override final;
	virtual void SetComputeUnorderedAccessView(uint32 Slot, const ITextureBase* InResource, uint32 Subresource) override final;
	virtual void SetComputeUnorderedAccessView(uint32 Slot, const IBuffer* InResource, uint32 IndexToStart, uint32 NumElements, uint32 StructureByteStride) override final;
	virtual void SetComputeConstantBuffer(uint32 Slot, const IBuffer* InBuffer, uint64 Offset) override final;

	virtual ITexture2D* CreateTemporaryTexture(uint32 Width, uint32 Height, EFORMAT Format, const SAMPLER_DESC& SampleDesc, ETEXTURE_USAGE Usage) override final;
	virtual IRenderTargetView* CreateTemporaryRenderTargetView(ITexture2D* InTexture) override final;
	virtual IDepthStencilView* CreateTemporaryDepthStencilView(ITexture2D* InTexture) override final;

	virtual void DestroyTemporaryTexture(ITexture2D* pTexture) override final;
	virtual void DestroyTemporaryRenderTargetView(IRenderTargetView* pView) override final;
	virtual void DestroyTemporaryDepthStencilView(IDepthStencilView* pView) override final;

	virtual void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ) final override;

	D3D12_RECT ScissorRect;
	D3D12_VIEWPORT Viewport;
	ECOMMAND_CONTEXT_TYPE ContextType;
	D3D12GraphicsDevice* ParentDevice;
	D3D12GraphicsCommandBuffer* CurrentCommandBuffer;
	TStack<D3D12GraphicsCommandBuffer*, 16> CommandPool;
};

class D3D12ComputeCommandContext : public IComputeCommandContext
{
public:

	D3D12ComputeCommandContext(D3D12GraphicsDevice* Device) :
		CommandList(nullptr),
		ParentDevice(Device)
	{
	}

	inline bool HasBegun() const { return CommandList != nullptr; }

	virtual void Begin() final override;
	virtual void End() final override;

	virtual void StallToEnd() final override;
	virtual void Flush() final override { StallToEnd(); }

	virtual void SetComputePipelineState(const IComputePipelineState* InState) final override;

	virtual void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ) final override;

	void CommitResources();

	void FlushResourceTransitions();

	bool bHasCheckedCurrentTable = false;
	ID3D12GraphicsCommandList* D3DCL;
	D3D12CommandList* CommandList;
	D3D12ComputePipelineState* PipelineState;
	ID3D12RootSignature* RootSignature;
	D3D12GraphicsDevice* ParentDevice;
	std::vector<D3D12_RESOURCE_BARRIER> PendingTransitions;
	std::vector<D3D12GPUResource*> TransitionedResources;
};
