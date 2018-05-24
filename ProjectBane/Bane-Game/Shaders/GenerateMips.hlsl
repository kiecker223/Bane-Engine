RWTexture2D<float4> DestTexture : register(u0);
Texture2D<float4> SrcTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer TexInfo : register(b0)
{
	float4 TexelSize;
}


[numthreads(8, 8, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	float2 TexCoords = TexelSize.xy * (DTid.xy + 0.5);
	DestTexture[DTid.xy] = SrcTexture.SampleLevel(Sampler, TexCoords, 0);
}

