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
	float Range;
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
	cbuffer CameraInfo : register(b0)
	{
		float4 CameraPosition;
		float4 CameraForward;
	}

	cbuffer ScreenData : register(b1)
	{
		float2 ScreenDimensions;
		float Frame;
		int RayBounces;
	}

	cbuffer GeometryData : register(b2)
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

#define MAX_FLOAT 3.402823466e+38F
#define M_PI 3.14159265358979323846
#define PI 3.141592654

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
	Result.Position = float4(In.Position, 0.0f, 1.0f);
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
	MATERIAL FirstBounceMaterial;
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
	float T = (-B - sqrt(Discriminate)) / (2 * A);
	if (T < MAX_FLOAT && T > 1e-3)
	{
		return T;
	}
	T = (-B + sqrt(Discriminate)) / (2 * A);
	if (T < MAX_FLOAT && T > 1e-3)
	{
		return T;
	}
	return -1.0f;
}

float HitPlane(PLANE Plane, RAY Ray)
{
	float RDotPlaneNormal = dot(Plane.Normal, Ray.Dir);
	if (abs(RDotPlaneNormal) > 1e-3)
	{
		float T = dot((Plane.Position - Ray.Pos), Plane.Normal) / RDotPlaneNormal;
		if (T < 1e-3)
		{
			return -1.0f;
		}
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
	RAY Result;
	Result.Pos = CameraPosition.xyz;

	float FOV = radians(55.0f);
	float HalfHeight = tan(FOV / 2.0f);
	float HalfWidth = tan(FOV / 2.0f) * (ScreenDimensions.x / ScreenDimensions.y);
	float3 Up = float3(0, 1, 0);
	float3 Right = cross(CameraForward.xyz, Up);

	Result.Dir = normalize(CameraForward.xyz + (HalfHeight * PixelCoord.y * Up) + (HalfWidth * PixelCoord.x * Right));
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

float Attenuate(float Range, float Dist)
{
	return saturate(((1 / ((Dist * Dist) / Range)) - 0.002f));
}

// Returns the visibility of the light
float CastShadowRay(HITRECORD Record)
{
	RAY Ray;
	Ray.Pos = Record.Position;
	float Brightness = 0.0;

	for (int i = 0; i < 1; i++)
	{
		float3 ShadowRayDir = normalize(PointLights[i].Position - Record.Position);
		Ray.Dir = ShadowRayDir;

		// Unused
		float3 Normal;
		MATERIAL Mat;
		float T;
		// -----

		if (CastToWorld(Normal, T, Mat, Ray) == 0 && T > 1e-3)
		{
			// If we didn't intersect calculate attenuation
			Brightness += Attenuate(PointLights[i].Range, length(PointLights[i].Position - Record.Position));
		}
	}
	return Brightness;
}

bool CastRay(RAY Ray, inout float3 Color, inout HITRECORD Record, int CurBounce, float2 RandSeed)
{
	float3 Normal;
	float T;
	MATERIAL Material;
	if (CastToWorld(Normal, T, Material, Ray) > 0)
	{
		if (CurBounce == 0)
		{
			Record.FirstBounceMaterial = Material;
			Color = Material.Color;
		}
		if (CurBounce >= 1)
		{
			// Hopefully this will give an average closer to the original color
			Color *= lerp(Record.FirstBounceMaterial.Color, Material.Color * Record.FirstBounceMaterial.Color, 1 - Record.FirstBounceMaterial.Roughness);
		}
		Record.Material = Material;
		Record.Normal = normalize(Normal + (Material.Roughness * (rand(RandSeed) - 1.0f)));
		Record.Position = PosAtTime(Ray, T);
		Color *= CastShadowRay(Record);  
		return true;
	}
	//Color += float3(0.1f, 0.1f, 0.1f);
	return false;
}

float4 PSMain(VS_OUTPUT Input) : SV_TARGET0
{
	float3 Color = float3(0.1f, 0.1f, 0.1f);
	
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

