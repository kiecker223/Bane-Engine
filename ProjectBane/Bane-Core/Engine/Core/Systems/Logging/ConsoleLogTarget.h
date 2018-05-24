#pragma once

#include "LogTarget.h"

class ConsoleLogTarget : public ILogTarget
{
public:

	void Initialize() override final;

	void LogMessage(const std::string& Message) override final;

	void FlushLog() override final;
};
