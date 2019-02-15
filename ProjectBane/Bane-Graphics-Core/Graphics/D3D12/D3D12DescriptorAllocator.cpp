#include "D3D12DescriptorAllocator.h"

void D3D12LinearDescriptorAllocator::Initialize(ID3D12Device1* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32 Size, bool bShaderVisible)
{
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = { };
	HeapDesc.Type = HeapType;
	HeapDesc.NumDescriptors = Size;
	HeapDesc.NodeMask = 1;
	HeapDesc.Flags = bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	D3D12ERRORCHECK(
		Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&DescriptorHeap))
	);

	IncrementSize = Device->GetDescriptorHandleIncrementSize(HeapType);

	Start = D3D12DescriptorAllocation(DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), Size,
									  DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), IncrementSize);
	Current = Start;
	End = Start.OffsetFromStart(Size);
}

void D3D12LinearDescriptorAllocator::InitializeSubAllocator(const D3D12_CPU_DESCRIPTOR_HANDLE& InStart, uint32 InIncrementSize, const D3D12_CPU_DESCRIPTOR_HANDLE& InEnd, const D3D12_GPU_DESCRIPTOR_HANDLE& InGpuBegin)
{
	IncrementSize = InIncrementSize;
	uint32 DescriptorCount = static_cast<uint32>(InEnd.ptr - InStart.ptr) / IncrementSize;
	Start = D3D12DescriptorAllocation(InStart, DescriptorCount, InGpuBegin, IncrementSize);
	End = Start.OffsetFromStart(DescriptorCount);
	Current = Start;
}

void D3D12LinearDescriptorAllocator::Reset()
{
	Current = Start;
}

void D3D12LinearDescriptorAllocator::Destroy()
{
	DescriptorHeap->Release();
}

D3D12DescriptorAllocation D3D12LinearDescriptorAllocator::AllocateDescriptor()
{
	return AllocateMultiple(1);
}

D3D12DescriptorAllocation D3D12LinearDescriptorAllocator::AllocateMultiple(uint32 Count)
{
	D3D12DescriptorAllocation Result = Current;
	Current = Current.OffsetFromStart(Count);
	if (Current.BaseCpuHandle.ptr > End.BaseCpuHandle.ptr)
	{
		// Out of descriptor memory
		__debugbreak();
		return D3D12DescriptorAllocation();
	}
	return Result;
}

void D3D12LinearDescriptorAllocator::Free(const D3D12DescriptorAllocation& Handle)
{
	UNUSED(Handle);
}

void D3D12BlockDescriptorAllocator::Initialize(ID3D12Device1* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32 Size, bool bShaderVisible)
{
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = { };
	HeapDesc.Type = HeapType;
	HeapDesc.NumDescriptors = Size;
	HeapDesc.NodeMask = 1;
	HeapDesc.Flags = bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_DescriptorCount = Size;
	D3D12ERRORCHECK(
		Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&DescriptorHeap))
	);

	m_IncrementSize = Device->GetDescriptorHandleIncrementSize(HeapType);

	m_AllocatedBlockList.reserve(128);
	m_FreeBlockList.reserve(128);

	m_Start = D3D12DescriptorAllocation(DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), Size,
										DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_IncrementSize);
	m_Current = m_Start;
	m_End = m_Start.OffsetFromStart(Size);
	m_FreeBlockList.push_back(m_Start);
}

void D3D12BlockDescriptorAllocator::Reset()
{
	m_AllocatedBlockList.clear();
	m_AllocatedBlockList.reserve(128);
	m_FreeBlockList.clear();
	m_FreeBlockList.reserve(128);
	m_Start = D3D12DescriptorAllocation(DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_DescriptorCount,
		DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_IncrementSize);

	m_FreeBlockList.push_back(m_Start);
}

void D3D12BlockDescriptorAllocator::Destroy()
{
	DescriptorHeap->Release();
}

D3D12DescriptorAllocation D3D12BlockDescriptorAllocator::AllocateDescriptor()
{
	return AllocateMultiple(1);
}

D3D12DescriptorAllocation D3D12BlockDescriptorAllocator::AllocateMultiple(uint32 Count)
{
	for (uint32 i = 0; i < m_FreeBlockList.size(); i++)
	{
		D3D12DescriptorAllocation& BlockTest = m_FreeBlockList[i];
		bool bViableBlock = false;
		if (BlockTest.DescriptorCount >= Count)
		{
			bViableBlock = true;
		}
		if (bViableBlock)
		{
			auto CurrentBlock = BlockTest;
			if (BlockTest.DescriptorCount > Count)
			{
				auto NewBlockTest = BlockTest.OffsetFromStart(Count);
				CurrentBlock.DescriptorCount = Count;
				m_AllocatedBlockList.push_back(CurrentBlock);
			}
		}
	}
	// Out of descriptor memory
	__debugbreak();
	return D3D12DescriptorAllocation();
}

void D3D12BlockDescriptorAllocator::Free(const D3D12DescriptorAllocation& Handle)
{
	m_FreeBlockList.push_back(Handle);
}
