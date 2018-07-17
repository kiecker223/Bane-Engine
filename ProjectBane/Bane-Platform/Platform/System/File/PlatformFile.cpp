#include "PlatformFile.h"



std::string ReadEntireFile(const std::string& FileName)
{
	std::string Result;

	std::ifstream FileStream(FileName);

	if (FileStream.is_open())
	{
		std::string Line;
		while (std::getline(FileStream, Line))
		{
			Result += Line + '\n';
		}
	}

	return Result;
}

