// We want to use row major matrices
#pragma pack_matrix(row_major)

cbuffer PerCameraConstants : register ( b0 )
{
	float4x4 viewMatrix;
	float3 worldCameraPos;
};

cbuffer PerObjectConstants : register (b1)
{
	float4x4 worldMatrix;
};

struct PointLightData
{
	float3 diffuseColor;
	float3 specularColor;
	float3 position;
	float specPower;
	float innerRad;
	float outerRad;
	bool enabled;
};

cbuffer LightingConstants : register (b2)
{
	float3 ambientLight;
	PointLightData PointLights[8];
};

cbuffer PostConstants : register (b4)
{
	float movementx;
	float movementy;
	bool ray;
	bool toon;
	bool neg;
	bool pix;
	bool rad;
	bool blur;
	bool bloom;
	bool depth;
	float pad1;
	float pad2;
	float4x4 currMatrix;
	float4x4 prevMatrix;
}	

SamplerState DefaultSampler : register(s0); 
Texture2D DiffuseTexture : register(t0);
Texture2D DepthBuffer : register(t1);