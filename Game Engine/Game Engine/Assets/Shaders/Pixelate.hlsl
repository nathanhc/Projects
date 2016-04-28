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
	float downScaleW = 13.0f;
	float downScaleH = 13.0f;

	input.mTex.xy *= float2(2400, 1600);

	float2 pixelatedUV;
	pixelatedUV.x = round(input.mTex.x / downScaleW) * downScaleW;
	pixelatedUV.y = round(input.mTex.y / downScaleH) * downScaleH;

	pixelatedUV.xy /= float2(2400, 1600);

	return DiffuseTexture.Sample(DefaultSampler, pixelatedUV);  
}