#pragma once

#include <KieckerMath.h>


typedef struct PLANET_START_INFO {
	vec3 Position;
	vec3 Velocity;
} PLANET_START_INFO;

/*
	N = longitude of the ascending node
	i = inclination to the ecliptic (plane of the Earth's orbit)
	w = argument of perihelion
	a = semi-major axis, or mean distance from Sun
	e = eccentricity (0=circle, 0-1=ellipse, 1=parabola)
	M = mean anomaly (0 at perihelion; increases uniformly with time)
*/

typedef struct PLANET_ORBIT_INFO {
	std::string PlanetName;
	double LongitudeOfAscendingNode;
	double InclinationOfEcliptic;
	double ArgumentOfPerihelion;
	double SemimajorAxis;
	double Eccentricity;
	double MeanAnomaly;
} PLANET_ORBIT_INFO;


double CalculateTimeForGame(double Year, double Month, double Day, double Hour, double Minute, double Second);
vec3 GetPlanetPosition(PLANET_ORBIT_INFO OrbitInfo);


PLANET_START_INFO PlacePlanet(PLANET_ORBIT_INFO OrbitInfo);

