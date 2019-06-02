#include "MainTaskSystem.h"
#include "Application/Application.h"
#include <Windows.h>
#include <iostream>

ThreadSafeQueue<TaskExecutionGroup*> ThreadStatus::WorkMemPool;
ThreadSafeQueue<TaskCommandGroup*> TaskSystem::CommandMemPool;

TaskSystem* TaskSystem::Get()
{
	return GetApplicationInstance()->GetTaskSystem();
}

void TaskSystem::Initialize()
{
	uint32 ThreadCount = 4;//std::thread::hardware_concurrency() - 2;
	m_bRunning = true;
	m_Threads.reserve(ThreadCount);
	ThreadStats.resize(ThreadCount);
	m_ThreadCount = ThreadCount;

	for (uint32 i = 0; i < 128; i++)
	{
		for (uint32 x = 0; x < m_ThreadCount; x++)
		{
			ThreadStatus::WorkMemPool.SafeAdd(new TaskExecutionGroup());
		}
		TaskSystem::CommandMemPool.SafeAdd(new TaskCommandGroup());
	}

	for (uint32 i = 0; i < ThreadStats.size(); i++)
	{
		ThreadStats[i] = new ThreadStatus();
	}
	for (uint32 i = 0; i < ThreadCount; i++)
	{
		m_Threads.push_back(std::move(
			std::thread([&]() {
				ThreadFunc(i);
			})
		));
	}
}

void TaskSystem::Shutdown()
{
	m_bRunning = false;
	WaitForThreadShutdown();
}

void TaskSystem::WaitForThreadStop()
{
	while (true)
	{
		bool bStop = false;
		for (uint32 i = 0; i < ThreadStats.size(); i++)
		{
			ThreadStatus* pStat = ThreadStats[i];
			if (!pStat->HasOrIsExecutingWork())
			{
				bStop = true;
			}
			else
			{
				bStop = false;
				break;
			}
		}
		if (!bStop)
		{
			Sleep(0);
		}
		else
		{
			return;
		}
	}
}

void TaskSystem::ScheduleTask(Task* pTask)
{
	// Own the lock
	std::lock_guard<std::mutex> LockGuard(PendingCommands.Lock);
	if (PendingCommands.Data.empty())
	{
		PendingCommands.SafeAdd(TaskSystem::CommandMemPool.Pop()); // New TaskCommandGroup;
	}
	(*PendingCommands.SafeGetFront())->Commands.Add(pTask);
}

void TaskSystem::AddTaskBarrier()
{
	PendingCommands.Add(TaskSystem::CommandMemPool.Pop());
}

void TaskSystem::UpdateSchedule()
{
	TaskCommandGroup* NewTaskGroup = PendingCommands.Pop();
	if (NewTaskGroup)
	{
		while (NewTaskGroup)
		{
			if (NewTaskGroup->Commands.GetCount() > 0)
			{
				for (uint32 i = 0; i < m_ThreadCount; i++)
				{
					ThreadStats[i]->WorkQueue.Lock.lock();
					ThreadStats[i]->WorkQueue.SafeAdd(ThreadStatus::WorkMemPool.Pop());
				}
				uint32 CurrentDispatchThreadId = 0;
				std::lock_guard<std::mutex> LockGuard(NewTaskGroup->Commands.Lock);
				Task* pTask = NewTaskGroup->Commands.SafePop();
				while (pTask)
				{
					for (uint32 i = 0; i < pTask->GetDispatchCount(); i++)
					{
						(*ThreadStats[CurrentDispatchThreadId % (m_ThreadCount)]->WorkQueue.SafeGetFront())->Commands.Add(pTask->GetTaskExecutionHandle(i));
						CurrentDispatchThreadId++;
					}
					pTask = NewTaskGroup->Commands.SafePop();
				}
				for (uint32 i = 0; i < m_ThreadCount; i++)
				{
					ThreadStats[i]->WorkQueue.Lock.unlock();
				}
			}
			NewTaskGroup->Commands.Clear();
			TaskSystem::CommandMemPool.Add(NewTaskGroup);
			NewTaskGroup = PendingCommands.Pop();
		}
	}
}

TaskExecutionHandle* TaskSystem::StealTask(uint64 CurrentFenceValue, uint32 CallingThread)
{
	for (uint32 i = 0; i < m_ThreadCount; i++)
	{
		if (i == CallingThread)
		{
			continue;
		}
		if (ThreadStats[i]->TaskGroupFenceValue.load() == CurrentFenceValue)
		{
			auto* Group = ThreadStats[i]->CurrentGroup.load();
			if (Group)
			{
				auto* Result = Group->Commands.Pop();
				if (Result)
				{
					return Result;
				}
			}
		}
	}
	return nullptr;
}

void TaskSystem::ThreadFunc(uint32 ThreadIdx)
{
	// Wait for the stack??
	// This is a really weird hack I have to setup but if I don't do this then values are 
	// seemingly corrupted
	while (true)
	{
		if (ThreadIdx < m_Threads.size())
		{
			break;
		}
	}
	if (!SetThreadAffinityMask(GetCurrentThread(), (uint64(1) << uint64(ThreadIdx + 2))))
	{
		__debugbreak();
	}
	
	ThreadStatus* Stats = ThreadStats[ThreadIdx];
	while (m_bRunning)
	{
		{
			TaskExecutionGroup* ExecutionGroup = Stats->WorkQueue.Pop();
			if (ExecutionGroup)
			{
				Stats->CurrentGroup.store(ExecutionGroup);
				TaskExecutionHandle* Handle = ExecutionGroup->Commands.Pop();
				if (Handle)
				{
					while (Handle)
					{
						Stats->CurrentTask.store(Handle);
						Handle->InternalExecute();
						Handle = ExecutionGroup->Commands.Pop();
					}
					// Try to steal some work
					Stats->CurrentGroup.store(nullptr);
					Handle = StealTask(Stats->TaskGroupFenceValue.load(), ThreadIdx);
					if (Handle)
					{
						while (Handle)
						{
							Stats->CurrentTask.store(Handle);
							Handle->InternalExecute();
							Handle = StealTask(Stats->TaskGroupFenceValue.load(), ThreadIdx);
						}
					}
				}
				else
				{
					// Duplicate code is bad!!! But we need to keep the threads busy
					Stats->CurrentGroup.store(nullptr);
					Handle = StealTask(Stats->TaskGroupFenceValue.load(), ThreadIdx);
					if (Handle)
					{
						while (Handle)
						{
							Stats->CurrentTask.store(Handle);
							Handle->InternalExecute();
							Handle = StealTask(Stats->TaskGroupFenceValue.load(), ThreadIdx);
						}
					}
				}
				ThreadStatus::WorkMemPool.Add(ExecutionGroup);
				Stats->CurrentTask.store(nullptr);
				Stats->TaskGroupFenceValue++;
			}
			else
			{
				continue;
			}
		}
		while (true)
		{
			bool bFinished = true;
			for (uint32 i = 0; i < ThreadStats.size(); i++)
			{
				ThreadStatus* tStats = ThreadStats[i];
				if (tStats->TaskGroupFenceValue.load() < Stats->TaskGroupFenceValue.load())
				{
					bFinished = false;
				}
			}
			if (!bFinished)
			{
				std::this_thread::yield();
			}
			else
			{
				break;
			}
		}
	}
}

Task* TaskSystem::CreateTask(int32 ThreadCount, std::function<void(uint32, uint32)> Func)
{
	Task* Result = new Task();
	Result->SetNumThreads(ThreadCount);
	Result->SetTaskFunction(Func);
	return Result;
}
