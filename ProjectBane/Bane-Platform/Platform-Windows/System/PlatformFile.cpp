#include "../../Platform/System/File/PlatformFile.h"
#include <Common/StringHelpers.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fstream>
#include <string>

std::string ReadEntireFile(const std::string& FileName)
{
	std::ifstream InStream(FileName);
	std::string Result;
	if (InStream.is_open())
	{
		std::string Buff;
		while (std::getline(InStream, Buff))
		{
			Result += Buff + '\n';
		}
	}
	return Result;
}

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

uint8* ReadFileBinary(const std::string& FileName, uint& NumBytes)
{
	uint8* FileBinary = nullptr;
	HANDLE FileHandle = CreateFile(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	DWORD Error = GetLastError();
	BANE_CHECK(Error == 0);
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		DWORD BytesNeeded = GetFileSize(FileHandle, nullptr);
		if (BytesNeeded == 0xffffffff)
		{
			// Fix me later
			BANE_CHECK(false);
		}
		FileBinary = new uint8[BytesNeeded];
		DWORD Stub;
		ReadFile(FileHandle, (LPVOID)FileBinary, BytesNeeded, &Stub, nullptr);
		BANE_CHECK(Stub == BytesNeeded);
		NumBytes = static_cast<uint>(BytesNeeded);
		CloseHandle(FileHandle);
	}
	return FileBinary;
}
