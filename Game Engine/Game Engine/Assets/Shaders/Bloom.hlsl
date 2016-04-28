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
	if (ray)
	{
		int NUM_SAMPLES = 250;
		float Density = 0.5f;
		float Weight = 0.4f;
		float Decay = 0.9f;
		float Exposure = 0.4f;

		// Calculate vector from pixel to light source in screen space.  
		half2 deltaTexCoord = (input.mTex - ambientLight.xy);
		// Divide by number of samples and scale by control factor.  
		deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;
		// Store initial sample.  
		half3 color = DiffuseTexture.Sample(DefaultSampler, input.mTex);
		// Set up illumination decay factor.  
		half illuminationDecay = 1.0f;
		// Evaluate summation from Equation 3 NUM_SAMPLES iterations.  
		for (int i = 0; i < NUM_SAMPLES; i++)
		{
			// Step sample location along ray.  
			input.mTex -= deltaTexCoord;
			// Retrieve sample at new location.  
			half3 sample = DiffuseTexture.Sample(DefaultSampler, input.mTex);
			// Apply sample attenuation scale/decay factors.  
			sample *= illuminationDecay * Weight;
			// Accumulate combined color.  
			color += sample;
			// Update exponential decay factor.  
			illuminationDecay *= Decay;
		}
		// Output final color with a further scale control factor.  
		return float4(color * Exposure, 1);
	}
	else
	{
		int NUM_SAMPLES = 4;
		int p = 2;
		float attenuation = 0.95f;
		float2 pxSize = float2(1.0f, 1.0f);
		float2 dir[4] = { float2(1.0f, 0.0f), float2(0.0f, 1.0f), float2(-1.0f, 0.0f), float2(0.0f, -1.0f)};

		float2 sampleCoord = 0;
		float4 cOut = 0;

		// sample weight = a^(b*s)
		// a = attenuation
		// b = 4^(pass-1)
		// s = sample number
		float b = pow(NUM_SAMPLES, p);
		for (int s = 0; s < NUM_SAMPLES; s++)
		{
			float weight = pow(attenuation, b * s);
			// dir = per-pixel, 2D orientation vector
			sampleCoord = input.mTex + (dir[s] * b * float2(s, s) * pxSize);
			cOut += saturate(weight) * DiffuseTexture.Sample(DefaultSampler, input.mTex);
		}
		return saturate(cOut);
	}
}