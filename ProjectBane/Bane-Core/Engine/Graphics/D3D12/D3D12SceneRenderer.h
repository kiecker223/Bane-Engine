#pragma once

#include "D3D12SwapChain.h"
#include "../Rendering/RendererInterface.h"


//class BasicD3D12Renderer : public SceneRenderer
//{
//
//public:
//
//	virtual void AllocateScene(Scene* pScene) override final { }
//
//	virtual void DumpCurrentScene() override final { }
//
//	virtual void Initialize() override final;
//
//	virtual void Render() override final;
//
//	virtual void Present() override final;
//
//	virtual void Shutdown() override final;
//
//	virtual Scene* GetScene() override final { return nullptr; }
//
//	virtual IDeviceSwapChain* GetSwapChain() { return &m_D3D12SwapChain; };
//
//	virtual bool SupportsAsyncContexts() { return true; }
//
//private:
//
//	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleForCurrentFrame();
//
//	void WaitForGPUFinish();
//
//	D3D12SwapChain m_D3D12SwapChain;
//	ID3D12Device1* m_Device;
//	ID3D12CommandQueue* m_CommandQueue;
//	ID3D12GraphicsCommandList* m_CommandList;
//	ID3D12CommandAllocator* m_CommandAllocator;
//	ID3D12DescriptorHeap* m_SRVHeap;
//	ID3D12DescriptorHeap* m_RTVHeap;
//	ID3D12DescriptorHeap* m_DSVHeap;
//	D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentSRVAllocation;
//	D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentRTVAllocation;
//	D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentDSVAllocation;
//	uint m_SRVHeapIncrementSize;
//	uint m_RTVHeapIncrementSize;
//	uint m_DSVHeapIncrementSize;
//	ID3D12Resource* m_BackBuffers[3];
//	HANDLE m_WaitEvent;
//	ID3D12Fence* m_GPUWaitFence;
//	uint64 m_FenceSignal;
//};
