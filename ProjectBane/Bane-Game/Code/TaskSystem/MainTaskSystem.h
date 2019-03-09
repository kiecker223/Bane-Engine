#pragma once

#include "Task.h"
#include <Core/Containers/LinearAllocator.h>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>


template<typename T>
class ThreadSafeQueue
{
public:

	ThreadSafeQueue() { }
	using TType = T;
	
	inline void Add(TType Item)
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		Data.push(Item);
	}

	inline TType Pop()
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		if (Data.empty())
		{
			return TType();
		}
		TType Result(Data.front());
		Data.pop();
		return Result;
	}

	std::mutex Lock;
	std::queue<TType> Data;
};

struct ThreadStatus
{
public:

	bool IsExecutingTasks() const
	{
		return ExecutingTask.load();
	}

	ITaskExecutionHandle* CurrentTask;
	ThreadSafeQueue<ITaskExecutionHandle*> TaskExecutionHandles;
	std::atomic<bool> ExecutingTask;
};

class TaskSystem
{
public:

	ThreadSafeQueue<Task*> Tasks;

	void Initialize();
	void Shutdown();

	inline void WaitForThreadShutdown()
	{
		m_bRunning = false;
		for (uint32 i = 0; i < m_ThreadCount; i++)
		{
			m_Threads[i].join();
		}
	}

	void WaitForThreadStop();

	template<typename TTaskType>
	inline TTaskType* CreateSingleTask(SharedTaskResource& Resource)
	{
		TTaskType* Result = new TTaskType(Resource, TASK_EXECUTION_TYPE_SINGLE);
		return Result;
	}

	template<typename TTaskType>
	inline TTaskType* CreateParallelForTask(SharedTaskResource& Resource, uint32 DataSize)
	{
		TTaskType* Result = new TTaskType(Resource, -1, DataSize, TASK_EXECUTION_TYPE_PARALLEL_FOR);
		return Result;
	}

	void ScheduleTask(Task* pTask);
	void UpdateSchedule();
	void ThreadFunc(uint32 ThreadIdx);
	
private:

	uint32 m_ThreadCount;
	bool m_bRunning;
	std::vector<std::thread> m_Threads;
	std::vector<ThreadStatus*> m_ThreadStatuses;
};


