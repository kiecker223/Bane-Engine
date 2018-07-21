#include "../../Platform/System/File/PlatformFile.h"
#include <Common/StringHelpers.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void EnsureFileDirectoryExists(const std::string& FileName)
{
	std::string DstFile = FileName;
	size_t Loc = 0;
	while (Loc != std::string::npos)
	{
		Loc = DstFile.find("\\");
		if (Loc != std::string::npos)
			DstFile.replace(Loc, Loc + 2, "/");
	}
	if (FileName.find('/') != std::string::npos)
	{
		std::vector<std::string> DirectoriesToMake = SplitString(FileName, '/');
		DirectoriesToMake.erase(DirectoriesToMake.begin() + (DirectoriesToMake.size() - 1));
		std::string LastDir;
		for (std::string& Str : DirectoriesToMake)
		{
			LastDir += Str + "/";
			CreateDirectoryA(LastDir.c_str(), nullptr);
		}
	}
}

std::vector<std::string> GetAllFilesInFolder(const std::string& FolderName)
{
	std::vector<std::string> Result;
	HANDLE File;
	WIN32_FIND_DATA FindData;
	std::string FindName = FolderName + "/*.*";
	File = FindFirstFile(FindName.c_str(), &FindData);
	if (File != INVALID_HANDLE_VALUE)
	{
		while (FindNextFileA(File, &FindData))
		{
			if (std::string(FindData.cFileName) != "..")
				Result.push_back(std::string(FindData.cFileName));
		}
	}
	return Result;
}
