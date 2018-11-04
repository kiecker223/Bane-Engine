#pragma once

#include <d3d12.h>
#include <Core/Containers/Array.h>
#include "Common.h"
#include "../D3DCommon/D3DCommon.h"

class D3D12DescriptorAllocation
{
public:

	inline void Increment()
	{
		CpuHandle.ptr += IncrementSize;
		GpuHandle.ptr += IncrementSize;
	}

	inline void Increment(uint32 Count)
	{
		CpuHandle.ptr += (IncrementSize * Count);
		GpuHandle.ptr += (IncrementSize * Count);
	}

	inline void Decrement()
	{
		CpuHandle.ptr -= IncrementSize;
		GpuHandle.ptr -= IncrementSize;
	}

	inline D3D12DescriptorAllocation OffsetFromStart(uint32 Count)
	{
		D3D12DescriptorAllocation Result = *this;
		Result.CpuHandle.ptr += (IncrementSize * Count);
		Result.GpuHandle.ptr += (IncrementSize * Count);
		return Result;
	}

	uint32 IncrementSize;
	D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
};

class D3D12DescriptorAllocator
{
public:

	void Initialize(ID3D12Device1* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32 Size, bool bShaderVisible = false);
	void Destroy();

	D3D12DescriptorAllocation AllocateDescriptor();

	D3D12DescriptorAllocation AllocateMultiple(uint32 Count);

	void Free(const D3D12DescriptorAllocation& Handle);

	inline ID3D12DescriptorHeap* GetDescriptorHeap()
	{
		return m_DescriptorHeap;
	}

private:

	TArray<D3D12DescriptorAllocation> m_AllocList;
	TArray<D3D12DescriptorAllocation> m_FreeList;
	ID3D12DescriptorHeap* m_DescriptorHeap;
	uint32 m_IncrementSize;
	D3D12DescriptorAllocation m_Start;
	D3D12DescriptorAllocation m_Current;
	D3D12DescriptorAllocation m_End;
};

