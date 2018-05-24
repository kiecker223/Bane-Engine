

Resources
{
	cbuffer ScreenData : register(b0)
	{
		float2 ScreenDimensions;
		float Frame;
	}
}

struct VS_INPUT
{
	float3 Position : POSITION;
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
	Result.SystemPosition = float4(In.Position.xy, 0.0f, 1.0f);
	Result.Position = Result.SystemPosition;
	Result.TexCoords = In.TexCoords;
	return Result;
}


//float3 LowerLeftCorner;
//float3 Horizontal;
//float3 Vertical;

struct RAY
{
	float3 Pos;
	float3 Dir;
};

struct SPHERE
{
	float3 Center;
	float Radius;
};

struct HITRECORD
{
	float3 LastNormal;
	float3 LastPosition;
};

struct PLANE
{
	float3 Position;
	float Size;
	float3 Normal;
};

#define MAX_SPHERES 30


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

float3 PosAtTime(RAY InRay, float T)
{
	return (InRay.Pos + (InRay.Dir + T));
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
	float AngleX = HalfFovX * (PixelCoord.x);
	float AngleY = HalfFovY * (PixelCoord.y);
	float CosAngleX = cos(radians(AngleX));
	float SinAngleX = sin(radians(AngleX));
	float CosAngleY = cos(radians(AngleY));
	float SinAngleY = sin(radians(AngleY));

	float3x3 RotX = float3x3(
		1, 0, 0,
		0, CosAngleY, -SinAngleY,
		0, SinAngleY, CosAngleY
	);
	float3x3 RotY = float3x3(
		CosAngleX, 0, SinAngleX,
		0.0f, 1.0f, 0.0f,
		-SinAngleX, 0.0f, CosAngleX
	);

	Result.Dir = CameraForward;
	Result.Dir = mul(Result.Dir, RotX);
	Result.Dir = mul(Result.Dir, RotY);
	Result.Dir = normalize(Result.Dir);
	return Result;
}

float4 PSMain(VS_OUTPUT Input) : SV_TARGET0
{
	float3 Color = float3(1.0f, 1.0f, 1.0f);

	RAY Ray = BeginRay(Input.Position.xy);
	Color = Ray.Dir * -1;
	int NumSpheres = 1;

	SPHERE Spheres[MAX_SPHERES];

	Spheres[0].Center = float3(0.0f, 0.0f, -2);// -((sin(Frame / 100) * 8) * 0.5 + 4));
	Spheres[0].Radius = 0.5f;

	Spheres[1].Center = float3(0.0f, -50.0f, -7.0f);
	Spheres[1].Radius = 50.0f;

	//PLANE Plane;
	//Plane.Position = float3(0.0f, -0.5f, -7.0f);
	//Plane.Normal = float3(0.0f, 1.0f, 0.0f);

	
	for (int i = 0; i < 2; i++)
	{
		float T = HitSphere(Spheres[i], Ray);
		if (T > 0)
		{
			//float3 PositionOfSurface = PosAtTime(Ray, T);
			//float3 Normal = normalize(Spheres[0].Center - PositionOfSurface);
			//Color = -1 * Normal;
			Color = float3(1.0f, 0.0f, 0.0f);
		}
	}
	//T = HitPlane(Plane, Ray);
	//if (T > 0)
	//{
	//	float3 PositionOfSurface = PosAtTime(Ray, T);
	//	Color = float3(1.0f, 1.0f, 1.0f);
	//}
	
	return float4(Color, 1.0f);
}

