// Shader.h
// Shader asset that encapsulates both the vertex
// and fragment shaders

#pragma once
#include "Asset.h"
#include "Math.h"
#include "Texture.h"
#include "GraphicsDriver.h"
#include "PointLightData.h"

class Shader : public Asset
{
	DECL_ASSET(Shader, Asset);
public:
	struct PerCameraConstants
	{
		Matrix4 viewMatrix;
		Vector3 worldCameraPos;
		float padding;
	};

	struct PerObjectConstants
	{
		Matrix4 worldMatrix;
	};

	struct LightingConstants
	{
		Vector3 ambientLight;
		float padding;

		PointLightData PointLights[MAX_POINT_LIGHTS];
	};

	struct PostConstants
	{
		float movementx;
		float movementy;
		unsigned int ray;
		unsigned int toon;
		unsigned int neg;
		unsigned int pix;
		unsigned int rad;
		unsigned int blur;
		unsigned int bloom;
		unsigned int depth;
		float pad1;
		float pad2;
		Matrix4 currMatrix;
		Matrix4 prevMatrix;
	};

	Shader(class Game& game);
	virtual ~Shader();

	void SetActive();

	PerCameraConstants& GetPerCameraConstants() { return mPerCamera; }
	PerObjectConstants& GetPerObjectConstants() { return mPerObject; }
	LightingConstants& GetLightingConstants() { return mLighting; }
	PostConstants& GetPostConstants() { return mPost; }

	void CreateMatrixPaletteBuffer();

	void UploadPerCameraConstants();
	void UploadPerObjectConstants();
	void UploadLightingConstants();
	void UploadMatrixPalette(const struct MatrixPalette& palette);

	// Bind the specified texture to the specified slot
	void BindTexture(TexturePtr texture, int slot);

	const std::vector<char>& GetCompiledVS() const { return mCompiledVS; }
	const std::vector<char>& GetCompiledPS() const { return mCompiledPS; }

	void UploadPostConstants();

	void ToggleShader()
	{
		mEnabled = !mEnabled;
	}
	bool IsOn()
	{
		return mEnabled;
	}
	void SetOn(bool enabled)
	{
		mEnabled = enabled;
	}

protected:
	bool Load(const char* fileName, class AssetCache* cache) override;
private:
	PerCameraConstants mPerCamera;
	PerObjectConstants mPerObject;
	LightingConstants mLighting;
	PostConstants mPost;

	GraphicsBufferPtr mPerCameraBuffer;
	GraphicsBufferPtr mPerObjectBuffer;
	GraphicsBufferPtr mLightingBuffer;
	GraphicsBufferPtr mMatrixPaletteBuffer;
	GraphicsBufferPtr mPostBuffer;

	MatrixPalette mMatrixPalette;

	VertexShaderPtr mVertexShader;
	PixelShaderPtr mPixelShader;

	SamplerStatePtr mDefaultSampler;

	std::vector<char> mCompiledVS;
	std::vector<char> mCompiledPS;

	bool mEnabled;

	GraphicsDriver& mGraphicsDriver;
};

DECL_PTR(Shader);
