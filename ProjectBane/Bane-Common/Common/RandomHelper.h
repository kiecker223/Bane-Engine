#pragma once

#include <random>


inline double RandomDoubleNumber(double Min, double Max)
{
	BANE_CHECK(Min < Max);
	std::random_device Dev;
	std::mt19937 Gen(Dev());
	std::uniform_real_distribution<double> Dist(Min, Max);
	return Dist(Gen);
}

