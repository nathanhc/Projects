#include "ITPEnginePCH.h"

Renderer::Renderer(Game& game)
	:mGame(game)
	,mWindow(nullptr)
	,mWidth(0)
	,mHeight(0)
{

}

Renderer::~Renderer()
{
	// Clear components...
	mDrawComponents.clear();
	mComponents2D.clear();

	mDepthBuffer.reset();
	mSpriteDepthState.reset();
	mMeshDepthState.reset();
	mRasterState.reset();

	mSpriteBlendState.reset();
	mMeshBlendState.reset();

	mSpriteShader.reset();
	mSpriteVerts.reset();

	mMeshShaders.clear();

	mPostProcessShaders.clear();

	// Shutdown the input cache and graphics driver
	mInputLayoutCache.reset();
	mGraphicsDriver.reset();

	if (mWindow != nullptr)
	{
		SDL_DestroyWindow(mWindow);
	}
}

bool Renderer::Init(int width, int height)
{
	// Create our SDL window
	mWindow = SDL_CreateWindow("ITP Engine 2 Demo!", 100, 100, width, height, 
		0);

	if (!mWindow)
	{
		SDL_Log("Could not create window.");
		return false;
	}

	mGraphicsDriver = std::make_shared<GraphicsDriver>(GetActiveWindow());
	mInputLayoutCache = std::make_shared<InputLayoutCache>();

	mWidth = width;
	mHeight = height;

	if (!InitFrameBuffer())
	{
		return false;
	}

	if (!InitShaders())
	{
		return false;
	}

	if (!InitSpriteVerts())
	{
		return false;
	}
	
	CreateTexture();
	
	return true;
}

void Renderer::RenderFrame()
{
	Clear();	
	mGraphicsDriver->SetRenderTarget(mTextureTarget);	
	mGraphicsDriver->SetDepthStencil(mDepthBuffer);
	DrawComponents();
	mGraphicsDriver->SetDepthStencil(nullptr);
	mGraphicsDriver->SetRenderTarget(mGraphicsDriver->GetBackBufferRenderTarget());
	DrawTexture();
	Present();
}

void Renderer::AddComponent(DrawComponentPtr component)
{
	if (IsA<SpriteComponent>(component) || IsA<FontComponent>(component))
	{
		mComponents2D.emplace(component);
	}
	else
	{
		mDrawComponents.emplace(component);
	}
}

void Renderer::RemoveComponent(DrawComponentPtr component)
{
	if (IsA<SpriteComponent>(component) || IsA<FontComponent>(component))
	{
		auto iter = mComponents2D.find(component);
		if (iter != mComponents2D.end())
		{
			mComponents2D.erase(iter);
		}
	}
	else
	{
		auto iter = mDrawComponents.find(component);
		if (iter != mDrawComponents.end())
		{
			mDrawComponents.erase(iter);
		}
	}
}

void Renderer::AddPointLight(PointLightComponentPtr light)
{
	mPointLights.insert(light);
	UpdatePointLights();
}

void Renderer::RemovePointLight(PointLightComponentPtr light)
{
	mPointLights.erase(light);
	UpdatePointLights();
}

void Renderer::UpdatePointLights()
{
	int count = 0;
	for (auto mesh : mMeshShaders)
	{
		count = 0;
		for (auto pointLights : mPointLights)
		{
			mesh.second->GetLightingConstants().PointLights[count] = pointLights->GetData();
			count++;
		}
		if (count < 8)
		{
			for (int i = count; i < MAX_POINT_LIGHTS; i++)
			{
				mesh.second->GetLightingConstants().PointLights[i].enabled = 0;
			}
		}
		mesh.second->UploadLightingConstants();
	}
}

void Renderer::DrawSprite(TexturePtr texture, const Matrix4& worldTransform)
{
	mSpriteShader->SetActive();

	// Updates World Matrix
	mSpriteShader->GetPerObjectConstants().worldMatrix = worldTransform;
	mSpriteShader->UploadPerObjectConstants();

	// Binds Texture
	mSpriteShader->BindTexture(texture, 0);

	// Draw vertex Array
	DrawVertexArray(mSpriteVerts);
}

void Renderer::DrawMesh(VertexArrayPtr vertArray, TexturePtr texture, const Matrix4& worldTransform, EMeshShader type)
{
	mMeshShaders.find(type)->second->SetActive();

	mMeshShaders.find(type)->second->GetPerObjectConstants().worldMatrix = worldTransform;
	mMeshShaders.find(type)->second->UploadPerObjectConstants();

	mMeshShaders.find(type)->second->BindTexture(texture, 0);

	DrawVertexArray(vertArray);
}

void Renderer::DrawSkeletalMesh(VertexArrayPtr vertArray, TexturePtr texture, const Matrix4& worldTransform, const struct MatrixPalette& palette)
{
	mMeshShaders.find(EMS_Skinned)->second->SetActive();

	mMeshShaders.find(EMS_Skinned)->second->GetPerObjectConstants().worldMatrix = worldTransform;
	mMeshShaders.find(EMS_Skinned)->second->UploadPerObjectConstants();
	mMeshShaders.find(EMS_Skinned)->second->UploadPostConstants();

	mMeshShaders.find(EMS_Skinned)->second->BindTexture(texture, 0);

	mMeshShaders.find(EMS_Skinned)->second->UploadMatrixPalette(palette);
	DrawVertexArray(vertArray);
}

void Renderer::DrawVertexArray(VertexArrayPtr vertArray)
{
	vertArray->SetActive();
	mGraphicsDriver->DrawIndexed(vertArray->GetIndexCount(), 0, 0);
}

void Renderer::UpdateViewMatrix(const Matrix4& view)
{
	mView = view;
	for (auto mesh : mMeshShaders)
	{
		mPostProcess->GetPostConstants().prevMatrix = mesh.second->GetPerCameraConstants().viewMatrix;
		mesh.second->GetPerCameraConstants().viewMatrix = mView * mProj;
		mesh.second->GetPerCameraConstants().worldCameraPos = view.GetTranslation();
		mPostProcess->GetPostConstants().currMatrix = mesh.second->GetPerCameraConstants().viewMatrix;
		mPostProcess->UploadPostConstants();
	}
}

void Renderer::SetAmbientLight(const Vector3& color)
{
	for (auto mesh : mMeshShaders)
	{
		mesh.second->GetLightingConstants().ambientLight = color;
		mesh.second->UploadLightingConstants();
	}
}

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
	// Convert screenPoint to device coordinates (between -1 and +1)
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= (mWidth) * 0.5f;
	deviceCoord.y /= (mHeight) * 0.5f;

	// First, undo the projection
	Matrix4 unprojection = mProj;
	unprojection.Invert();
	Vector3 unprojVec = TransformWithPerspDiv(deviceCoord, unprojection);

	// Now undo the view matrix
	Matrix4 uncamera = mView;
	uncamera.Invert();
	return Transform(unprojVec, uncamera);
}

void Renderer::Clear()
{
	mGraphicsDriver->ClearDepthStencil(mDepthBuffer, 1);
	mGraphicsDriver->ClearBackBuffer(Vector3(0, 0, 0), 1);
	mGraphicsDriver->ClearRenderTarget(mTextureTarget, Vector3(0, 0, 0), 1);
}

void Renderer::DrawComponents()
{
	// Enable depth buffering and disable blending
	mGraphicsDriver->SetDepthStencilState(mMeshDepthState);
	mGraphicsDriver->SetBlendState(mMeshBlendState);

	// Upload per camera constants
	for (auto mesh : mMeshShaders)
	{
		mesh.second->UploadPerCameraConstants();
	}

	// Update point lights
	UpdatePointLights();

	// Draw the normal components
	for (auto& comp : mDrawComponents)
	{
		if (comp->IsVisible())
		{
			comp->Draw(*this);
		}
	}

	// Disable depth buffering and enable blending
	mGraphicsDriver->SetDepthStencilState(mSpriteDepthState);
	mGraphicsDriver->SetBlendState(mSpriteBlendState);

	// Draw the 2D components
	for (auto& comp : mComponents2D)
	{
		if (comp->IsVisible())
		{
			comp->Draw(*this);
		}
	}
}

void Renderer::Present()
{
	mGraphicsDriver->Present();
}

bool Renderer::InitFrameBuffer()
{
	// Create Depth Buffer
	mDepthBuffer = mGraphicsDriver->CreateDepthStencil(mWidth, mHeight);
	mGraphicsDriver->SetDepthStencil(mDepthBuffer);

	// Create Depth States
	mMeshDepthState = mGraphicsDriver->CreateDepthStencilState(true, ECF_LessEqual);
	mSpriteDepthState = mGraphicsDriver->CreateDepthStencilState(false, ECF_Never);

	// Create Blend States
	mMeshBlendState = mGraphicsDriver->CreateBlendState(false);
	mSpriteBlendState = mGraphicsDriver->CreateBlendState(true);

	// Create RasterizerState
	mGraphicsDriver->SetRasterizerState(mGraphicsDriver->CreateRasterizerState(EFM_Solid));
	return true;
}

bool Renderer::InitShaders()
{
	// Load sprite shader and basic shader
	// And create input layouts needed for both
	mSpriteShader = mGame.GetAssetCache().Load<Shader>("Shaders/Sprite.hlsl");
	if (mSpriteShader == nullptr)
	{
		return false;
	}

	InputLayoutElement spriteElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	// Register Layout
	mInputLayoutCache->RegisterLayout("positiontexcoord", mGraphicsDriver->CreateInputLayout(spriteElements, 2, mSpriteShader->GetCompiledVS()));

	// Setting View Transform
	mSpriteShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mSpriteShader->UploadPerCameraConstants();

	mNegShader = mGame.GetAssetCache().Load<Shader>("Shaders/Negative.hlsl");
	if (mNegShader == nullptr)
	{
		return false;
	}

	InputLayoutElement negElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	// Register Layout
	mInputLayoutCache->RegisterLayout("positiontexcoord", mGraphicsDriver->CreateInputLayout(negElements, 2, mNegShader->GetCompiledVS()));

	// Setting View Transform
	mNegShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mNegShader->UploadPerCameraConstants();

	mPostProcessShaders.emplace(P_Neg, mNegShader);

	mPixShader = mGame.GetAssetCache().Load<Shader>("Shaders/Pixelate.hlsl");
	if (mPixShader == nullptr)
	{
		return false;
	}

	InputLayoutElement pixElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	// Register Layout
	mInputLayoutCache->RegisterLayout("positiontexcoord", mGraphicsDriver->CreateInputLayout(pixElements, 2, mPixShader->GetCompiledVS()));

	// Setting View Transform
	mPixShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mPixShader->UploadPerCameraConstants();

	mPostProcessShaders.emplace(P_Pix, mPixShader);

	mRadBlurShader = mGame.GetAssetCache().Load<Shader>("Shaders/RadialBlur.hlsl");
	if (mRadBlurShader == nullptr)
	{
		return false;
	}

	InputLayoutElement radElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	// Register Layout
	mInputLayoutCache->RegisterLayout("positiontexcoord", mGraphicsDriver->CreateInputLayout(radElements, 2, mRadBlurShader->GetCompiledVS()));

	// Setting View Transform
	mRadBlurShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mRadBlurShader->UploadPerCameraConstants();

	mPostProcessShaders.emplace(P_Rad, mRadBlurShader);

	mToonShader = mGame.GetAssetCache().Load<Shader>("Shaders/ToonLine.hlsl");
	if (mToonShader == nullptr)
	{
		return false;
	}
	InputLayoutElement toonElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	// Register Layout
	mInputLayoutCache->RegisterLayout("positiontexcoord", mGraphicsDriver->CreateInputLayout(toonElements, 2, mToonShader->GetCompiledVS()));

	// Setting View Transform
	mToonShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mToonShader->UploadPerCameraConstants();

	mPostProcessShaders.emplace(P_Toon, mToonShader);

	mBloomShader = mGame.GetAssetCache().Load<Shader>("Shaders/Bloom.hlsl");
	if (mBloomShader == nullptr)
	{
		return false;
	}

	InputLayoutElement bloomElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	// Register Layout
	mInputLayoutCache->RegisterLayout("positiontexcoord", mGraphicsDriver->CreateInputLayout(bloomElements, 2, mBloomShader->GetCompiledVS()));

	// Setting View Transform
	mBloomShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mBloomShader->UploadPerCameraConstants();

	mPostProcessShaders.emplace(P_Bloom, mBloomShader);

	mBlurShader = mGame.GetAssetCache().Load<Shader>("Shaders/Blurred.hlsl");
	if (mBlurShader == nullptr)
	{
		return false;
	}

	InputLayoutElement blurElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	// Register Layout
	mInputLayoutCache->RegisterLayout("positiontexcoord", mGraphicsDriver->CreateInputLayout(blurElements, 2, mBlurShader->GetCompiledVS()));

	// Setting View Transform
	mBlurShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mBlurShader->UploadPerCameraConstants();

	mPostProcessShaders.emplace(P_Blur, mBlurShader);

	mPostProcess = mGame.GetAssetCache().Load<Shader>("Shaders/PostProcess.hlsl");
	if (mPostProcess == nullptr)
	{
		return false;
	}

	InputLayoutElement postElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 12),
	};

	// Register Layout
	mInputLayoutCache->RegisterLayout("positiontexcoord", mGraphicsDriver->CreateInputLayout(postElements, 2, mPostProcess->GetCompiledVS()));

	// Setting View Transform
	mPostProcess->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), static_cast<float>(mHeight), 1000.0f, -1000.0f);
	mPostProcess->UploadPerCameraConstants();

	mPostProcess->GetPostConstants().ray = false;
	mPostProcess->GetPostConstants().neg = false;
	mPostProcess->GetPostConstants().rad = false;
	mPostProcess->GetPostConstants().pix = false;
	mPostProcess->GetPostConstants().blur = false;
	mPostProcess->GetPostConstants().bloom = false;
	mPostProcess->GetPostConstants().depth = false;
	mPostProcess->UploadPostConstants();

	// setting mProj
	mProj = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		static_cast<float>(mWidth),
		static_cast<float>(mHeight),
		25.0f, 10000.0f);

	// Basic mesh not loaded.
	auto basicShader = mGame.GetAssetCache().Load<Shader>("Shaders/BasicMesh.hlsl");
	if (basicShader == nullptr)
	{
		return false;
	}
	basicShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth), 
																	static_cast<float>(mHeight),
																	1000.0f, -1000.0f);
	basicShader->UploadPerCameraConstants();
	mMeshShaders.emplace(EMS_Basic, basicShader);

	InputLayoutElement meshElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("NORMAL", 0, EGF_R32G32B32_Float, 12),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 24),
	};

	mInputLayoutCache->RegisterLayout("positionnormaltexcoord", mGraphicsDriver->CreateInputLayout(meshElements, 3, mMeshShaders.find(EMS_Basic)->second->GetCompiledVS()));

	// Load phong shader
	auto phongShader = mGame.GetAssetCache().Load<Shader>("Shaders/Phong.hlsl");
	if (phongShader == nullptr)
	{
		return false;
	}
	phongShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth),
																static_cast<float>(mHeight),
																1000.0f, -1000.0f);
	phongShader->UploadPerCameraConstants();
	mMeshShaders.emplace(EMS_Phong, phongShader);

	// Load skinned shader and create appropriate input layout
	auto skinShader = mGame.GetAssetCache().Load<Shader>("Shaders/Skinned.hlsl");
	if (skinShader == nullptr)
	{
		return false;
	}
	skinShader->GetPerCameraConstants().viewMatrix = Matrix4::CreateOrtho(static_cast<float>(mWidth),
		static_cast<float>(mHeight),
		1000.0f, -1000.0f);
	skinShader->UploadPerCameraConstants();
	mMeshShaders.emplace(EMS_Skinned, skinShader);
	mMeshShaders.find(EMS_Skinned)->second->CreateMatrixPaletteBuffer();

	InputLayoutElement skinElements[] =
	{
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("NORMAL", 0, EGF_R32G32B32_Float, 12),
		InputLayoutElement("BLENDINDICES", 0, EGF_R8G8B8A8_UInt, 24),
		InputLayoutElement("BLENDWEIGHTS", 0, EGF_R8G8B8A8_UNorm, 28),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 32),
	};
	mInputLayoutCache->RegisterLayout("positionnormalbonesweightstexcoord", mGraphicsDriver->CreateInputLayout(skinElements, 5, mMeshShaders.find(EMS_Skinned)->second->GetCompiledVS()));

	mMeshShaders.find(EMS_Skinned)->second->GetPostConstants().toon = false;
	mMeshShaders.find(EMS_Skinned)->second->UploadPostConstants();

	return true;
}

bool Renderer::InitSpriteVerts()
{
	// Create the vertex array for sprites
	float verts[] =
	{
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,  // top left
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // top right
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom left
	};

	uint16_t indices[] =
	{
		0, 1, 2, // <top left, top right, bottom right>
		2, 3, 0, // <bottom right, bottom left, top left>
	};

	mSpriteVerts = VertexArray::Create(GetGraphicsDriver(), GetInputLayoutCache(),
		verts, 4, 20, "positiontexcoord", indices, 6);

	return true;
}

void Renderer::CreateTexture()
{
	mTextureTarget = mGraphicsDriver->CreateRenderTargetTexture(mWidth, mHeight, ETF_RGBA);
	/*for (auto shade : mPostProcessShaders)
	{
		shade.second->SetOn(false);
	}*/
}

void Renderer::DrawTexture()
{
	mShaderResource = mGraphicsDriver->GetShaderResource();
	mDepthResource = mGraphicsDriver->GetDepthResource();

	// Updates World Matrix
	Matrix4 scaleMat = Matrix4::CreateScale(
		static_cast<float>(mWidth),
		static_cast<float>(mHeight),
		1.0f);

	/*mPostProcessShaders.find(P_Rad)->second->SetOn(true);
	for (auto shade : mPostProcessShaders)
	{
		if (shade.second->IsOn())
		{
			shade.second->SetActive();
			shade.second->GetPerObjectConstants().worldMatrix = Matrix4::Identity * scaleMat;
			shade.second->UploadPerObjectConstants();
			//shade.second->GetPostConstants().ray = true;
			shade.second->UploadPostConstants();
		}
	}*/
	
	mPostProcess->SetActive();
	mPostProcess->GetPerObjectConstants().worldMatrix = Matrix4::Identity * scaleMat;
	mPostProcess->UploadPerObjectConstants();
	mPostProcess->UploadPostConstants();
	
	// Binds Texture
	mGraphicsDriver->SetPSTexture(mShaderResource, 0);
	mGraphicsDriver->SetPSTexture(mDepthResource, 1);

	// Draw vertex Array
	DrawVertexArray(mSpriteVerts);
	mGraphicsDriver->SetPSTexture(nullptr, 0);
	mGraphicsDriver->SetPSTexture(nullptr, 1);
}
