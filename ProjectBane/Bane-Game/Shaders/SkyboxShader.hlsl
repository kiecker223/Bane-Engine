Resources
{
	CAMERA_CONSTANTS

	TextureCube<float4> Skybox : register(t0);
	SamplerState Sampler : register(s0);
}

Pipeline
{
	DepthStencilState = 
	{
		DepthFunction = COMPARISON_FUNCTION_LESS_EQUAL;
	};
	RasterState =
	{
		Cull = true;
		IsCounterClockwiseForward = false;
	};
}


struct VS_INPUT
{
	float3 Position : POSITION;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float3 UVW : TEXCOORD;
};

PS_INPUT VSMain(VS_INPUT Input)
{
	PS_INPUT Result;
	Result.UVW = Input.Position;
	float3 Pos = Input.Position;
	float3 Position = mul(Pos, (float3x3)View);
	Position = mul(Position, (float3x3)Projection);
	Position = mul(Position, (float3x3)Model);
	Result.Position = float4(Position.xy, 1.0f, 1.0f);
	return Result;
}	

float4 PSMain(PS_INPUT Input) : SV_TARGET0
{
	return Skybox.Sample(Sampler, Input.UVW);
}


