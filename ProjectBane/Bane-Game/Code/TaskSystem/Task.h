#pragma once

#include <Common.h>
#include <vector>
#include <atomic>
#include <functional>
#include "TaskSystemCommand.h"

class SharedTaskResourceHandle
{
public:

	void* Pointer;
	
	template<typename T>
	inline T* Get()
	{
		return reinterpret_cast<T*>(Pointer);
	}

	template<typename T>
	inline void Set(T* InPointer)
	{
		Pointer = reinterpret_cast<void*>(InPointer);
	}

};

typedef enum ERESOURCE_LOCK_STATE {
	RESOURCE_LOCK_STATE_UNLOCKED,
	RESOURCE_LOCK_STATE_FULL_LOCK,
	RESOURCE_LOCK_STATE_WRITE_LOCK,
	RESOURCE_LOCK_STATE_PARTIAL_WRITE_LOCK,
	RESOURCE_LOCK_STATE_READ_LOCK
} ERESOURCE_LOCK_STATE;

typedef enum ETASK_EXECUTION_TYPE {
	TASK_EXECUTION_TYPE_INVALID = 0,
	TASK_EXECUTION_TYPE_PARALLEL_FOR = 1,
	TASK_EXECUTION_TYPE_SINGLE,
} ETASK_EXECUTION_TYPE;

class SharedTaskResource
{
public:

	struct ProtectionRange
	{
		uint32 StartIndex, EndIndex;

		inline bool Overlap(const ProtectionRange& Rhs)
		{
			return Rhs.StartIndex <= EndIndex || Rhs.EndIndex >= StartIndex;
		}
	};

	SharedTaskResourceHandle ResourceHandle;
	ERESOURCE_LOCK_STATE CurrentState;
	ERESOURCE_LOCK_STATE PendingState;

// 	bool BeginRead(uint32 StartIndex, uint32 EndIndex, bool bLockTillReady = false);
// 	bool EndRead(uint32 StartIndex, uint32 EndIndex, bool bLockTillReady = false);
// 	bool BeginWrite(uint32 StartIndex, uint32 EndIndex, bool bLockTillReady = false);
// 	bool EndWrite(uint32 StartIndex, uint32 EndIndex, bool bLockTillReady = false);
// 
// 	bool BeginRead(bool bLockTillReady = false);
// 	bool EndRead(bool bLockTillReady = false);
// 
// 	bool BeginWrite(bool bLockTillReady = false);
// 	bool EndWrite(bool bLockTillReady = false);
// 
// 	bool AddWriteProtection(uint32 StartIndex, uint32 EndIndex, bool bLockTillReady = false);
// 	bool AddReadProtection(uint32 StartIndex, uint32 EndIndex, bool bLockTillReady = false);
// 	bool WriteProtect(bool bLockTillReady = false);
// 	bool ReadProtect(bool bLockTillReady = false);
// 
// 	std::vector<ProtectionRange> WriteProtectedRanges;
// 	std::vector<ProtectionRange> ReadProtectedRanges;
};


class TaskSegmentExecutor : public TaskExecutionHandle
{
public:

	TaskSegmentExecutor()
	{
		pOwningTask = nullptr;
		NextValue.store(0);
		CompletedValue.store(0);
	}

	std::function<void(uint32 /*DispatchSize*/, uint32 /*DispatchIndex*/)> Func;

	void InternalExecute() override;
};

class Task
{
public:

	Task() : 
		NextValue(0),
		CurrentValue(0),
		m_ThreadCount(0)
	{
	}

	void WaitForFinish();
	void Setup();

	void Dispatch();

	bool AllHandlesFinished();
	void SetupForNextDispatch();

	// ThreadCount = -1 means that this task will be distributed among all threads
	void SetNumThreads(int32 ThreadCount);
	void SetTaskFunction(std::function<void(uint32 DispatchSize, uint32 DispatchIndex)> InFunc);
	uint32 GetDispatchCount() const;

	inline TaskSegmentExecutor* GetTaskExecutionHandle(uint32 Index) const
	{
		return m_OwnedHandles[Index];
	}

	std::atomic<uint64> NextValue;
	std::atomic<uint64> CurrentValue;

	inline bool IsCompleted()
	{
		return AllHandlesFinished();
		//return NextValue.load() == CurrentValue.load();
	}

	inline bool HasDispatched()
	{
		return NextValue.load() > CurrentValue.load();
	}

protected:

	std::function<void(uint32, uint32)> m_DispatchFunc;

	std::vector<Task*> m_Dependencies;
	int32 m_ThreadCount;
	std::vector<TaskSegmentExecutor*> m_OwnedHandles;
};


