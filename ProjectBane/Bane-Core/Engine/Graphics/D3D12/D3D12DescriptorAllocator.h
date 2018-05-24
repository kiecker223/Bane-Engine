#pragma once

#include <d3d12.h>
#include <vector>
#include "Core/Common.h"

class D3D12DescriptorAllocation
{
public:

	ForceInline void Increment()
	{
		CpuHandle.ptr += IncrementSize;
		GpuHandle.ptr += IncrementSize;
	}

	ForceInline void Increment(uint Count)
	{
		CpuHandle.ptr += (IncrementSize * Count);
		GpuHandle.ptr += (IncrementSize * Count);
	}

	ForceInline void Decrement()
	{
		CpuHandle.ptr -= IncrementSize;
		GpuHandle.ptr -= IncrementSize;
	}

	ForceInline D3D12DescriptorAllocation OffsetFromStart(uint Count)
	{
		D3D12DescriptorAllocation Result = *this;
		Result.CpuHandle.ptr += (IncrementSize * Count);
		Result.GpuHandle.ptr += (IncrementSize * Count);
		return Result;
	}

	uint IncrementSize;
	D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
};

class D3D12DescriptorAllocator
{
public:

	void Initialize(ID3D12Device1* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint Size, bool bShaderVisible = false);
	void Destroy();

	D3D12DescriptorAllocation AllocateDescriptor();

	D3D12DescriptorAllocation AllocateMultiple(uint Count);

	void Free(const D3D12DescriptorAllocation& Handle);

	ForceInline ID3D12DescriptorHeap* GetDescriptorHeap()
	{
		return m_DescriptorHeap;
	}

private:

	std::vector<D3D12DescriptorAllocation> m_AllocList;
	std::vector<D3D12DescriptorAllocation> m_FreeList;
	ID3D12DescriptorHeap* m_DescriptorHeap;
	uint m_IncrementSize;
	D3D12DescriptorAllocation m_Start;
	D3D12DescriptorAllocation m_Current;
	D3D12DescriptorAllocation m_End;
};

