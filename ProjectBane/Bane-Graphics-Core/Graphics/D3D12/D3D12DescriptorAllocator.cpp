#include "D3D12DescriptorAllocator.h"

void D3D12DescriptorAllocator::Initialize(ID3D12Device1* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32 Size, bool bShaderVisible)
{
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = { };
	HeapDesc.Type = HeapType;
	HeapDesc.NumDescriptors = Size;
	HeapDesc.NodeMask = 1;
	HeapDesc.Flags = bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	D3D12ERRORCHECK(
		Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&m_DescriptorHeap))
	);

	m_IncrementSize = Device->GetDescriptorHandleIncrementSize(HeapType);

	m_Start = { m_IncrementSize, m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart() };
	m_Current = m_Start;
	m_End = m_Start.OffsetFromStart(Size);
}

void D3D12DescriptorAllocator::Destroy()
{
	m_DescriptorHeap->Release();
}

D3D12DescriptorAllocation D3D12DescriptorAllocator::AllocateDescriptor()
{
	// At the moment there is no need to keep track of free and used resources, as all temp rts are just pooled for later use
	D3D12DescriptorAllocation Current = m_Current;
	m_Current.Increment();
	return Current;
}

D3D12DescriptorAllocation D3D12DescriptorAllocator::AllocateMultiple(uint32 Count)
{
	D3D12DescriptorAllocation Current = m_Current;
	m_Current.Increment(Count);
	return Current;
}

void D3D12DescriptorAllocator::Free(const D3D12DescriptorAllocation& Handle)
{
	UNREFERENCED_PARAMETER(Handle);
}
