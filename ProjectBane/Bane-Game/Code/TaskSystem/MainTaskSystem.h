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
		Data.push_back(Item);
	}

	inline void AddFront(TType Item)
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		Data.push_front(Item);
	}
	
	inline void SafeAdd(TType Item)
	{
		Data.push_back(Item);
	}

	inline uint32 GetCount()
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		return static_cast<uint32>(Data.size());
	}

	inline uint32 SafeGetCount() const
	{
		return static_cast<uint32>(Data.size());
	}

	inline TType SafePop()
	{
		if (Data.empty())
		{
			return TType();
		}
		TType Result(Data.front());
		Data.pop_front();
		return Result;
	}

	inline TType* SafeGetFront()
	{
		if (Data.empty())
		{
			return nullptr;
		}
		return &Data.front();
	}

	inline TType* GetFront()
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		return SafeGetFront();
	}

	inline void Clear()
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		SafeClear();
	}

	inline void SafeClear()
	{
		Data.clear();
	}

	inline TType Pop()
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		if (Data.empty())
		{
			return TType();
		}
		TType Result(Data.front());
		Data.pop_front();
		return Result;
	}

	std::mutex Lock;
	std::deque<TType> Data;
};

template<typename T>
class ThreadSafeVector
{
public:

	using TType = T;

	inline void Add(TType Item)
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		Data.push_back(Item);
	}

	inline TType Pop()
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		if (Data.empty())
		{
			return TType();
		}
		TType Result(Data.back());
		Data.pop_back();
		return Result;
	}

	inline void Clear()
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		Data.clear();
	}

	inline TType& SafeGetAt(const uint32 Index)
	{
		if (Data.empty() || Index >= Data.size())
		{
			return TType();
		}
		return Data[Index];
	}


	inline TType& GetAt(const uint32 Index)
	{
		std::lock_guard<std::mutex> LockGuard(Lock);
		return SafeGetAt(Index);
	}

	inline TType& operator[](const uint32 Index)
	{
		return GetAt(Index);
	}

	std::mutex Lock;
	std::vector<TType> Data;
};


#define TASK_DISPATCH_ON_ALL_THREADS -1

class TaskCommandGroup
{
public:
	ThreadSafeQueue<Task*> Commands;

	TaskCommandGroup() = default;

	TaskCommandGroup(const TaskCommandGroup& Rhs)
	{
		Commands.Data = Rhs.Commands.Data;
	}

	inline TaskCommandGroup& operator = (const TaskCommandGroup& Rhs)
	{
		Commands.Data = Rhs.Commands.Data;
		return *this;
	}
};

class TaskExecutionGroup
{
public:
	ThreadSafeQueue<TaskExecutionHandle*> Commands;

	TaskExecutionGroup() = default;

	TaskExecutionGroup(const TaskExecutionGroup& Rhs)
	{
		Commands.Data = Rhs.Commands.Data;
	}

	inline TaskExecutionGroup& operator = (const TaskExecutionGroup& Rhs)
	{
		Commands.Data = Rhs.Commands.Data;
		return *this;
	}
};


class ThreadStatus
{
public:

	ThreadStatus() : CurrentTask(nullptr), CurrentGroup(nullptr), TaskGroupFenceValue(0) { }

	static ThreadSafeQueue<TaskExecutionGroup*> WorkMemPool;

	ThreadSafeQueue<TaskExecutionGroup*> WorkQueue;
	std::atomic<TaskExecutionGroup*> CurrentGroup;
	std::atomic<TaskExecutionHandle*> CurrentTask;
	std::atomic<uint64> TaskGroupFenceValue;

	bool HasOrIsExecutingWork()
	{
		return WorkQueue.GetCount() > 0 || CurrentTask.load() != nullptr;
	}
};


class TaskSystem
{
public:

	static ThreadSafeQueue<TaskCommandGroup*> CommandMemPool;
	ThreadSafeQueue<TaskCommandGroup*> PendingCommands;

	static TaskSystem* Get();

	void Initialize();
	void Shutdown();

	inline uint32 GetThreadCount() const
	{
		return m_ThreadCount;
	}

	inline void WaitForThreadShutdown()
	{
		m_bRunning = false;
		for (uint32 i = 0; i < m_ThreadCount; i++)
		{
			m_Threads[i].join();
		}
	}

	std::vector<ThreadStatus*> ThreadStats;

	void WaitForThreadStop();

	void ScheduleTask(Task* pTask);
	void AddTaskBarrier();
	void UpdateSchedule();

	TaskExecutionHandle* StealTask(uint64 CurrentFenceValue, uint32 CallingThread);

	void ThreadFunc(uint32 ThreadIdx);

	Task* CreateTask(int32 ThreadCount, std::function<void(uint32, uint32)> Func);
	
private:

	uint32 m_ThreadCount;
	bool m_bRunning;
	std::vector<std::thread> m_Threads;
};


