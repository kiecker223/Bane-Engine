#pragma once

#include "Common.h"
#include <vector>
#include <string>

std::string ReadEntireFile(const std::string& FileName);

void EnsureFileDirectoryExists(const std::string& FileName);

std::vector<std::string> GetAllFilesInFolder(const std::string& FolderName);

uint8* ReadFileBinary(const std::string& FileName, uint& NumBytes);
