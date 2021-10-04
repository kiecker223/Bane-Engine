#pragma once

#include <Common/Types.h>
#include <atomic>

class TaskExecutionHandle
{
public:

	TaskExecutionHandle() :
		DispatchIndex(0),
		DispatchSize(0),
		pOwningTask(nullptr)
	{
	}

	uint32 DispatchIndex;
	uint32 DispatchSize;

	std::atomic<uint64> NextValue;
	std::atomic<uint64> CompletedValue;

	inline bool IsCompleted()
	{
		return NextValue.load() == CompletedValue.load();
	}

	class Task* pOwningTask;
	virtual void InternalExecute() = 0;
};
