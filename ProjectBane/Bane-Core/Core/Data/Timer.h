#pragma once
#include <chrono>
#include <Common/Types.h>


class Timer
{
public:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

	inline void StartTimer()
	{
		Start = Clock::now();
	}

	inline void EndTimer()
	{
		End = Clock::now();
	}

	inline float GetTimerElapsedSeconds()
	{
		return static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(End - Start).count()) / 1E+9f;
	}

	TimePoint Start;
	TimePoint End;
};


class ScopedTimer
{
public:
	Timer Time;
	ScopedTimer() { Time.StartTimer(); }
	~ScopedTimer() { Time.EndTimer(); }
};

