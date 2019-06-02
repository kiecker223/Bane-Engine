#pragma once

// #include <malloc.h>
// #include <stdint.h>
// #include <cstddef>
// #include <new.h>
// #include <stdlib.h>


// inline void* operator new (std::size_t Count)
// {
// 	return _aligned_malloc(16, Count);
// }
// 
// inline void* operator new[](std::size_t Count)
// {
// 	return _aligned_malloc(16, Count);
// }
// 
// inline void* operator new  (std::size_t Count, std::align_val_t Al)
// {
// 	return _aligned_malloc(static_cast<std::size_t>(Al), Count);
// }
// 
// inline void* operator new[](std::size_t Count, std::align_val_t Al)
// {
// 	return _aligned_malloc(static_cast<std::size_t>(Al), Count);
// }
