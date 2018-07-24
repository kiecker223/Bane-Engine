#include "../../Platform/System/Process.h"
#include <Windows.h>

class WindowsProcessHandle : public IProcessHandle
{

public:

	~WindowsProcessHandle()
	{
		CloseHandle(ProcessInfo.hProcess);
	}

	virtual void WaitForFinish() final override
	{
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	}

	virtual void* GetNativeHandle() const final override
	{
		return reinterpret_cast<void*>(ProcessInfo.hProcess);
	}


	PROCESS_INFORMATION ProcessInfo;
};


IProcessHandle* StartProcess(const std::string& ProcessName, const std::vector<std::string>& ProcessArguments)
{
	WindowsProcessHandle* Result = new WindowsProcessHandle();
	STARTUPINFO StartupInfo = { };
	StartupInfo.cb = sizeof(STARTUPINFO);
	char CWD[256];
	GetCurrentDirectoryA(256, CWD);
	std::string ArgumentsStr = CWD;
	ArgumentsStr += ProcessName + ' ';
	for (uint i = 0; i < ProcessArguments.size(); i++)
	{
		ArgumentsStr += ProcessArguments[i] + ' ';
	}
	char Arguments[2048];
	memcpy(Arguments, ArgumentsStr.c_str(), ArgumentsStr.size() - 1);
	Arguments[ArgumentsStr.size() - 1] = '\0';
	if (!CreateProcessA(
		ProcessName.c_str(),
		Arguments,
		nullptr, nullptr,
		FALSE, 0,
		nullptr, nullptr,
		&StartupInfo, &Result->ProcessInfo))
	{
		DWORD Error = GetLastError();
		UNUSED(Error);
		BANE_CHECK(false);
		return nullptr;
	}
	return Result;
}



