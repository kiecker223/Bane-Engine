cbuffer TestBuffer : register(b0)
{
	float InMultiplier;
}

SamplerState Sampler : register(s0);
Texture2D Tex : register(t0);

struct VERTEX_INPUT
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
};

struct VERTEX_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

VERTEX_OUTPUT VSMain(VERTEX_INPUT Input)
{
	VERTEX_OUTPUT Res;
	Res.Position = float4(Input.Position, 1.0f);
	Res.UV = Input.UV;
	return Res;
}

float4 PSMain(VERTEX_OUTPUT Output) : SV_TARGET
{
	return Tex.Sample(Sampler, Output.UV);
}
