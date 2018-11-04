#include <Core/Containers/Array.h>


class Assembly
{
public:

	~Assembly();
	Assembly();

	//template<class T>
	//inline T LoadFunction(const char* FunctionName) 
	//{
	//	return (T)FindProc(FunctionName);
	//}

	void* LoadProc(const char* ProcName);

	void* NativeHandle;
};

class AssemblyLoader
{
public:

	Assembly* LoadAssembly(const char* AssemblyName);

private:

	void* m_NativeHandle;

};