#include "MainTaskSystem.h"
#include <Windows.h>




void TaskSystem::Initialize()
{
	uint32 ThreadCount = std::thread::hardware_concurrency() - 2;
	m_bRunning = true;
	m_Threads.reserve(ThreadCount);
	m_ThreadStatuses.resize(ThreadCount);
	for (uint32 i = 0; i < m_ThreadStatuses.size(); i++)
	{
		m_ThreadStatuses[i] = new ThreadStatus();
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
		for (uint32 i = 0; i < m_ThreadStatuses.size(); i++)
		{
			ThreadStatus* pStat = m_ThreadStatuses[i];
			if (!pStat->IsExecutingTasks())
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
			std::this_thread::sleep_for(std::chrono::nanoseconds(100));
		}
		else
		{
			return;
		}
	}
}

void TaskSystem::ScheduleTask(Task* pTask)
{
	Tasks.Add(pTask);
}

void TaskSystem::UpdateSchedule()
{
	Task* pTask = Tasks.Pop();

	if (pTask)
	{
		while (pTask)
		{
			if (pTask->ExecutionType == TASK_EXECUTION_TYPE_SINGLE)
			{
				for (uint32 i = 0; i < m_ThreadStatuses.size(); i++)
				{
					if (!m_ThreadStatuses[i]->IsExecutingTasks())
					{
						m_ThreadStatuses[i]->TaskExecutionHandles.Add(pTask->CreateTaskExecutionHandle());
					}
				}
			}
			if (pTask->ExecutionType == TASK_EXECUTION_TYPE_PARALLEL_FOR)
			{
				if (pTask->ThreadCount == -1)
				{
					pTask->ThreadDispatchCount = static_cast<uint32>(m_Threads.size());
				}
				else
				{
					pTask->ThreadDispatchCount = pTask->ThreadCount;
				}
				for (uint32 i = 0; i < pTask->ThreadDispatchCount; i++)
				{
					ThreadStatus* Status = m_ThreadStatuses[i];
					Status->TaskExecutionHandles.Add(pTask->CreateTaskExecutionHandle(i));
				}
			}
			pTask = Tasks.Pop();
		}
	}
}

void TaskSystem::ThreadFunc(uint32 ThreadIdx)
{
	if (!SetThreadAffinityMask(GetCurrentThread(), (uint64(1) << uint64(ThreadIdx + 2))))
	{
		__debugbreak();
	}

	while (m_bRunning)
	{
		if (m_ThreadStatuses.size() > 0)
		{
			if (ThreadIdx + 1 > m_Threads.size()) { continue; }
			ThreadStatus* ThreadStats = m_ThreadStatuses[ThreadIdx];
			if (ThreadStats)
			{
				ITaskExecutionHandle* TaskExecution = ThreadStats->TaskExecutionHandles.Pop();

				if (TaskExecution)
				{
					ThreadStats->ExecutingTask.store(true);
					TaskExecution->Execute();
					ThreadStats->ExecutingTask.store(false);
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::nanoseconds(100));
				}
			}
		}
	}
}