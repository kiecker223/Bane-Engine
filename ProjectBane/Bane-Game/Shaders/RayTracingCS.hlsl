
#define MAX_GEOMETRY 30
#define MAX_LIGHTS 5

#define HIT_FLAG_SPHERE 1
#define HIT_FLAG_BOX 2
#define HIT_FLAG_PLANE 3

struct MATERIAL
{
	float3 Color;
	float Metallic;
	float Roughness;
};

struct POINT_LIGHT
{
	float3 Color;
	float3 Position;
	float Intensity;
};

struct DIRECTIONAL_LIGHT
{
	float3 Direction;
	float3 Color;
};

struct PLANE
{
	MATERIAL Material;
	float3 Position;
	float Size;
	float3 Normal;
};

struct SPHERE
{
	MATERIAL Material;
	float3 Center;
	float Radius;
};

struct BOX
{
	MATERIAL Material;
	float3 Min;
	float3 Max;
};


cbuffer ScreenData : register(b0)
{
	float2 ScreenDimensions;
	float Frame;
}

cbuffer GeometryData : register(b1)
{
	PLANE        Planes[MAX_GEOMETRY];
	int          NumPlanes;
	SPHERE       Spheres[MAX_GEOMETRY];
	int	         NumSpheres;
	BOX          Boxes[MAX_GEOMETRY];
	int          NumBoxes;
	POINT_LIGHT	 PointLights[MAX_LIGHTS];
	int          NumPointLights;
	DIRECTIONAL_LIGHT DirectionalLights[MAX_LIGHTS];
	int          NumDirectionalLights;
}

RWTexture2D<float4> Image : register(u0);


struct RAY
{
	float3 Pos;
	float3 Dir;
};


struct HITRECORD
{
	float3 Normal;
	float3 Position;
};


// Credit: https://gamedev.stackexchange.com/questions/32681/random-number-hlsl
float rand_1_05(in float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}


float HitSphere(SPHERE Sphere, RAY Ray)
{
	float3 OriginMCenter = Ray.Pos - Sphere.Center;
	float A = dot(Ray.Dir, Ray.Dir);
	float B = 2 * dot(OriginMCenter, Ray.Dir);
	float C = dot(OriginMCenter, OriginMCenter) - Sphere.Radius * Sphere.Radius;
	float Discriminate = B * B - 4 * A * C;
	if (Discriminate < 0)
	{
		return -1;
	}
	return -B * sqrt(Discriminate) / 2 * A;
}

float HitPlane(PLANE Plane, RAY Ray)
{
	float RDotPlaneNormal = dot(Plane.Normal, Ray.Dir);
	if (abs(RDotPlaneNormal) > 1e-3)
	{
		float T = dot((Plane.Position - Ray.Pos), Plane.Normal) / RDotPlaneNormal;
		return T;
	}
	return -1.0f;
}

// Return the nearest???
float HitBox(BOX Box, RAY Ray)
{
	// HORRIBLE NAMES ALEX!!
	float3 TimeMin = (Box.Min - Ray.Pos) / Ray.Dir;
	float3 TimeMax = (Box.Max - Ray.Pos) / Ray.Dir;
	float3 Min = min(TimeMin, TimeMax);
	float3 Max = max(TimeMin, TimeMax);

	float NearT = max(max(Min.x, Min.y), Min.z);
	// float FarT = min(min(Max.x, Max.y), Max.z); // ??
	if (NearT < 0)
	{
		return 0.0f;
	}
	return NearT;
}

float3 PosAtTime(RAY InRay, float T)
{
	return (InRay.Pos + (InRay.Dir * T));
}

// PixelCoord is just Input.Position.xy
RAY BeginRay(uint2 DispatchId)
{
	int2 ScreenDims = int2(ScreenDimensions.x, ScreenDimensions.y);
	const float3 CameraPosition = float3(0.0f, 0.0f, 2.0f);
	const float3 CameraForward = float3(0.0f, 0.0f, -1.0f);
	const float FovX = 85.f;
	float FovY = (ScreenDimensions.y / ScreenDimensions.x) * FovX;

	float2 ScreenPos;
	int2 HalfImgSize = ScreenDims / 2;
	int2 ImgPos = DispatchId - (-HalfImgSize);
	ScreenPos = float2(ImgPos.x, ImgPos.y);
	ScreenPos /= (ScreenDimensions / 2);

	RAY Result;
	Result.Pos = CameraPosition;

	float HalfFovX = FovX / 2.f;
	float HalfFovY = FovY / 2.f;

	// TODO: Bughunt
	float AngleX = radians(HalfFovX * (ScreenPos.x));
	float AngleY = radians(HalfFovY * (ScreenPos.y));
	float CosAngleX = cos(AngleX);
	float SinAngleX = sin(AngleX);
	float CosAngleY = cos(AngleY);
	float SinAngleY = sin(AngleY);

	float3x3 Rot = float3x3(
		CosAngleX, 0, SinAngleX,
		0, CosAngleY, -SinAngleY,
		-SinAngleX, SinAngleY, CosAngleX + CosAngleY
		);

	Result.Dir = CameraForward;
	Result.Dir = mul(Rot, Result.Dir);
	Result.Dir = normalize(Result.Dir);
	return Result;
}

int CastToWorld(out float3 OutNormal, out float T, out MATERIAL OutMaterial, RAY Ray)
{
	T = 3.402823466e+38F;
	int i = 0;
	int HitFlag = 0;
	for (i = 0; i < NumSpheres; i++)
	{
		float Temp = HitSphere(Spheres[i], Ray);
		if (Temp < T)
		{
			T = Temp;
			HitFlag = HIT_FLAG_SPHERE;
			OutNormal = PosAtTime(Ray, T) - Spheres[i].Center;
			OutMaterial = Spheres[i].Material;
		}
	}
	for (i = 0; i < NumPlanes; i++)
	{
		float Temp = HitPlane(Planes[i], Ray);
		if (Temp < T)
		{
			T = Temp;
			HitFlag = HIT_FLAG_PLANE;
			OutNormal = Planes[i].Normal;
			OutMaterial = Planes[i].Material;
		}
	}
	return HitFlag;
}


float3 CastRay(RAY Ray, int Bounces, int MaxBounces, float3 LastColor, float2 RandSeed)
{
	if (Bounces < MaxBounces)
	{
		float3 Color;
		float T;
		float3 Normal;
		MATERIAL Material;
		if (CastToWorld(Normal, T, Material, Ray) > 0)
		{
			Color = Material.Color;
			HITRECORD Record;
			Record.Normal = Normal;
			Record.Position = PosAtTime(Ray, T);
			if (Bounces == 0)
			{
				Color = Material.Color;
			}
			Record.Normal = Normal * (Material.Roughness * rand_1_05(RandSeed));
			RAY NewRay;
			NewRay.Pos = Record.Position;
			NewRay.Dir = reflect(Ray.Dir, Record.Normal);
			return Color * CastRay(NewRay, Bounces + 1, MaxBounces, Color, RandSeed * rand_1_05(RandSeed));
		}
		else
		{
			return -Ray.Dir;
		}
	}
	return LastColor;
}

[numthreads(16, 16, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	float3 Color = float3(0.1f, 0.1f, 0.1f);
	float2 RandSeed = float2((float)DTid.x, (float)DTid.y);

	RAY Ray = BeginRay(DTid.xy);
	Color = CastRay(Ray, 0, 3, Color, RandSeed);

	Image[DTid.xy] = float4(Color, 1.0f);
}