#include "Constants.hlsl"

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	float3 mPos : POSITION;
	float2 mTex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 mPos : SV_POSITION;
	float2 mTex: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.mPos = mul(mul(float4(input.mPos, 1.0f), worldMatrix), viewMatrix);
	output.mTex = input.mTex;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	/*float threshold = 4.0f;
	float4 fCenter = DiffuseTexture.Sample(DefaultSampler, input.mTex);
	float4x4 fEdges = {
		DiffuseTexture.Sample(DefaultSampler, input.mTex, int2(-1, 0)),
		DiffuseTexture.Sample(DefaultSampler, input.mTex, int2(0, -1)),
		DiffuseTexture.Sample(DefaultSampler, input.mTex, int2(1, 0)),
		DiffuseTexture.Sample(DefaultSampler, input.mTex, int2(0, 1)) // Line 11
	};

	float4 delta = abs(fCenter.xxxx - fEdges);
	float4 edges = step(threshold / 10.0, delta);

	if (dot(edges, 1.0) == 0.0)
	discard;

	return edges;*/
	return DiffuseTexture.Sample(DefaultSampler, input.mTex);
}