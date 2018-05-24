#pragma once

#include "Core/Common.h"
#include "D3D12Helper.h"
#include "D3D12CommandContext.h"
#include "Core/Containers/StackQueue.h"



class D3D12CommandQueue
{
public:

	typedef StackQueue<D3D12CommandList*, 16> Queue;

	D3D12CommandQueue(ID3D12CommandQueue* CommandQueue, ID3D12Device1* Device, const std::string& QueueName, ECOMMAND_CONTEXT_TYPE InContextType) :
		m_ContextType(InContextType),
		m_CommandQueue(CommandQueue)
	{
		D3D12ERRORCHECK(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_CompletionEvent = CreateEventExA(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		assert(GetLastError() == 0);
	}
	
	D3D12CommandQueue(D3D12_COMMAND_QUEUE_DESC CreationDesc, ID3D12Device1* Device, const std::string& QueueName, ECOMMAND_CONTEXT_TYPE InContextType) :
		m_ContextType(InContextType)
	{
		D3D12ERRORCHECK(Device->CreateCommandQueue(&CreationDesc, IID_PPV_ARGS(&m_CommandQueue)));
		D3D12ERRORCHECK(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_CompletionEvent = CreateEventExA(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		assert(GetLastError() == 0);
	}

	~D3D12CommandQueue()
	{
		m_Fence->Release();
		m_CommandQueue->Release();
		CloseHandle(m_CompletionEvent);
	}

	inline void ScheduleCommandList(D3D12CommandList* InList)
	{
		m_WaitQueue.Push(InList);
	}
	
	inline void ExecuteCommandLists(bool bWaitForFinish = false)
	{
		ID3D12CommandList* CommandLists[16];
		for (uint i = 0; i < m_WaitQueue.GetNumElements(); i++)
		{
			CommandLists[i] = (ID3D12CommandList*)m_WaitQueue[i]->CommandList;
		}
		InternalExecuteCommandLists(CommandLists, m_WaitQueue.GetNumElements());
		m_ExecutionQueue.CopyFrom(m_WaitQueue);
		m_WaitQueue.Reset();

		if (bWaitForFinish)
		{
			StallForFinish();
		}
	}

	inline void ExecuteImmediate(D3D12CommandList* InList, bool bWaitForFinish = false)
	{
		m_ExecutionQueue.Push(InList);
		InternalExecuteCommandList(InList->CommandList);
		if (InList->bNeedsWaitForComputeQueue)
		{
			InList->bNeedsWaitForComputeQueue = false;
			InsertWaitForComputeQueue();
		}
		else if (InList->bNeedsWaitForGraphicsQueue)
		{
			InList->bNeedsWaitForGraphicsQueue = false;
			InsertWaitForGraphicsQueue();
		}
		if (bWaitForFinish)
		{
			StallForFinish();
		}
	}

	inline void StallForFinish()
	{
		if (!IsFinished())
		{
			m_Fence->SetEventOnCompletion(m_FenceValue, m_CompletionEvent);
			DWORD Error = GetLastError();
			WaitForSingleObjectEx(m_CompletionEvent, INFINITE, FALSE);
		}
		CompleteExecution();
	}

	inline bool IsFinished()
	{
		const uint64 CurrentFenceValue = m_Fence->GetCompletedValue();
		return CurrentFenceValue >= m_FenceValue;
	}

	ForceInline Queue& GetWaitQueue()
	{
		return m_WaitQueue;
	}

	ForceInline Queue& GetExecutionQueue()
	{
		return m_ExecutionQueue;
	}

	// This clears the completed queue
	ForceInline Queue GetCompletedQueue()
	{
		Queue Result(m_CompletedQueue);
		m_CompletedQueue.Reset();
		return Result;
	}

	ForceInline ID3D12CommandQueue* GetD3DCommandQueue()
	{ 
		return m_CommandQueue;
	}

	ForceInline ID3D12Fence* GetD3DFence()
	{
		return m_Fence;
	}

	ForceInline uint64 GetNextFenceValue()
	{
		return m_FenceValue;
	}

	void CompleteExecution();
	void InsertWaitForComputeQueue();
	void InsertWaitForGraphicsQueue();

	inline void SetParentDevice(D3D12GraphicsDevice* GraphicsDevice)
	{
		m_GraphicsDevice = GraphicsDevice;
	}

private:

	inline void InternalExecuteCommandList(ID3D12CommandList* CommandList)
	{
		InternalExecuteCommandLists(&CommandList, 1);
	}

	inline void InternalExecuteCommandLists(ID3D12CommandList** CommandLists, uint NumCommandLists)
	{
		m_CommandQueue->ExecuteCommandLists(NumCommandLists, CommandLists);
		m_FenceValue++;
		m_CommandQueue->Signal(m_Fence, m_FenceValue);
	}

	D3D12GraphicsDevice* m_GraphicsDevice;
	HANDLE m_CompletionEvent;
	uint64 m_FenceValue;
	ID3D12Fence* m_Fence;
	Queue m_WaitQueue;
	Queue m_ExecutionQueue;
	Queue m_CompletedQueue;
	ID3D12CommandQueue* m_CommandQueue;
	ECOMMAND_CONTEXT_TYPE m_ContextType;
};