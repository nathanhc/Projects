#include "ITPEnginePCH.h"
#include <SDL/SDL_log.h>

Shader::Shader(class Game& game)
	:Asset(game)
	,mGraphicsDriver(mGame.GetRenderer().GetGraphicsDriver())
{
}

Shader::~Shader()
{
}

void Shader::SetActive()
{
	// Set this shader as active, and set
	// any constant buffers
	mGraphicsDriver.SetVertexShader(mVertexShader);
	mGraphicsDriver.SetPixelShader(mPixelShader);

	mGraphicsDriver.SetVSConstantBuffer(mPerCameraBuffer, 0);
	mGraphicsDriver.SetPSConstantBuffer(mPerCameraBuffer, 0);
	mGraphicsDriver.SetVSConstantBuffer(mPerObjectBuffer, 1);

	mGraphicsDriver.SetPSSamplerState(mDefaultSampler, 0);

	// Set the lighting constant buffer	
	mGraphicsDriver.SetPSConstantBuffer(mLightingBuffer, 2);

	mGraphicsDriver.SetPSConstantBuffer(mPostBuffer, 4);

	// Set the matrix palette buffer, if it exists
	if (mMatrixPaletteBuffer != nullptr)
	{
		mGraphicsDriver.SetVSConstantBuffer(mMatrixPaletteBuffer, 3);
	}
}

void Shader::CreateMatrixPaletteBuffer()
{
	mMatrixPaletteBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mMatrixPalette, sizeof(mMatrixPalette), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);

	mGraphicsDriver.SetVSConstantBuffer(mMatrixPaletteBuffer, 3);
}

void Shader::UploadPerCameraConstants()
{
	memcpy(mGraphicsDriver.MapBuffer(mPerCameraBuffer), &mPerCamera, sizeof(PerCameraConstants));
	mGraphicsDriver.UnmapBuffer(mPerCameraBuffer);
}

void Shader::UploadPerObjectConstants()
{
	memcpy(mGraphicsDriver.MapBuffer(mPerObjectBuffer), &mPerObject, sizeof(PerObjectConstants));
	mGraphicsDriver.UnmapBuffer(mPerObjectBuffer);
}

void Shader::UploadLightingConstants()
{
	memcpy(mGraphicsDriver.MapBuffer(mLightingBuffer), &mLighting, sizeof(LightingConstants));
	mGraphicsDriver.UnmapBuffer(mLightingBuffer);
}

void Shader::UploadPostConstants()
{
	memcpy(mGraphicsDriver.MapBuffer(mPostBuffer), &mPost, sizeof(PostConstants));
	mGraphicsDriver.UnmapBuffer(mPostBuffer);
}

void Shader::UploadMatrixPalette(const struct MatrixPalette& palette)
{
	memcpy(mGraphicsDriver.MapBuffer(mMatrixPaletteBuffer), &palette, sizeof(mMatrixPalette));
	mGraphicsDriver.UnmapBuffer(mMatrixPaletteBuffer);
}

void Shader::BindTexture(TexturePtr texture, int slot)
{
	texture->SetActive(slot);
}

bool Shader::Load(const char* fileName, class AssetCache* cache)
{
	if (mGraphicsDriver.CompileShaderFromFile(fileName, "VS", "vs_4_0", mCompiledVS))
	{
		mVertexShader = mGraphicsDriver.CreateVertexShader(mCompiledVS);
		mGraphicsDriver.SetVertexShader(mVertexShader);
	}
	else 
	{
		return false;
	}
	if (mGraphicsDriver.CompileShaderFromFile(fileName, "PS", "ps_4_0", mCompiledPS))
	{
		mPixelShader = mGraphicsDriver.CreatePixelShader(mCompiledPS);
		mGraphicsDriver.SetPixelShader(mPixelShader);
	}
	else
	{
		return false;
	}
	
	// creating buffers
	mPerCameraBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mPerCamera, sizeof(PerCameraConstants), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);
	mPerObjectBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mPerObject, sizeof(PerObjectConstants), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);
	mLightingBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mLighting, sizeof(LightingConstants), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);
	mPostBuffer = mGraphicsDriver.CreateGraphicsBuffer(&mPost, sizeof(PostConstants), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);

	mGraphicsDriver.SetVSConstantBuffer(mPerCameraBuffer, 0);
	mGraphicsDriver.SetPSConstantBuffer(mPerCameraBuffer, 0);
	mGraphicsDriver.SetVSConstantBuffer(mPerObjectBuffer, 1);
	mGraphicsDriver.SetPSConstantBuffer(mLightingBuffer, 2);
	mGraphicsDriver.SetPSConstantBuffer(mPostBuffer, 4);

	mDefaultSampler = mGraphicsDriver.CreateSamplerState();
	mGraphicsDriver.SetPSSamplerState(mDefaultSampler, 0);

	return true;
}
