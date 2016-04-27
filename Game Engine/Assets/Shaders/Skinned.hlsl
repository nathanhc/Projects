#include "Constants.hlsl"

cbuffer SkinningConstants : register (b3)
{
	float4x4 skinMatrices[96];
};

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	float3 mPos : POSITION;
	float3 mNorm : NORMAL;
	uint4 mBoneIndices : BLENDINDICES0;
	float4 mBoneWeights : BLENDWEIGHTS0;
	float2 mTex : TEXCOORD0;	
};

struct PS_INPUT
{
	float4 mPos : SV_POSITION;
	float2 mTex : TEXCOORD0;
	float3 mWPos : POSITION;
	float3 mWNorm : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// Skinning computations
	float4 result = input.mBoneWeights.x * mul(float4(input.mPos, 1.0f), skinMatrices[input.mBoneIndices.x])
		+ input.mBoneWeights.y * mul(float4(input.mPos, 1.0f), skinMatrices[input.mBoneIndices.y])
		+ input.mBoneWeights.z * mul(float4(input.mPos, 1.0f), skinMatrices[input.mBoneIndices.z])
		+ input.mBoneWeights.w * mul(float4(input.mPos, 1.0f), skinMatrices[input.mBoneIndices.w]);

	// lab 2
	output.mPos = mul(mul(result, worldMatrix), viewMatrix);
	output.mTex = input.mTex;

	// lab 3
	output.mWPos = mul(result, worldMatrix).xyz;
	output.mWNorm = mul(float4(input.mNorm, 0.0f), worldMatrix).xyz;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	// Look at slide 19
	if (!toon)
		{
		float3 NVector = normalize(input.mWNorm);
		float3 Phong = ambientLight;
		for (int i = 0; i < 8; i++)
		{
			// Pointlight isn't enabled 
			if (PointLights[i].enabled == false)
			{
				continue;
			}
			else
			{
				float3 LVector = normalize(PointLights[i].position - input.mWPos);
				if (dot(NVector, LVector) > 0)
				{
				// Diffuse Light Stuff
					float dist = distance(PointLights[i].position, input.mWPos);
					float smooth = smoothstep(PointLights[i].innerRad, PointLights[i].outerRad, dist);
					Phong += lerp(PointLights[i].diffuseColor, float3(0.0f, 0.0f, 0.0f), smooth) * dot(NVector, LVector);

					// Specular Light Stuff
					float3 VVector = normalize(worldCameraPos - input.mWPos);
					float3 RVector = reflect(-LVector, NVector);
					Phong += lerp(PointLights[i].specularColor, float3(0.0f, 0.0f, 0.0f), smooth) * pow(max(0.0f, dot(RVector, VVector)), PointLights[i].specPower);
				}
			}
		}
		Phong = saturate(Phong);
		return DiffuseTexture.Sample(DefaultSampler, input.mTex) * float4(Phong, 1.0f);
	}
	else
	{
		float3 Nvector = normalize(input.mWNorm);
		float4 color;
		for (int i = 0; i < 8; i++)
		{
			float3 Lvector = normalize(PointLights[i].position - input.mWPos);

			float4 DiffuseColor = float4(1, 1, 1, 1);
			float DiffuseIntensity = 1.0;

			float intensity = dot(Nvector, Lvector);
			if (intensity < 0)
				intensity = 0;

			// Calculate what would normally be the final color, including texturing and diffuse lighting
			color = DiffuseTexture.Sample(DefaultSampler, input.mTex) * DiffuseColor * DiffuseIntensity;
			color.a = 1;

			// Discretize the intensity, based on a few cutoff points
			if (intensity > 0.95)
				color = float4(1.0, 1, 1, 1.0) * color;
			else if (intensity > 0.5)
				color = float4(0.7, 0.7, 0.7, 1.0) * color;
			else if (intensity > 0.05)
				color = float4(0.35, 0.35, 0.35, 1.0) * color;
			else
				color = float4(0.1, 0.1, 0.1, 1.0) * color;

			
		}
		return color;
	}
}
