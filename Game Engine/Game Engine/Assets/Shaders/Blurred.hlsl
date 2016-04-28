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
	int NUM_SAMPLES = 4;

	// Get the depth buffer value at this pixel.
	float zOverW = DepthBuffer.Sample(DefaultSampler, input.mTex).w;
	// H is the viewport position at this pixel in the range -1 to 1.
	float4 H = float4(input.mTex.x * 2 - 1, (1 - input.mTex.y) * 2 - 1, zOverW, 1);
	// Transform by the view-projection inverse.
	float4 D = mul(H, viewMatrix);
	// Divide by w to get the world position.
	float4 worldPos = D / D.w;

	// Current viewport position
	float4 currentPos = H;
	// Use the world position, and transform by the previous view-
	// projection matrix.
	float4 previousPos = mul(input.mPos, prevMatrix);
	// Convert to nonhomogeneous points [-1,1] by dividing by w.
	previousPos /= previousPos.w;
	// Use this frame's position and last frame's to compute the pixel
	// velocity.
	float2 velocity = (currentPos - previousPos) / 2.0f;

	// Get the initial color at this pixel.
	float4 color = DiffuseTexture.Sample(DefaultSampler, input.mTex);
	input.mTex += velocity;
	for (int i = 1; i < NUM_SAMPLES; ++i, input.mTex += velocity)
	{
		// Sample the color buffer along the velocity vector.
		float4 currentColor = DiffuseTexture.Sample(DefaultSampler, input.mTex);
		// Add the current color to our color sum.
		color += currentColor;
	}
	// Average all of the samples to get the final blur color.
	float4 finalColor = color / NUM_SAMPLES;
	return finalColor;
}