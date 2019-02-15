#pragma once

#include <d3d12.h>
#include <vector>
#include "Common.h"
#include "../D3DCommon/D3DCommon.h"

class D3D12DescriptorAllocation
{
public:

	D3D12DescriptorAllocation() = default;

	D3D12DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE InBaseCpuHandle, uint32 InDescriptorCount, D3D12_GPU_DESCRIPTOR_HANDLE InGpuHandle, uint32 InIncrementSize) :
		CpuHandle(InBaseCpuHandle),
		BaseCpuHandle(InBaseCpuHandle),
		DescriptorCount(InDescriptorCount),
		GpuHandle(InGpuHandle),
		IncrementSize(InIncrementSize)
	{
	}

	inline D3D12DescriptorAllocation& operator = (const D3D12DescriptorAllocation& Rhs)
	{
		DescriptorCount = Rhs.DescriptorCount;
		IncrementSize = Rhs.IncrementSize;
		BaseCpuHandle = Rhs.BaseCpuHandle;
		CpuHandle = Rhs.CpuHandle;
		GpuHandle = Rhs.GpuHandle;
		return *this;
	}

	inline void Increment()
	{
		if (CpuHandle.ptr + IncrementSize > (BaseCpuHandle.ptr + (DescriptorCount * IncrementSize)))
		{
			return;
		}
		CpuHandle.ptr += IncrementSize;
	}

	inline void Increment(uint32 Count)
	{
		if (CpuHandle.ptr + (IncrementSize * Count) > (BaseCpuHandle.ptr + static_cast<SIZE_T>(DescriptorCount * IncrementSize)))
		{
			return;
		}
		CpuHandle.ptr += (IncrementSize * Count);
	}

	inline void Decrement()
	{
		if (CpuHandle.ptr - static_cast<SIZE_T>(IncrementSize) < BaseCpuHandle.ptr)
		{
			return;
		}
		CpuHandle.ptr -= IncrementSize;
	}

	inline D3D12DescriptorAllocation OffsetFromStart(uint32 Count)
	{
		D3D12DescriptorAllocation Result = *this;
		Result.BaseCpuHandle.ptr += (IncrementSize * Count);
		Result.CpuHandle.ptr += (IncrementSize * Count);
		Result.GpuHandle.ptr += static_cast<UINT64>(IncrementSize * Count);
		return Result;
	}

	uint32						DescriptorCount;
	uint32						IncrementSize;
	D3D12_CPU_DESCRIPTOR_HANDLE BaseCpuHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
};

class ID3D12DescriptorAllocator
{
public:
	virtual void Initialize(ID3D12Device1* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32 Size, bool bShaderVisible = false) = 0;
	virtual void InitializeSubAllocator(const D3D12_CPU_DESCRIPTOR_HANDLE& InStart, uint32 InIncrementSize, const D3D12_CPU_DESCRIPTOR_HANDLE& InEnd, const D3D12_GPU_DESCRIPTOR_HANDLE& InGpuBegin) 
	{
		UNUSED(InStart);
		UNUSED(InIncrementSize);
		UNUSED(InEnd);
		UNUSED(InGpuBegin);
	}

	virtual void Reset() = 0;
	virtual void Destroy() = 0;

	virtual D3D12DescriptorAllocation AllocateDescriptor() = 0;
	virtual D3D12DescriptorAllocation AllocateMultiple(uint32 Count) = 0;

	virtual void Free(const D3D12DescriptorAllocation& Handle) = 0;

	ID3D12DescriptorHeap* DescriptorHeap;
};

class D3D12LinearDescriptorAllocator : public ID3D12DescriptorAllocator
{
public:

	void Initialize(ID3D12Device1* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32 Size, bool bShaderVisible) override final;
	void InitializeSubAllocator(const D3D12_CPU_DESCRIPTOR_HANDLE& InStart, uint32 InIncrementSize, const D3D12_CPU_DESCRIPTOR_HANDLE& InEnd, const D3D12_GPU_DESCRIPTOR_HANDLE& InGpuBegin) override final;
	void Reset() override final;
	void Destroy() override final;

	D3D12DescriptorAllocation AllocateDescriptor() override final;
	D3D12DescriptorAllocation AllocateMultiple(uint32 Count) override final;

	void Free(const D3D12DescriptorAllocation& Handle) override final;

	uint32 IncrementSize;
	D3D12DescriptorAllocation Start;
	D3D12DescriptorAllocation Current;
	D3D12DescriptorAllocation End;
};

class D3D12BlockDescriptorAllocator : public ID3D12DescriptorAllocator
{
public:

	void Initialize(ID3D12Device1* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32 Size, bool bShaderVisible = false) override final;
	void Reset() override final;
	void Destroy() override final;

	D3D12DescriptorAllocation AllocateDescriptor() override final;
	D3D12DescriptorAllocation AllocateMultiple(uint32 Count) override final;

	void Free(const D3D12DescriptorAllocation& Handle) override final;

private:

	std::vector<D3D12DescriptorAllocation> m_AllocatedBlockList;
	std::vector<D3D12DescriptorAllocation> m_FreeBlockList;
	uint32 m_IncrementSize;
	D3D12DescriptorAllocation m_Start;
	D3D12DescriptorAllocation m_Current;
	D3D12DescriptorAllocation m_End;
	uint32 m_DescriptorCount;
};

