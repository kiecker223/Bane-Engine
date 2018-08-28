#include "PlatformFile.h"
#include <filesystem>
#include <iostream>
#include <fstream>

std::vector<std::string> GetAllFilesInFolder(const std::string& FolderName) {
	std::vector<std::string> Result;

	for (auto& File : std::filesystem::directory_iterator(FolderName))
	{
		Result.push_back(File.path().filename().string());
	}

	return Result;
}

void EnsureFileDirectoryExists(const std::string& FileName)
{
	auto EndOfDirectory = FileName.find_last_of("/");
	std::string Directory = FileName.substr(0, EndOfDirectory);
	std::cout << Directory << std::endl;
	std::filesystem::create_directories(Directory);
}

std::string ReadEntireFile(const std::string& FileName)
{
	std::ifstream InStream(FileName);
	std::string Result;
	Result.assign(std::istreambuf_iterator<char>(InStream),
		std::istreambuf_iterator<char>());

	return Result;
}

uint8* ReadFileBinary(const std::string& FileName, size_t fileSize)
{
	uint8* FileBinary = new uint8[fileSize];

	std::ifstream rfile(FileName, std::ios::binary);
	rfile.read((char*)FileBinary, fileSize * sizeof(FileBinary[0]));
	rfile.close();

	return FileBinary;
}

uintmax_t GetFileSize(const std::string& FileName) {
	std::filesystem::path PathForSize(FileName);
	auto filesize = std::filesystem::file_size(PathForSize);

	return filesize;
}
