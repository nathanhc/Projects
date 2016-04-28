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
	float2 Center = { 0.5, 0.5 }; ///center of the screen (could be any place)
	float BlurStart = 1.0f; /// blur offset
	float BlurWidth = -0.15; ///how big it should be
	int nsamples = 15;

	nsamples = nsamples * ((movementx + movementy) / 2) ;
	
	if (nsamples == 0)
	{
		return DiffuseTexture.Sample(DefaultSampler, input.mTex);
	}

	input.mTex -= Center;
	float4 c = 0;
	for (int i = 0; i < nsamples; i++) {
		float scale = BlurStart + BlurWidth*(i / (float)(nsamples - 1));
		c += DiffuseTexture.Sample(DefaultSampler, input.mTex * scale + Center);
	}
	c /= nsamples;
	return c;
}