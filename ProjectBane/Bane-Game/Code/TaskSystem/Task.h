#pragma once

#include <Common.h>
#include <vector>
#include <atomic>
#include <functional>
#include "TaskSystemCommand.h"



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

	Task(int32 InThreadCount, std::function<void(uint32 DispatchSize, uint32 DispatchIndex)> InFunc);

	void WaitForFinish();
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


namespace Dispatcher
{
	// Begin new list of tasks
	void Begin();

	// Dispatches a new group of tasks.
	void DispatchTasks(const std::vector<Task*>& pTasks);

	// Execute single task
	void DispatchTask(Task* pTask);

	// Adds task to an existing group
	void AddTask(Task* pTask);

	// Get the number of threads the task system is using
	uint32 GetThreadCount();

	// Flushes the current task group
	void FinishTaskGroup();

	// Stall the calling thread until the pTask is complete
	void WaitOnTask(Task* pTask);

	// Execute them
	void End();
}

