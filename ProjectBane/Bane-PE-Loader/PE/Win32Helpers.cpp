#include "Win32Helpers.h"

const Flags IMAGE_OPTIONAL_HEADER_MAGIC = std::map<std::string, int>{
	{"PE32",		0x10b},
	{"PE32+",		0x20b }
};