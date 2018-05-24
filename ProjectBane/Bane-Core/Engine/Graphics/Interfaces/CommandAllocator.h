#pragma once


class CommandAllocator
{
public:

	void* AllocateCommand(uint Size);

	template<typename TCmd>
	TCmd* AllocCommand();

	void Reset(uint NumBytes = 1024);

private:



};