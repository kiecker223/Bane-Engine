#include "D3D12CommandQueue.h"
#include "D3D12GraphicsDevice.h"

void D3D12CommandQueue::CompleteExecution()
{
	BANE_CHECK(IsFinished());

	for (uint32 i = 0; i < m_ExecutionQueue.GetNumElements(); i++)
	{
		if (!m_ExecutionQueue[i]->UploadResourcesToDestroy.empty())
		{
			if (m_ExecutionQueue[i]->DeletionQueueLock.try_lock())
			{
				m_ExecutionQueue[i]->FlushDestructionQueue();
				m_ExecutionQueue[i]->DeletionQueueLock.unlock();
			}
			if (m_ExecutionQueue[i]->CommitQueueLock.try_lock())
			{
				m_ExecutionQueue[i]->FlushCommitQueue();
				m_ExecutionQueue[i]->CommitQueueLock.unlock();
			}
		}
		if (m_ExecutionQueue[i]->ResetLock.try_lock())
		{
			m_ExecutionQueue[i]->Reset();
			m_ExecutionQueue[i]->ResetLock.unlock();
		}
	}

	// Push all the finished command lists to the device and reset
	if (m_ExecutionQueue.NumUsed > 0)
	{
		m_ExecutionQueue.AppendToOther(m_GraphicsDevice->m_AvailableCLs[m_ContextType]);
		m_ExecutionQueue.Reset();
	}
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
