#include "ITPEnginePCH.h"
#include <SDL/SDL_mixer.h>
#include "Player.h"

Game::Game()
	:mRenderer(*this)
	,mAssetCache(*this, "Assets/")
	,mShouldQuit(false)
{

}

Game::~Game()
{
	mMessage = nullptr;
	mFont = nullptr;
	mAssetCache.Clear();
	mWorld.RemoveAllActors();
	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
}

bool Game::Init()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Failed to initialize SDL.");
		return false;
	}

	// Initialize Renderer
	if (!mRenderer.Init(1050, 700))
	{
		SDL_Log("Failed to initialize renderer.");
		return false;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
	{
		SDL_Log("Failed to initialize SDL_mixer.");
		return false;
	}

	// Initialize SDL_ttf
	if (TTF_Init() != 0)
	{
		SDL_Log("Failed to initialize SDL_ttf");
		return false;
	}

	// Initialize RNG
	Random::Init();

	// Add input mappings
	AddInputMappings();

	// Start frame timer
	mTimer.Start();

	// Run any code at game start
	StartGame();

	return true;
}

void Game::RunLoop()
{
	while (!mShouldQuit)
	{
		ProcessInput();
		Tick();
		GenerateOutput();
	}
}

void Game::Quit()
{
	mShouldQuit = true;
}

void Game::StartGame()
{
	LevelLoader loader(*this);
	loader.Load("Assets/Levels/lab5.itplevel");

	auto HUDActor = Actor::Spawn(*this);
	mMessage = FontComponent::Create(*HUDActor);
	mFont = mAssetCache.Load<Font>("Fonts/Carlito-Regular.ttf");
	mMessage->SetFont(mFont);

	HUDActor->SetPosition(Vector3(0, 300, 0));
	mMessage->SetText("Toggle Shaders: 1- Negative 2 - Light Scatter 3 - Pixelate 4 - Toon 5- Radial Blur 6 - Motion Blur 7 - Bloom 8 - Depth", Color::Pink, 18);
	mMessage->SetAlignment(FontComponent::AlignCenter);
	mMessage->SetIsVisible(true);
}

void Game::ProcessInput()
{
	// Poll events from SDL
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mShouldQuit = true;
			break;
		case SDL_KEYDOWN:
			if (event.key.repeat == 0)
			{
				HandleKeyPressed(event.key.keysym.sym);
			}
			break;
		case SDL_KEYUP:
			HandleKeyReleased(event.key.keysym.sym);
			break;
		default:
			// Ignore other events
			break;
		}
	}
}

void Game::HandleKeyPressed(int key)
{
	mInput.HandleKeyPressed(key);
}

void Game::HandleKeyReleased(int key)
{
	mInput.HandleKeyReleased(key);
}

void Game::Tick()
{
	// Lock @ 60 FPS
	float deltaTime = mTimer.GetFrameTime(0.016666f);

	mGameTimers.Tick(deltaTime);

	// Update game world
	mWorld.Tick(deltaTime);

	// Update physics world
	mPhysWorld.Tick(deltaTime);
}

void Game::GenerateOutput()
{
	mRenderer.RenderFrame();
}

void Game::AddInputMappings()
{
	// Parse input mappings
	mInput.ParseMappings("Config/Mappings.itpconfig");

	// Bind our quit function to "Quit" on release
	mInput.BindAction("Quit", IE_Released, this, &Game::Quit);

	// Bind shader stuff
	mInput.BindAction("Neg", IE_Released, this, &Game::TurnNeg);
	mInput.BindAction("Ray", IE_Released, this, &Game::TurnRay);
	mInput.BindAction("Pix", IE_Released, this, &Game::TurnPix);
	mInput.BindAction("Toon", IE_Released, this, &Game::TurnToon);
	mInput.BindAction("Rad", IE_Released, this, &Game::TurnRad);
	mInput.BindAction("Blur", IE_Released, this, &Game::TurnBlur);
	mInput.BindAction("Glow", IE_Released, this, &Game::TurnGlow);
	mInput.BindAction("Depth", IE_Released, this, &Game::TurnDepth);
}

void Game::TurnNeg() {
	if (mRenderer.GetPostProcess()->GetPostConstants().neg == false)
	{
		mRenderer.GetPostProcess()->GetPostConstants().neg = true;
		mRenderer.GetPostProcess()->GetPostConstants().ray = false;
		mRenderer.GetPostProcess()->GetPostConstants().pix = false;
		mRenderer.GetPostProcess()->GetPostConstants().rad = false;
		mRenderer.GetPostProcess()->GetPostConstants().blur = false;
		mRenderer.GetPostProcess()->GetPostConstants().bloom = false;
		mRenderer.GetPostProcess()->GetPostConstants().depth = false;
	}
	else mRenderer.GetPostProcess()->GetPostConstants().neg = false;

	mRenderer.GetPostProcess()->UploadPostConstants();
}
void Game::TurnRay() {
	if (mRenderer.GetPostProcess()->GetPostConstants().ray == false)
	{
		mRenderer.GetPostProcess()->GetPostConstants().neg = false;
		mRenderer.GetPostProcess()->GetPostConstants().ray = true;
		mRenderer.GetPostProcess()->GetPostConstants().pix = false;
		mRenderer.GetPostProcess()->GetPostConstants().rad = false;
		mRenderer.GetPostProcess()->GetPostConstants().blur = false;
		mRenderer.GetPostProcess()->GetPostConstants().bloom = false;
		mRenderer.GetPostProcess()->GetPostConstants().depth = false;
	}
	else mRenderer.GetPostProcess()->GetPostConstants().ray = false;

	mRenderer.GetPostProcess()->UploadPostConstants();
}
void Game::TurnPix() {
	if (mRenderer.GetPostProcess()->GetPostConstants().pix == false)
	{
		mRenderer.GetPostProcess()->GetPostConstants().neg = false;
		mRenderer.GetPostProcess()->GetPostConstants().ray = false;
		mRenderer.GetPostProcess()->GetPostConstants().pix = true;
		mRenderer.GetPostProcess()->GetPostConstants().rad = false;
		mRenderer.GetPostProcess()->GetPostConstants().blur = false;
		mRenderer.GetPostProcess()->GetPostConstants().bloom = false;
		mRenderer.GetPostProcess()->GetPostConstants().depth = false;
	}
	else mRenderer.GetPostProcess()->GetPostConstants().pix = false;

	mRenderer.GetPostProcess()->UploadPostConstants();
}
void Game::TurnToon() {
	if (mRenderer.GetMShaders().find(EMS_Skinned)->second->GetPostConstants().toon == false)
		mRenderer.GetMShaders().find(EMS_Skinned)->second->GetPostConstants().toon = true;
	else GetRenderer().GetMShaders().find(EMS_Skinned)->second->GetPostConstants().toon = false;

	mRenderer.GetMShaders().find(EMS_Skinned)->second->UploadPostConstants();
}
void Game::TurnRad() {
	if (mRenderer.GetPostProcess()->GetPostConstants().rad == false)
	{
		mRenderer.GetPostProcess()->GetPostConstants().neg = false;
		mRenderer.GetPostProcess()->GetPostConstants().ray = false;
		mRenderer.GetPostProcess()->GetPostConstants().pix = false;
		mRenderer.GetPostProcess()->GetPostConstants().rad = true;
		mRenderer.GetPostProcess()->GetPostConstants().blur = false;
		mRenderer.GetPostProcess()->GetPostConstants().bloom = false;
		mRenderer.GetPostProcess()->GetPostConstants().depth = false;
	}
	else mRenderer.GetPostProcess()->GetPostConstants().rad = false;

	mRenderer.GetPostProcess()->UploadPostConstants();
}

void Game::TurnBlur() {
	if (mRenderer.GetPostProcess()->GetPostConstants().blur == false)
	{
		mRenderer.GetPostProcess()->GetPostConstants().neg = false;
		mRenderer.GetPostProcess()->GetPostConstants().ray = false;
		mRenderer.GetPostProcess()->GetPostConstants().pix = false;
		mRenderer.GetPostProcess()->GetPostConstants().rad = false;
		mRenderer.GetPostProcess()->GetPostConstants().blur = true;
		mRenderer.GetPostProcess()->GetPostConstants().bloom = false;
		mRenderer.GetPostProcess()->GetPostConstants().depth = false;
	}
	else mRenderer.GetPostProcess()->GetPostConstants().blur = false;

	mRenderer.GetPostProcess()->UploadPostConstants();
}

void Game::TurnGlow() {
	if (mRenderer.GetPostProcess()->GetPostConstants().bloom == false)
	{
		mRenderer.GetPostProcess()->GetPostConstants().neg = false;
		mRenderer.GetPostProcess()->GetPostConstants().ray = false;
		mRenderer.GetPostProcess()->GetPostConstants().pix = false;
		mRenderer.GetPostProcess()->GetPostConstants().rad = false;
		mRenderer.GetPostProcess()->GetPostConstants().blur = false;
		mRenderer.GetPostProcess()->GetPostConstants().bloom = true;
		mRenderer.GetPostProcess()->GetPostConstants().depth = false;
	}
	else mRenderer.GetPostProcess()->GetPostConstants().bloom = false;

	mRenderer.GetPostProcess()->UploadPostConstants();
}

void Game::TurnDepth() {
	if (mRenderer.GetPostProcess()->GetPostConstants().depth == false)
	{
		mRenderer.GetPostProcess()->GetPostConstants().neg = false;
		mRenderer.GetPostProcess()->GetPostConstants().ray = false;
		mRenderer.GetPostProcess()->GetPostConstants().pix = false;
		mRenderer.GetPostProcess()->GetPostConstants().rad = false;
		mRenderer.GetPostProcess()->GetPostConstants().blur = false;
		mRenderer.GetPostProcess()->GetPostConstants().bloom = false;
		mRenderer.GetPostProcess()->GetPostConstants().depth = true;
	}
	else mRenderer.GetPostProcess()->GetPostConstants().depth = false;

	mRenderer.GetPostProcess()->UploadPostConstants();
}