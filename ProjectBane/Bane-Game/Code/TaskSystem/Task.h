#pragma once

#include <Common.h>
#include <vector>

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


class ITaskExecutionHandle
{
public:
	
	virtual ~ITaskExecutionHandle() { }

	virtual bool IsFinished() const = 0;
	virtual void Execute() = 0;
};

class Task
{
public:
	
	Task(SharedTaskResource& InDependentResource, int32 InThreadCount, uint32 InSizeOfForeach, ETASK_EXECUTION_TYPE InExecutionType) :
		DependentResource(InDependentResource),
		ThreadCount(InThreadCount),
		DataIterationSize(InSizeOfForeach),
		ExecutionType(InExecutionType)
	{
		BANE_CHECK(ExecutionType == TASK_EXECUTION_TYPE_PARALLEL_FOR);
	}
	Task(SharedTaskResource& InDependentResource, ETASK_EXECUTION_TYPE InExecutionType) :
		DependentResource(InDependentResource),
		ExecutionType(InExecutionType)
	{
		BANE_CHECK(ExecutionType == TASK_EXECUTION_TYPE_SINGLE);
	}

	inline void BlockTillFinished()
	{
		bool bFinished = false;
		while (true)
		{
			bFinished = IsFinished();
			if (bFinished)
			{
				break;
			}
		}
	}

	virtual ITaskExecutionHandle* CreateTaskExecutionHandle() { return nullptr; }
	virtual ITaskExecutionHandle* CreateTaskExecutionHandle(uint32 WorkerThreadIndex) { UNUSED(WorkerThreadIndex); return nullptr; }
	virtual bool IsFinished() const = 0;

	SharedTaskResource& DependentResource;
	int32 ThreadCount;
	uint32 ThreadDispatchCount;
	uint32 DataIterationSize;
	ETASK_EXECUTION_TYPE ExecutionType;

protected:

	std::vector<ITaskExecutionHandle*> m_TaskExecutionHandles;
	bool m_bIsFinished;
};


