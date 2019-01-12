#pragma once
#include <Common/Types.h>
#include <Common/BaneMacros.h>
#include <KieckerMath.h>
#include <string>


typedef struct ORBITAL_ELEMENTS {
	std::string PlanetName;
	double LongitudeOfAscendingNode;
	double InclinationOfEcliptic;
	double ArgumentOfPerihelion;
	double SemimajorAxis;
	double Eccentricity;
	double MeanAnomaly;
} ORBITAL_ELEMENTS;

class PlanetaryBody
{
public:

	ORBITAL_ELEMENTS Orbit;

	inline double3 GetPosition() { }
	inline double3 GetVelocity() { }

	void SetTime(double InTime) { UNUSED(InTime); }
	void TickTime(double TimeRate) { UNUSED(TimeRate); }

private:

	double m_Time;

};
