

struct DIRECTIONAL_LIGHT
{
	float3 Direction;
	float Range;
	float3 Color;
	float Intensity;
};

struct POINT_LIGHT
{
	float3 Position;
	float Range;
	float3 Color;
	float Intensity;
};

struct SPOTLIGHT
{
	float3 Position;
	float Range;
	float3 Direction;
	float Intensity;
	float3 Color;
	float SpotAngle;
};

#define MAX_LIGHTS 30

Resources
{


	cbuffer LightData : register(b0)
	{
		float3            AmbientColor;
		float             Frame;
		float3            CameraPosition;
		float             Padding02;
		DIRECTIONAL_LIGHT DirectionalLights[MAX_LIGHTS];
		POINT_LIGHT       PointLights[MAX_LIGHTS];
		SPOTLIGHT         SpotLights[MAX_LIGHTS];
		int               NumDirectionalLights;
		int               NumPointLights;
		int               NumSpotLights;
		int               Padding03;
	}

	Texture2D<float4> AlbedoBuffer : register(t0);
	Texture2D<float4> NormalBuffer : register(t1);
	Texture2D<float4> PositionBuffer : register(t2);
	Texture2D<float4> ParameterBuffer : register(t3);
	TextureCube<float4> SkyboxTexture : register(t4);
	SamplerState PointSampler : register(s0);
	SamplerState SkySampler : register(s1);
}

Pipeline
{
	DepthEnable = false;
}


struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoords : TEXCOORDS;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORDS;
};

struct INPUTS
{
	float3 Albedo;
	float Specular;
	float3 Position;
	float3 Normal;
	float Roughness;
	float Metallic;
	float3 Tangent;
};

INPUTS GetInputs(float2 UV)
{
	INPUTS Result;
	float4 f0 = AlbedoBuffer.Sample(PointSampler, UV);
	Result.Albedo = f0.xyz;
	Result.Specular = f0.w;
	float4 f1 = NormalBuffer.Sample(PointSampler, UV);
	Result.Normal = f1.xyz;
	Result.Tangent.x = f1.w;
	float4 f2 = PositionBuffer.Sample(PointSampler, UV);
	Result.Position = f2.xyz;
	Result.Tangent.y = f2.w;
	float4 f3 = ParameterBuffer.Sample(PointSampler, UV);
	Result.Roughness = f3.x;
	Result.Metallic = f3.y;
	Result.Tangent.z = f3.w;
	return Result;
}

VS_OUTPUT VSMain(VS_INPUT Input)
{
	VS_OUTPUT Result;
	Result.Position = float4(Input.Position.xy, 1e-4, 1.0f);
	Result.UV = Input.TexCoords;
	return Result;
}


static const float PI = 3.1415926535897932384626433832795f;

float3 FresnelSchlick(float3 FresnelAtZero, float3 FresnelAt90, float u)
{
	return (FresnelAtZero + (FresnelAt90 - FresnelAtZero) * pow(1.0f - u, 5.0f));
}

float3 CalculateDirectionalLight(int InLightIdx, float3 InNormal, float3 InColor)
{
	DIRECTIONAL_LIGHT Light = DirectionalLights[InLightIdx];
	float Intensity = Light.Intensity;
	float3 RaysDir = -Light.Direction;
	float NDotL = max(dot(RaysDir, InNormal), 0.0f);
	float3 Strength = Light.Color * (NDotL * Intensity);
	return InColor * Strength;
}

float DisneyDiffuse(INPUTS Inputs, float3 RayDir, float3 EyeDir, float3 SurfPos, float3 Normal, float3 FresnelFactor)
{
	float3 HalfVec = normalize(RayDir + EyeDir);

	float NDotL = saturate(dot(Normal, RayDir));
	float LDotH = saturate(dot(RayDir, HalfVec));
	float NDotV = saturate(dot(Normal, EyeDir));

	float EnergyBias = lerp(0.0f, 0.5f, Inputs.Roughness);
	float EnergyFactor = lerp(1.0f, 1.0f / 1.51f, Inputs.Roughness);
	float FD90 = EnergyBias + 2.0f * (LDotH * LDotH) * Inputs.Roughness;

	float LightScatter = FresnelSchlick(FresnelFactor, FD90, NDotL);
	float ViewScatter = FresnelSchlick(FresnelFactor, FD90, NDotV);

	return LightScatter * ViewScatter * EnergyFactor;
}

float3 DisneyGGX(INPUTS Inputs, float3 RayDir, float3 EyeDir, float3 SurfPos, float3 Normal, float3 FresnelFactor)
{
	float3 HalfVec = normalize(RayDir + EyeDir);
	float NDotH = saturate(dot(Normal, HalfVec));

	float RoughnessPow2 = max(Inputs.Roughness * Inputs.Roughness, 2.0e-3f);
	float RoughnessPow4 = RoughnessPow2 * RoughnessPow2;

	float MinorDistribution = (NDotH * RoughnessPow4 - NDotH) * NDotH + 1.0f;
	float Distrubution = RoughnessPow4 / PI * (MinorDistribution * MinorDistribution);

	float3 Reflectivity = Inputs.Specular;
	float NDotL = saturate(dot(Normal, RayDir));
	float LDotH = saturate(dot(RayDir, HalfVec));
	float NDotV = saturate(dot(Normal, EyeDir));
	float3 Fresnel = Reflectivity + (Reflectivity) * exp2((-5.55473f * LDotH - 6.98316f) * LDotH);

	float K = RoughnessPow2 * 0.5f;
	float G_SmithL = NDotL * (1.0f - K) + K;
	float G_SmithV = NDotV * (1.0f - K) + K;
	float G = 0.25 / (G_SmithL * G_SmithV);

	return G * K * Fresnel;
}

float3 RadianceIBLIntegration(float NdotV, float roughness, float3 Specular)
{
	return Specular;
}

float3 IBL(INPUTS Inputs, float3 Normal, float3 EyeDir)
{
	// Note: Currently this function assumes a cube texture resolution of 1024x1024
	float NDotV = max(dot(Inputs.Normal, EyeDir), 0.0f);

	float3 ReflectionVector = normalize(reflect(-EyeDir, Inputs.Normal));
	float Smoothness = 1.0f - Inputs.Roughness;
	float MipLevel = (1.0f - Smoothness * Smoothness) * 10.0f;
	float4 CS = SkyboxTexture.SampleLevel(SkySampler, ReflectionVector, MipLevel);
	float3 Result = pow(CS.xyz, 2.2) * RadianceIBLIntegration(NDotV, Inputs.Roughness, Inputs.Specular);

	float3 DiffuseDominantDirection = Inputs.Normal;
	float DiffuseLowMip = 9.6;
	float3 DiffuseImageLighting = SkyboxTexture.SampleLevel(SkySampler, DiffuseDominantDirection, DiffuseLowMip).rgb;
	DiffuseImageLighting = pow(DiffuseImageLighting, 2.2);

	return Result + DiffuseImageLighting * Inputs.Albedo.rgb;
}

void FinalGamma(inout float3 FinalColor)
{
	FinalColor = pow(FinalColor, 1.f / 2.2f);
}

float3 CalculateSkyboxReflection(float3 InNormal, float3 InCameraPosition, float3 Position)
{
	float3 IncidinceAngle = normalize(Position - InCameraPosition);
	float3 Reflection = reflect(IncidinceAngle, normalize(InNormal));
	return SkyboxTexture.SampleLevel(SkySampler, Reflection, ((sin(Frame / 100.0f) * 0.5) + 0.5) * 11);
}
float3 CalculateAttenuation(float Range, float Dist)
{
	return (saturate((1 / ((Dist * Dist) / Range)) - 0.002f));
}

float4 PSMain(VS_OUTPUT Input) : SV_TARGET0
{
	INPUTS Inputs = GetInputs(Input.UV);
	Inputs.Roughness = 0.5f; //(sin(Frame / 20) * 0.5) + 0.5;
	Inputs.Metallic = 0.5f;// (sin(Frame / 10) * 0.5) + 0.5;
	Inputs.Specular = 1 - Inputs.Roughness;// (sin(Frame / 20) * 0.5) + 0.5;
	float3 Color = float3(0.0f, 0.0f, 0.0f);
	
	if (length(Inputs.Normal) < 1e-3)
	{
		return float4(Inputs.Albedo, 1.0f);
	}
	
	Inputs.Normal = normalize(Inputs.Normal);
	
	float3 FresnelFactor = float3(0.04, 0.04, 0.04);
	FresnelFactor = lerp(Inputs.Albedo, FresnelFactor, Inputs.Metallic);
	
	float3 EyeDir = normalize(CameraPosition - Inputs.Position);
    
    int i = 0;
	float3 Specular = (0.0f, 0.0f, 0.0f);
    for (i = 0; i < NumDirectionalLights; i++)
    {
		DIRECTIONAL_LIGHT Light = DirectionalLights[i];
		float3 RayDir = normalize(Light.Direction);
		float NDotL = saturate(dot(Inputs.Normal, RayDir));
		float NDotV = saturate(dot(Inputs.Normal, EyeDir));
		Color += NDotL * DisneyDiffuse(Inputs, RayDir, EyeDir, Inputs.Position, Inputs.Normal, FresnelFactor) * Light.Color * Light.Intensity;
		Specular += NDotL * DisneyGGX(Inputs, RayDir, EyeDir, Inputs.Position, Inputs.Normal, FresnelFactor) * Light.Color * Light.Intensity;
    }

	for (i = 0; i < NumPointLights; i++)
	{
	//	POINT_LIGHT Light = PointLights[i];
	//	float Attenuation = CalculateAttenuation(Light.Range, Inputs.Position);
	//	float3 RayDir = normalize(Light.Position - Inputs.Position);
	//	float NDotL = saturate(dot(Inputs.Normal, RayDir));
	//	float NDotV = saturate(dot(Inputs.Normal, EyeDir));
    }
	float3 FinalColor = Inputs.Albedo * Color + (Specular * IBL(Inputs, Inputs.Normal, EyeDir)); 
	FinalGamma(FinalColor);
	return float4(FinalColor, 1.0f);
}

