#pragma once

// Disable warnings related to padding structures
#if _MSC_VER
#pragma warning(disable:4324)
#endif
#ifdef _DEBUG
#define BANE_CHECK(x) if (!(x)) {  __debugbreak(); abort(); }
#else
#define BANE_CHECK(x) 
#endif

#define KILOBYTE(x) x * 1000
#define MEGABYTE(x) KILOBYTE(x) * 1000

#define MAKE_ALIGN(x) __declspec(align(x))
#define SHADER_ALIGNMENT MAKE_ALIGN(sizeof(float4))
#define ALIGN_FOR_GPU_BUFFER MAKE_ALIGN(256)

#if defined(BANE_BUILD_EXPORT) && 1
#define BANE_ENGINE_API __declspec(dllimport)
#else
#define BANE_ENGINE_API __declspec(dllexport)
#endif

#define NO_VTABLE __declspec(novtable)

#define UNUSED(expr) do { (void)(expr); } while (0)

#define GRAV_CONST 6.6747E-11f
#define M_GRAV_CONST 6.6747E-11
#define AU(x) x * 149600000.f
#define M_AU(x) x * 149600000.0

