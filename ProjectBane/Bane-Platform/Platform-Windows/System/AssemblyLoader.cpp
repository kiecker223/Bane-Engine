#include <Windows.h>
#include <Common.h>
#include "../../Platform/System/AssemblyLoader.h"


Assembly::Assembly()
{

}

Assembly::~Assembly()
{
	FreeLibrary((HMODULE)NativeHandle);
}

void* Assembly::LoadProc(const char* ProcName)
{
	void* Proc = GetProcAddress((HMODULE)NativeHandle, ProcName);
	DWORD Error = GetLastError();
	BANE_CHECK(Error == 0);
	return Proc;
}

Assembly* AssemblyLoader::LoadAssembly(const char* AssemblyName)
{
	Assembly* Result = new Assembly();
	Result->NativeHandle = (void*)LoadLibraryA(AssemblyName);
	BANE_CHECK(GetLastError() == 0);
	return Result;
}



