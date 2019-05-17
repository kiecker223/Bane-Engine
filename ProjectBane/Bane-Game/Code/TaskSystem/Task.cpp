#include "MainTaskSystem.h"
#include <Windows.h>
#include <iostream>
#include "Task.h"

void TaskSegmentExecutor::InternalExecute()
{
	Func(DispatchSize, DispatchIndex);
	CompletedValue++;
	if (pOwningTask->AllHandlesFinished())
	{
		pOwningTask->SetupForNextDispatch();
	}
}

Task::Task(int32 InThreadCount, std::function<void(uint32 DispatchSize, uint32 DispatchIndex)> InFunc)
{
	SetNumThreads(InThreadCount);
	SetTaskFunction(InFunc);
}

void Task::WaitForFinish()
{
	while (true)
	{
		if (IsCompleted())
		{
			return;
		}
		else
		{
			Sleep(0);
		}
	}
}

void Task::Dispatch()
{
	TaskSystem* System = TaskSystem::Get();
	NextValue++;

	if (m_OwnedHandles.empty())
	{
		uint32 NumHandles = m_ThreadCount == -1 ? System->GetThreadCount() : m_ThreadCount;
		m_OwnedHandles.resize(NumHandles);
		
		for (uint32 i = 0; i < NumHandles; i++)
		{
			m_OwnedHandles[i] = new TaskSegmentExecutor();
			m_OwnedHandles[i]->Func = m_DispatchFunc;
			m_OwnedHandles[i]->pOwningTask = this;
			m_OwnedHandles[i]->DispatchIndex = i;
			m_OwnedHandles[i]->DispatchSize = NumHandles;
		}
	}

	for (uint32 i = 0; i < m_OwnedHandles.size(); i++)
	{
		m_OwnedHandles[i]->NextValue++;
	}

	System->ScheduleTask(this);
}

bool Task::AllHandlesFinished()
{
	bool bFinished = true;
	for (uint32 i = 0; i < m_OwnedHandles.size(); i++)
	{
		if (!m_OwnedHandles[i]->IsCompleted())
		{
			bFinished = false;
		}
	}
	return bFinished;
}

void Task::SetupForNextDispatch()
{
	// Avoid the race condition here
	CurrentValue.store(NextValue.load());
}

void Task::SetNumThreads(int32 InThreadCount)
{
	m_ThreadCount = InThreadCount;
}

void Task::SetTaskFunction(std::function<void(uint32 DispatchSize, uint32 DispatchIndex)> InFunc)
{
	m_DispatchFunc = InFunc;
}

uint32 Task::GetDispatchCount() const
{
	uint32 Result = m_ThreadCount == -1 ? TaskSystem::Get()->GetThreadCount() : static_cast<uint32>(m_ThreadCount);
	BANE_CHECK(Result < 65);
	return Result;
}

void Dispatcher::Begin()
{
	// Nothing to do for now
}

void Dispatcher::DispatchTasks(const std::vector<Task*>& pTasks)
{
	for (auto* pTask : pTasks)
	{
		pTask->Dispatch();
	}
	TaskSystem::Get()->AddTaskBarrier();
	TaskSystem::Get()->UpdateSchedule();
}

void Dispatcher::DispatchTask(Task* pTask)
{
	pTask->Dispatch();
	TaskSystem::Get()->AddTaskBarrier();
	TaskSystem::Get()->UpdateSchedule();
}

void Dispatcher::AddTask(Task* pTask)
{
	pTask->Dispatch();
}

uint32 Dispatcher::GetThreadCount()
{
	return TaskSystem::Get()->GetThreadCount();
}

void Dispatcher::FinishTaskGroup()
{
	TaskSystem::Get()->AddTaskBarrier();
}

void Dispatcher::WaitOnTask(Task* pTask)
{
	pTask->WaitForFinish();
}

void Dispatcher::End()
{
	TaskSystem::Get()->UpdateSchedule();
}