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
	float P0, P1, P2;
};

struct POINT_LIGHT
{
	float3 Color;
	float P0;
	float3 Position;
	float Intensity;
};

struct DIRECTIONAL_LIGHT
{
	float3 Direction;
	float P0;
	float3 Color;
	float P1;
};

struct PLANE
{
	MATERIAL Material;
	float3 Position;
	float Size;
	float3 Normal;
	float P0;
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
	float P0;
	float3 Max;
	float P1;
};

Resources
{
	cbuffer ScreenData : register(b0)
	{
		float2 ScreenDimensions;
		float Frame;
		int RayBounces;
	}

	cbuffer GeometryData : register(b1)
	{
		PLANE        Planes[30];
		SPHERE       Spheres[30];
		BOX          Boxes[30];
		POINT_LIGHT	 PointLights[5];
		DIRECTIONAL_LIGHT DirectionalLights[5];
		int			 Padding01;
		int          NumPlanes;
		int	         NumSpheres;
		int          NumBoxes;
		int          NumPointLights;
		int          NumDirectionalLights;
	}
}

struct VS_INPUT
{
	float2 Position : POSITION;
	float2 TexCoords : TEXCOORDS;
};

struct VS_OUTPUT
{
	float4 SystemPosition : SV_POSITION;
	float4 Position : POSITION;
	float2 TexCoords : TEXCOORDS;
};

VS_OUTPUT VSMain(VS_INPUT In)
{
	VS_OUTPUT Result;
	Result.SystemPosition = float4(In.Position, 0.0f, 1.0f);
	Result.Position = Result.SystemPosition;
	Result.TexCoords = In.TexCoords;
	return Result;
}

struct RAY
{
	float3 Pos;
	float3 Dir;
};


struct HITRECORD
{
	float3 Normal;
	float3 Position;
	MATERIAL Material;
};


// Credit: https://gamedev.stackexchange.com/questions/32681/random-number-hlsl
float rand(in float2 uv)
{
	return (frac(sin(dot(uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
}



float HitSphere(in SPHERE Sphere, RAY Ray)
{
	float3 OriginMCenter = Ray.Pos - Sphere.Center;
	float A = dot(Ray.Dir, Ray.Dir);
	float B = 2 * dot(OriginMCenter, Ray.Dir);
	float C = dot(OriginMCenter, OriginMCenter) - (Sphere.Radius * Sphere.Radius);
	float Discriminate = (B * B) - (4 * A * C);
	if (Discriminate < 0)
	{
		return -1.0f;
	}
	return (-B - sqrt(Discriminate)) / (2 * A);
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
RAY BeginRay(float2 PixelCoord)
{
	const float3 CameraPosition = float3(0.0f, 0.0f, 2.0f);
	const float3 CameraForward = float3(0.0f, 0.0f, -1.0f);
	const float FovX = 85.f;
	float FovY = (ScreenDimensions.y / ScreenDimensions.x) * FovX;

	RAY Result;
	Result.Pos = CameraPosition;

	float HalfFovX = FovX / 2.f;
	float HalfFovY = FovY / 2.f;
	
	// TODO: Bughunt
	float AngleX = radians(HalfFovX * (PixelCoord.x));
	float AngleY = radians(HalfFovY * (PixelCoord.y));
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

RAY CreateRay(HITRECORD Record, RAY OldRay)
{
	RAY Result;
	Result.Pos = Record.Position;
	Result.Dir = normalize(reflect(OldRay.Dir, Record.Normal));
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
		if (Temp < T && Temp > 0.0f)
		{
			T = Temp;
			HitFlag = HIT_FLAG_SPHERE;
			OutNormal = normalize(PosAtTime(Ray, T) - Spheres[i].Center);
			OutMaterial = Spheres[i].Material;
		}
	}
	for (i = 0; i < NumPlanes; i++)
	{
		float Temp = HitPlane(Planes[i], Ray);
		if (Temp < T && Temp > 0.0f)
		{
			T = Temp;
			HitFlag = HIT_FLAG_PLANE;
			OutNormal = Planes[i].Normal;
			OutMaterial = Planes[i].Material;
		}
	}
	return HitFlag;
}

bool CastRay(RAY Ray, inout float3 Color, inout HITRECORD Record, int CurBounce, float2 RandSeed)
{
	float3 Normal;
	float T;
	MATERIAL Material;
	if (CastToWorld(Normal, T, Material, Ray) > 0)
	{
		Color *= Material.Color;
		Record.Material = Material;
		Record.Normal = normalize(Normal + (Material.Roughness * (rand(RandSeed) - 1.0f)));
		Record.Position = PosAtTime(Ray, T);
		return true;
	}
	Color += float3(0.1f, 0.1f, 0.1f);
	return false;
}

float4 PSMain(VS_OUTPUT Input) : SV_TARGET0
{
	float3 Color = float3(1.0f, 1.0f, 1.0f);

	RAY Ray = BeginRay(Input.Position.xy);

	float2 RandSeed = Input.Position * rand(-Input.Position * Frame);

	HITRECORD Record;
	for (int i = 0; i < 5; i++)
	{
		if (CastRay(Ray, Color, Record, i, RandSeed))
		{
			Ray = CreateRay(Record, Ray);
		}
		else
		{
			break;
		}
	}

	return float4(Color, 1.0f);
}

