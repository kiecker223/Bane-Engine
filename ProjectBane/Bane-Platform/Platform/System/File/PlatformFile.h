#pragma once

#include "Common.h"
#include <Core/Containers/Array.h>
#include <string>

std::string ReadEntireFile(const std::string& FileName);

void EnsureFileDirectoryExists(const std::string& FileName);

TArray<std::string> GetAllFilesInFolder(const std::string& FolderName);

uint8* ReadFileBinary(const std::string& FileName, size_t fileSize);

uintmax_t GetFileSize(const std::string& FileName);
