#pragma once

#include <string>
#include <Core/Containers/Array.h>
#include <Common.h>

class IProcessHandle 
{
public:

	~IProcessHandle() { };

	template<class T>
	T* ToNativeHandle() const 
	{
		return reinterpret_cast<T*>(GetNativeHandle());
	}

	virtual void WaitForFinish() = 0;

protected:

	virtual void* GetNativeHandle() const = 0;

};


IProcessHandle* StartProcess(const std::string& ProcessName, const TArray<std::string>& ProcessArguments);
