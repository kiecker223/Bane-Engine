#pragma once

#include "Common.h"
#include "../D3DCommon/D3DCommon.h"
#include "D3D12Helper.h"
#include "../D3DCommon/D3DCommon.h"
#include "D3D12CommandContext.h"
#include "Core/Containers/StackQueue.h"



class D3D12CommandQueue
{
	friend class D3D12CommandQueue;

public:

	typedef StackQueue<D3D12CommandList*, 16> Queue;

	D3D12CommandQueue() :
		m_FenceValue(0)
	{
	}

	D3D12CommandQueue(ID3D12CommandQueue* CommandQueue, ID3D12Device1* Device, const std::string& QueueName, ECOMMAND_CONTEXT_TYPE InContextType) :
		m_FenceValue(0)
	{
		Initialize(CommandQueue, Device, QueueName, InContextType);
	}
	
	D3D12CommandQueue(D3D12_COMMAND_QUEUE_DESC CreationDesc, ID3D12Device1* Device, const std::string& QueueName, ECOMMAND_CONTEXT_TYPE InContextType) :
		m_FenceValue(0)
	{
		Initialize(CreationDesc, Device, QueueName, InContextType);
	}

	inline void Initialize(ID3D12CommandQueue* CommandQueue, ID3D12Device1* Device, const std::string& QueueName, ECOMMAND_CONTEXT_TYPE InContextType)
	{
		m_ContextType = InContextType;
		m_CommandQueue = CommandQueue;
		D3D12ERRORCHECK(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_CompletionEvent = CreateEventExA(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		m_CommandQueue->SetName(std::wstring(QueueName.begin(), QueueName.end()).c_str());
		BANE_CHECK(GetLastError() == 0);
	}

	inline void Initialize(D3D12_COMMAND_QUEUE_DESC CreationDesc, ID3D12Device1* Device, const std::string& QueueName, ECOMMAND_CONTEXT_TYPE InContextType)
	{
		m_ContextType = InContextType;
		D3D12ERRORCHECK(Device->CreateCommandQueue(&CreationDesc, IID_PPV_ARGS(&m_CommandQueue)));
		D3D12ERRORCHECK(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_CommandQueue->SetName(std::wstring(QueueName.begin(), QueueName.end()).c_str());
		m_CompletionEvent = CreateEventExA(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		BANE_CHECK(GetLastError() == 0);
	}

	D3D12CommandQueue& operator = (const D3D12CommandQueue& Rhs)
	{
		m_GraphicsDevice = Rhs.m_GraphicsDevice;
		m_CompletionEvent = Rhs.m_CompletionEvent;
		m_FenceValue	 = Rhs.m_FenceValue;
		m_Fence			 = Rhs.m_Fence;
		m_WaitQueue		 = Rhs.m_WaitQueue;
		m_ExecutionQueue = Rhs.m_ExecutionQueue;
		m_CompletedQueue = Rhs.m_CompletedQueue;
		m_CommandQueue	 = Rhs.m_CommandQueue;
		m_ContextType	 = Rhs.m_ContextType;
		return *this;
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
		for (uint32 i = 0; i < m_WaitQueue.GetNumElements(); i++)
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
			WaitForSingleObjectEx(m_CompletionEvent, INFINITE, FALSE);
		}
		CompleteExecution();
	}

	inline bool IsFinished()
	{
		const uint64 CurrentFenceValue = m_Fence->GetCompletedValue();
		return CurrentFenceValue >= m_FenceValue;
	}

	inline Queue& GetWaitQueue()
	{
		return m_WaitQueue;
	}

	inline Queue& GetExecutionQueue()
	{
		return m_ExecutionQueue;
	}

	// This clears the completed queue
	inline Queue GetCompletedQueue()
	{
		Queue Result(m_CompletedQueue);
		m_CompletedQueue.Reset();
		return Result;
	}

	inline ID3D12CommandQueue* GetD3DCommandQueue()
	{ 
		return m_CommandQueue;
	}

	inline ID3D12Fence* GetD3DFence()
	{
		return m_Fence;
	}

	inline uint64 GetNextFenceValue()
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

	inline void InternalExecuteCommandLists(ID3D12CommandList** CommandLists, uint32 NumCommandLists)
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