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
	if (!depth && !bloom && ray && !neg && !pix && !blur && !rad)
	{
		int NUM_SAMPLES = 150;
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
	else if (!depth && !bloom && !ray && !neg && !pix && !blur && rad)
	{
		float2 Center = { 0.5, 0.5 }; ///center of the screen (could be any place)
		float BlurStart = 1.0f; /// blur offset
		float BlurWidth = -0.1; ///how big it should be
		int nsamples = 20;

		nsamples = nsamples * ((movementx + movementy) / 2);

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
	else if (!depth && !bloom && !ray && neg && !pix && !blur && !rad)
	{
		float4 color;
		color = DiffuseTexture.Sample(DefaultSampler, input.mTex);
		float4 invert = float4(color.a - color.rgb, color.a);
		return invert;
	}
	else if (!ray && !neg && pix && !blur && !rad)
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
	else if (!depth && !bloom && !ray && !neg && !pix && blur && !rad)
	{
		int NUM_SAMPLES = 4;

		// Get the depth buffer value at this pixel.
		float zOverW = DepthBuffer.Sample(DefaultSampler, input.mTex).r;
		//return zOverW;
		// H is the viewport position at this pixel in the range -1 to 1.
		float4 H = float4(input.mTex.x * 2 - 1, (1 - input.mTex.y) * 2 - 1, zOverW, 1);
		// Transform by the view-projection inverse.
		float4 D = mul(H, currMatrix);
		// Divide by w to get the world position.
		float4 worldPos = D / D.w;

		// Current viewport position
		float4 currentPos = H;
		// Use the world position, and transform by the previous view-
		// projection matrix.
		float4 previousPos = mul(worldPos, prevMatrix);
		// Convert to nonhomogeneous points [-1,1] by dividing by w.
		previousPos /= previousPos.w;
		// Use this frame's position and last frame's to compute the pixel
		// velocity.
		float2 velocity = (currentPos - previousPos) / 2.0f;

		// Get the initial color at this pixel.
		float2 tex = input.mTex;
		float4 color = DiffuseTexture.Sample(DefaultSampler, tex);
		tex += velocity;
		for (int i = 1; i < NUM_SAMPLES; ++i, tex += velocity)
		{
			// Sample the color buffer along the velocity vector.
			float4 currentColor = DiffuseTexture.Sample(DefaultSampler, tex);
			// Add the current color to our color sum.
			color += currentColor;
		}
		// Average all of the samples to get the final blur color.
		float4 finalColor = color / NUM_SAMPLES;
		return finalColor;
	}
	else if (!depth && bloom && !ray && !neg && !pix && !blur && !rad)
	{
		int NUM_SAMPLES = 4;
		int p = 2;
		float attenuation = 0.95f;
		float2 pxSize = float2(0.05f, 0.05f);
		float2 dir[4] = { float2(1.0f, 0.0f), float2(0.0f, 1.0f), float2(-1.0f, 0.0f), float2(0.0f, -1.0f) };

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
	else if (depth && !bloom && !ray && !neg && !pix && !blur && !rad)
	{
		return DepthBuffer.Sample(DefaultSampler, input.mTex).r;
	}
	else
	{
		return DiffuseTexture.Sample(DefaultSampler, input.mTex);
	}
}