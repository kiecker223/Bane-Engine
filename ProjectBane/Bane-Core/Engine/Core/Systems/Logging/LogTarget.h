#pragma once

#include <string>

class ILogTarget
{
public:

	virtual void Initialize() = 0;
	virtual void LogMessage(const std::string& Message) = 0;
	virtual void FlushLog() = 0;

};

