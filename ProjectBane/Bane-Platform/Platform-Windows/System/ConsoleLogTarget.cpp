#include "../../Platform/System/Logging/ConsoleLogTarget.h"
#include <iostream>


void ConsoleLogTarget::Initialize()
{
}

void ConsoleLogTarget::LogMessage(const std::string& Message)
{
	std::cout << Message;
}

void ConsoleLogTarget::FlushLog()
{
	std::cout.flush();
}