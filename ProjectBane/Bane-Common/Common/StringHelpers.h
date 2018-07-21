#pragma once

#include <sstream>
#include <vector>
#include <string>


inline std::vector<std::string> SplitString(const std::string& InStr, const char Delimiter)
{
	std::istringstream Str(InStr);

	std::vector<std::string> Result;

	std::string Buff;
	while (std::getline(Str, Buff, Delimiter))
	{
		Result.push_back(Buff);
	}
	return Result;
}

inline void RemoveWhiteSpace(std::string& Str)
{
	size_t Loc = 0;
	while ((Loc = Str.find(' ')) != std::string::npos)
	{
		Str.erase(Loc, 1);
	}
	while ((Loc = Str.find('\n')) != std::string::npos)
	{
		Str.erase(Loc, 1);
	}
	while ((Loc = Str.find('\t')) != std::string::npos)
	{
		Str.erase(Loc, 1);
	}
}

inline void RemoveTabs(std::string& Str)
{
	size_t Loc = 0;
	while ((Loc = Str.find('\t')) != std::string::npos)
	{
		Str.erase(Loc, 1);
	}
}

inline void RemoveAllNonSpaceWhiteSpace(std::string& Str)
{
	size_t Loc = 0;
	while ((Loc = Str.find('\n')) != std::string::npos)
	{
		Str.erase(Loc, 1);
	}
	while ((Loc = Str.find('\t')) != std::string::npos)
	{
		Str.erase(Loc, 1);
	}
}

inline bool IsWhiteSpace(const std::string& TestStr)
{
	bool bWhiteSpace = false;
	char C;
	for (uint i = 0; i < TestStr.length(); i++)
	{
		C = TestStr[i];
		if (C == ' ' || C == '\n' || C == '\t')
		{
			bWhiteSpace = true;
		}
		else
		{
			bWhiteSpace = false;
			break;
		}
	}
	return bWhiteSpace;
}


inline std::vector<std::string> Tokenize(const std::string& InRawStr, const std::string& Delimiters)
{
	std::vector<std::string> Result;
	size_t Start = InRawStr.find_first_not_of(Delimiters);
	size_t End = InRawStr.find_first_of(Delimiters, 0);

	while (End < std::string::npos)
	{
		std::string SubStr = InRawStr.substr(Start, End);
		if (!SubStr.empty())
		{
			Result.push_back(SubStr);
		}

		Start = End + 1;
		End = InRawStr.find_first_of(Delimiters, Start);
	}
	return Result;
}

// Parameter is called count for readability
inline std::string CutStringRange(std::string& InStr, const size_t Start, const size_t Count)
{
	std::string Result = InStr.substr(Start, Count);
	InStr.erase((size_t)Start, (size_t)Count);
	return Result;
}

inline std::string CutStringLocations(std::string& InStr, const size_t Start, const size_t End)
{
	BANE_CHECK((End > Start));
	std::string Result = InStr.substr(Start, End - Start);
	InStr.erase(InStr.begin() + Start, InStr.begin() + End);
	return Result;
}
