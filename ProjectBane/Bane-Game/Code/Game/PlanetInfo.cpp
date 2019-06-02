
#include "PlanetInfo.h"
#include <iostream>


double CalculateTimeForGame(double Year, double Month, double Day, double Hour, double Minute, double Second)
{
	double d = 367.0*Year - 7.0 * (Year + (Month + 9.0) / 12.0) / 4.0 - 3.0 * ((Year + (Month - 9.0) / 7.0) / 100.0 + 1.0) / 4.0 + 275.0*Month/9.0 + Day - 730515.0;
	double f = (Hour + (Minute/60.0) + (Second/3600.0)) / 24.0;
	return d + f;
}

double KeplerStart(double Eccentricity, double M)
{
	double t34 = Eccentricity * Eccentricity;
	double t35 = t34 * Eccentricity;
	double t33 = cos(M);
	return M + (-1. / 2.*t35 + Eccentricity + (t34 + 3. / 3.*t33*t35)*t33)*sin(M);
}

double Eps3(double Eccentricity, double M, double StartingValue)
{
	double t1 = cos(StartingValue);
	double t2 = -1. + Eccentricity * t1;
	double t3 = sin(StartingValue);
	double t4 = Eccentricity * t3;
	double t5 = -StartingValue + t4 + M;
	double t6 = t5 / (0.5*t5*t4 / t2 + t2);
	return t5 / ((0.5*t3 - 1. / 6.*t1*t6)*Eccentricity*t6 + t2);
}

double SolveEccentricAnomaly(double Time, double Period, double Eccentricity, double TimePeriod)
{
	double Tolerance;
	if (Eccentricity < 0.8)
	{
		Tolerance = 1e-14;
	}
	else
	{
		Tolerance = 1e-13;
	}
	double n = 2 * _M_PI_ / Period;
	double MeanAnomaly = n * (Time - TimePeriod);
	double MNorm = fmod(MeanAnomaly, 2. * _M_PI_);
	double E0 = KeplerStart(Eccentricity, MNorm);
	double dE = Tolerance + 1;
	double E = 0.;
	uint32 Iter = 0;
	while (dE > Tolerance)
	{
		E = E0 - Eps3(Eccentricity, MNorm, E0);
		dE = abs(E - E0);
		E0 = E;
		Iter++;
		if (Iter == 100) break;
	}
	return E;
}



static vec3 SunPosition = vec3(M_NEGATIVE_INFINITY, M_NEGATIVE_INFINITY, M_NEGATIVE_INFINITY);
const double SunMass = 1.989e30;

static vec3 EarthVelocity;
const double EarthMass = 5.97219e24;

/*
	N = longitude of the ascending node
	i = inclination to the ecliptic (plane of the Earth's orbit)
	w = argument of perihelion
	a = semi-major axis, or mean distance from Sun
	e = eccentricity (0=circle, 0-1=ellipse, 1=parabola)
	M = mean anomaly (0 at perihelion; increases uniformly with time)

	w1 = N + w   = longitude of perihelion
	L  = M + w1  = mean longitude
	q  = a*(1-e) = perihelion distance
	Q  = a*(1+e) = aphelion distance
	P  = a ^ 1.5 = orbital period (years if a is in AU, astronomical units)
	T  = Epoch_of_M - (M(deg)/360_deg) / P  = time of perihelion
	v  = true anomaly (angle between position and perihelion)
	E  = eccentric anomaly
*/


vec3 GetPlanetPosition(PLANET_ORBIT_INFO OrbitInfo)
{
	vec3 Position;

	OrbitInfo.InclinationOfEcliptic = radiansD(OrbitInfo.InclinationOfEcliptic);
	double EccentricAnomaly = OrbitInfo.MeanAnomaly + (OrbitInfo.Eccentricity * (180.0 / _M_PI_)) * sin(OrbitInfo.MeanAnomaly) * (1.0 + OrbitInfo.Eccentricity * cos(OrbitInfo.MeanAnomaly));
	double E0 = EccentricAnomaly;
	double E1 = 0.;
	if (OrbitInfo.Eccentricity > 0.06)
	{
		for (uint32 i = 0; i < 100; i++)
		{
			E1 = E0 - (E0 - (OrbitInfo.Eccentricity*(180. / _M_PI_)) * sin(E0) - OrbitInfo.MeanAnomaly) / (1 - OrbitInfo.Eccentricity * cos(E0));
			if (abs(E1 - E0) <= 1E-4)
			{
				break;
			}
		}
		EccentricAnomaly = E0;
	}
	double xv = OrbitInfo.SemimajorAxis * cos(EccentricAnomaly) - OrbitInfo.Eccentricity;
	double Test = sqrt(1.0 - OrbitInfo.Eccentricity * OrbitInfo.Eccentricity);
	double yv = OrbitInfo.SemimajorAxis * Test * sin(EccentricAnomaly);
	double TrueAnomaly = atan2(yv, xv);
	double Distance = sqrt(xv * xv + yv * yv);
	double SinVW = sin(TrueAnomaly + OrbitInfo.ArgumentOfPerihelion);
	double CosVW = cos(TrueAnomaly + OrbitInfo.ArgumentOfPerihelion);
	double CosI = cos(OrbitInfo.LongitudeOfAscendingNode);
	double SinI = sin(OrbitInfo.LongitudeOfAscendingNode);
	double CosInclination = cos(OrbitInfo.InclinationOfEcliptic);
	Position = vec3(
		Distance * (CosI * CosVW - SinI * SinVW * CosInclination),
		Distance * (SinVW * sin(OrbitInfo.InclinationOfEcliptic)),
		Distance * (SinI * CosVW + CosI * SinVW * CosInclination)
	);
	return Position;
}

PLANET_START_INFO PlacePlanet(PLANET_ORBIT_INFO OrbitInfo)
{
	PLANET_START_INFO Result;
	UNUSED(OrbitInfo);
	UNUSED(Result);
	return Result;
}

/*
void InitApplication1()
{
	std::cout << "Astronomical unit check: " << M_AU(1.0) << std::endl;
	const double TimeArg = 9000.;
	{
		PLANET_START_INFO SInfo = PlacePlanet(
			{
				"Sun",
				0.0, 
				0.0, 
				282.9404 + 4.70935E-5 * TimeArg,
				M_AU(1.0), 
				0.016709 - 1.151E-9*TimeArg,
				356.0470 + 0.9856002585 * TimeArg
			},
			TimeArg);
	}
	{
		PLANET_START_INFO SInfo = PlacePlanet(
			{
				"Mercury",
				48.3313 + 3.24587E-5 * TimeArg,
				7.0047 + 5.00E-8 * TimeArg,
				29.1241 + 1.01444E-5 * TimeArg, 
				M_AU(0.397098),
				0.205635 + 5.59E-10 * TimeArg,
				168.6562 + 4.0923344368 * TimeArg
			},
			TimeArg
		);
	}
	{
		PLANET_START_INFO SInfo = PlacePlanet(
			{ 
				"Venus", 
				76.6799 + 2.46590E-5 * TimeArg, 
				3.3946 + 2.75E-8 * TimeArg, 
				54.8910 + 1.38374E-5 * TimeArg,
				M_AU(0.723330),	
				0.006773 - 1.302E-9 * TimeArg, 
				48.0052 + 1.6021302244 * TimeArg 
			}, 
			TimeArg
		);
	}
	{
		PLANET_START_INFO SInfo = PlacePlanet(
			{
				"Moon",
				125.1228 - 0.0529538083 * TimeArg,
				5.1454,
				318.0634 + 0.1643573223 * TimeArg,
				60.266 * (12742000.0 / 2.),
				0.054900,
				1163654 + 13.069929509 * TimeArg
			},
			TimeArg
		);
	}

}
*/