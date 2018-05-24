Resources
{
	CAMERA_CONSTANTS

	cbuffer MaterialParameters : register(b1)
	{
		float3 MatColor;
		float MatSpecularFactor;
		float2 P0;
		float MatRoughness;
		float Metallic;
	}

	Texture2D<float3> AlbedoTexture : register(t0);
	Texture2D<float3> NormalTexture : register(t1);
	Texture2D<float> SpecularMap : register(t2);
	SamplerState DefaultSampler : register(s0);
};

Pipeline 
{
	// Color buffer
	RenderTarget_0 =  
	{
		Format = FORMAT_R8G8B8A8_UNORM;
	};
	// Position / Normal buffer	
	RenderTarget_1, RenderTarget_2, RenderTarget_3 = 
	{
		Format = FORMAT_R32G32B32A32_FLOAT;
	};

	RasterState = 
	{
		Cull = false;
	};
};

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Binormal : BINORMAL;
	float3 Tangent : TANGENT;
	float2 TexCoords : TEXCOORD;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL;
	float3 Binormal : BINORMAL;
	float3 Tangent : TANGENT;
	float2 TexCoords : TEXCOORD;
};

PS_INPUT VSMain(VS_INPUT Input)
{
	PS_INPUT Output;
	Output.TexCoords = Input.TexCoords;
	float4 Pos = float4(Input.Position, 1.0f);
	Pos = mul(Pos, Model);
	Pos = mul(Pos, View);
	Pos = mul(Pos, Projection);
	Output.Position = Pos;
	Output.WorldPos = float3(mul(float4(Input.Position, 1.0f), Model).xyz);
	Output.Normal =   normalize(mul(Input.Normal, (float3x3)Model));
	float3 Binormal = normalize(mul(Input.Binormal, (float3x3)Model));
	float3 Tangent =  normalize(mul(Input.Tangent, (float3x3)Model));
	Output.Binormal = Binormal;
	Output.Tangent = Tangent;

	return Output;
}

struct PS_OUTPUT
{
	float4 ColorBuffer : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Position : SV_TARGET2;
	float4 Parameters : SV_TARGET3;
};


PS_OUTPUT PSMain(PS_INPUT Input)
{
	PS_OUTPUT Output;
	Output.ColorBuffer = float4(MatColor * AlbedoTexture.Sample(DefaultSampler, Input.TexCoords), 0.0f);
	Output.ColorBuffer.w = saturate(SpecularMap.Sample(DefaultSampler, Input.TexCoords));
	Output.Position = float4(Input.WorldPos, Input.Tangent.x);

	float3x3 TBN = (float3x3(Input.Tangent, Input.Binormal, Input.Normal));
	Output.Normal = float4(mul(NormalTexture.Sample(DefaultSampler, Input.TexCoords) * 2.0f - 1.0f, TBN), Input.Tangent.y);
	Output.Parameters = float4(MatRoughness, Metallic, 1.0f, Input.Tangent.z);
	return Output;
}

