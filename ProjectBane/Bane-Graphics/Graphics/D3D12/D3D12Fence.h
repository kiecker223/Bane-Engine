#pragma once

#include "Core/Common.h"
#include <d3d12.h>


class D3D12Fence
{
public:

	inline D3D12Fence(ID3D12Device1* Device)
	{
		Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
	}

	~D3D12Fence()
	{
		Fence->Release();
	}

	uint64 Value;
	ID3D12Fence* Fence;
};
