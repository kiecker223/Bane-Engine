#pragma once

// Disable warnings related to padding structures
#pragma warning(disable:4324)

#ifdef _DEBUG
#define BANE_CHECK(x) if (!(x)) {  __debugbreak(); abort(); }
#else
#define BANE_CHECK(x) 
#endif

#define KILOBYTE(x) x * 1000
#define MEGABYTE(x) KILOBYTE(x) * 1000

#define MAKE_ALIGN(x) __declspec(align(x))
#define SHADER_ALIGNMENT MAKE_ALIGN(sizeof(float4))

#define NO_VTABLE __declspec(novtable)

#define UNUSED(expr) do { (void)(expr); } while (0)
