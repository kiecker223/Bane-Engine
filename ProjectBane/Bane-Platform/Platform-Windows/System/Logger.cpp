#include "../../Platform/System/Logging/Logger.h"
#include "../../Platform/System/Logging/ConsoleLogTarget.h"

GlobalLog* GlobalLog::LogInstance = nullptr;

void GlobalLog::Initialize()
{
	LogInstance = new GlobalLog();
	LogInstance->LogTarget = new ConsoleLogTarget();
}

void GlobalLog::Cleanup()
{
	delete LogInstance->LogTarget;
	delete LogInstance;
}

