#pragma once


#ifdef _DEBUG
#define BANE_CHECK(x) if (!(x)) {  __debugbreak(); abort(); }
#else
#define BANE_CHECK(x) 
#endif


#define NO_VTABLE __declspec(novtable)

#define UNUSED(expr) do { (void)(expr); } while (0)
