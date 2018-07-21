#pragma once

#include "Common.h"
#include <vector>
#include <string>

void EnsureFileDirectoryExists(const std::string& FileName);

std::vector<std::string> GetAllFilesInFolder(const std::string& FolderName);
