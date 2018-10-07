#pragma once

#include <cstddef>

#ifdef __GNUC__
#include <stdint.h>
#endif

typedef unsigned char		uint8;
typedef signed char			int8;

typedef unsigned short		uint16;
typedef signed short		int16;

typedef float				float32;
typedef double				float64;

#if !defined(_WIN32) && defined(__GNUC__) && (defined(__x86_64__))
typedef unsigned int		uint32;
typedef signed int			int32;
typedef unsigned long		uint64;
typedef signed long			int64;
#else
typedef unsigned long		uint32;
typedef signed long			int32;
typedef unsigned long long	uint64;
typedef signed long long	int64;
#endif
