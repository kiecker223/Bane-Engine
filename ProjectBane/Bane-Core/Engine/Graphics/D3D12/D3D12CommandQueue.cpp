#include "D3D12CommandQueue.h"
#include "D3D12GraphicsDevice.h"

void D3D12CommandQueue::CompleteExecution()
{
	assert(IsFinished());

	for (uint i = 0; i < m_ExecutionQueue.GetNumElements(); i++)
	{
		if (!m_ExecutionQueue[i]->UploadResourcesToDestroy.empty())
		{
			m_ExecutionQueue[i]->FlushDestructionQueue();
			m_ExecutionQueue[i]->FlushCommitQueue();
		}
		m_ExecutionQueue[i]->Reset();
	}

	// Push all the finished command lists to the device and reset
	m_ExecutionQueue.AppendToOther(m_GraphicsDevice->m_AvailableCLs[m_ContextType]);
	m_ExecutionQueue.Reset();
}

void D3D12CommandQueue::InsertWaitForGraphicsQueue()
{
	D3D12CommandQueue& CmdQueue = m_GraphicsDevice->GetMainCommandQueue();
	CmdQueue.GetD3DCommandQueue()->Wait(CmdQueue.GetD3DFence(), CmdQueue.GetNextFenceValue() - 1);
}

void D3D12CommandQueue::InsertWaitForComputeQueue()
{
	D3D12CommandQueue& CmdQueue = m_GraphicsDevice->GetComputeQueue();
	CmdQueue.GetD3DCommandQueue()->Wait(CmdQueue.GetD3DFence(), CmdQueue.GetNextFenceValue() - 1);
}
