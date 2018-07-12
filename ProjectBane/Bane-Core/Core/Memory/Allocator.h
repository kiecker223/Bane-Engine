#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Common.h"

#define KILOBYTES(x) ((1000) * x)
#define MEGABYTES(x) ((1000  *  1000) * x)
#define GIGABYTES(x) ((1000  * (1000  * 1000)) * x)


void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);





	