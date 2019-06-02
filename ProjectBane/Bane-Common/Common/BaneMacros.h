#pragma once

// Disable warnings related to padding structures
#if _MSC_VER
#pragma warning(disable:4324)
#endif
#ifdef _DEBUG
#include <stdlib.h>
#define BANE_CHECK(x) if (!(x)) {  __debugbreak(); abort(); }
#else
#define BANE_CHECK(x) if (!(x)) { abort(); }
#endif

#define KILOBYTE(x) x * 1024
#define MEGABYTE(x) KILOBYTE(x) * 1024

#define MAKE_ALIGN(x) __declspec(align(x))
#define SHADER_ALIGNMENT MAKE_ALIGN(sizeof(fvec4))
#define ALIGN_FOR_GPU_BUFFER MAKE_ALIGN(256)

#if defined(BANE_BUILD_EXPORT) && 1
#define BANE_ENGINE_API __declspec(dllimport)
#else
#define BANE_ENGINE_API __declspec(dllexport)
#endif

#define NO_VTABLE __declspec(novtable)

#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN32_) || defined(CYGWIN)
#define PRAGMA_STATEMENT(x) __pragma(x)
#elif defined(__unix__)
#define PRAGMA_STATEMENT(x) _Pragma(x)
#else
#error Unrecognized platform
#endif

#define UNUSED(expr) do { (void)(expr); } while (0)

#define GRAV_CONST 6.6747E-11f
#define M_GRAV_CONST 6.6747E-11
#define AU(x) x * 149600000000.f
#define M_AU(x) x * 149600000000.0

#define M_POSITIVE_INFINITY 0x7FF0000000000000
#define M_NEGATIVE_INFINITY 0xFFF0000000000000

