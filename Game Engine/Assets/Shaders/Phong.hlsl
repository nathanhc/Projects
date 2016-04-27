#include "Constants.hlsl"

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	float3 mPos : POSITION;
	float3 mNorm : NORMAL;
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

	// lab 2
	output.mPos = mul(mul(float4(input.mPos, 1.0f), worldMatrix), viewMatrix);
	output.mTex = input.mTex;
	
	// lab 3
	output.mWPos = mul(float4(input.mPos, 1.0f), worldMatrix).xyz;
	output.mWNorm = mul(float4(input.mNorm, 0.0f), worldMatrix).xyz;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	// Look at slide 19
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
