#pragma once

#include "LogTarget.h"

typedef enum ELOG_TARGET_TYPE {
	LOG_TARGET_TYPE_CONSOLE,
	LOG_TARGET_TYPE_SURFACE
} ELOG_TARGET_TYPE;

class GlobalLog
{
public:

	ILogTarget* LogTarget;
	
	static void Initialize();
	static void Cleanup();

	inline void LogMessage(const std::string& Message) 
	{
		LogTarget->LogMessage(Message);
	}
	
	static GlobalLog* LogInstance;
};

static const char* END_LINE = "\n";

inline GlobalLog& BaneLog()
{
	return *GlobalLog::LogInstance;
}


inline GlobalLog& operator << (GlobalLog& Logger, char C)
{
	Logger.LogTarget->LogMessage({ C });
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, short S)
{
	Logger.LogTarget->LogMessage(std::to_string(S));
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, int I)
{
	Logger.LogTarget->LogMessage(std::to_string(I));
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, long long LL)
{
	Logger.LogTarget->LogMessage(std::to_string(LL));
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, unsigned char C)
{
	Logger.LogTarget->LogMessage(std::to_string(C));
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, unsigned short S)
{
	Logger.LogTarget->LogMessage(std::to_string(S));
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, unsigned int I)
{
	Logger.LogTarget->LogMessage(std::to_string(I));
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, unsigned long long LL)
{
	Logger.LogTarget->LogMessage(std::to_string(LL));
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, double d)
{
	Logger.LogTarget->LogMessage(std::to_string(d));
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, const std::string& Message)
{
	Logger.LogTarget->LogMessage(Message);
	return Logger;
}

inline GlobalLog& operator << (GlobalLog& Logger, const std::wstring& Message)
{
	Logger.LogTarget->LogMessage(std::string(Message.begin(), Message.end()));
	return Logger;
}


