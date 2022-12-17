#include "SnakeGame.h"
#include "SnakeBrain.h"
#include "World.h"
#include "../Engine/Math/Vector2.h"
#include "../Engine/Math/Math.h"
#include "../Engine/Math/Random.h"
#include "../Engine/Graphics.h"

#include <SDL/SDL.h>
#include <cmath>
#include <cassert>
#include <memory>
#include <vector>
#include <algorithm>

const int SnakeGame::CELL_SIZE = 32;

const Vector2 SnakeGame::NORTH = Vector2( 0, -1); // In SDL +y faces down
const Vector2 SnakeGame::EAST  = Vector2( 1,  0);
const Vector2 SnakeGame::SOUTH = Vector2( 0,  1);
const Vector2 SnakeGame::WEST  = Vector2(-1,  0);

typedef const int Command;
Command MOVE_NORTH = SDL_SCANCODE_UP;
Command MOVE_EAST  = SDL_SCANCODE_RIGHT;
Command MOVE_SOUTH = SDL_SCANCODE_DOWN;
Command MOVE_WEST  = SDL_SCANCODE_LEFT;

const int SNAKE_SPEED   = 7;				  // How many cells it covers per second
const float SNAKE_DELAY = 1.0f / SNAKE_SPEED; // Delay between snake updates in seconds

using std::unique_ptr;
using std::make_unique;

SnakeGame::SnakeGame()
	: m_pBrain(nullptr)
	, m_pLastInputDir(nullptr)
	, m_nextUpdateTime(0.0f)
{
	static_assert(CELL_SIZE > 0, "Cell size is too small");
}

bool SnakeGame::Init()
{
	if (!InitSDL())
		return false;

	GetGraphics().LoadTexture(Assets::SNAKE_CORNER_TEXTURE_PATH);
	GetGraphics().LoadTexture(Assets::SNAKE_BODY_TEXTURE_PATH);

	Random::Init();

	SetWindowTitle("SDL Snake");

	// Calculate world size according to window size
	const auto& winSize = GetWindowSize();

	// Calculate the dimensions of the world given the width and height
	// of the render region and accounting for some space
	int worldWidth  = static_cast<int>(std::roundf(1.0f * winSize.w / CELL_SIZE)) - 1;
	int worldHeight = static_cast<int>(std::roundf(1.0f * winSize.h / CELL_SIZE)) - 1;

	m_pWorld = make_unique<World>(worldWidth, worldHeight);

	// Create snake brain
	m_pBrain = make_unique<NormalBrain>();
	//m_pBrain = make_unique<DebugBrain>();

	Vector2 worldOriginScreenSpace = CalculateRenderOrigin(winSize.w, winSize.h, worldWidth, worldHeight);
	GetGraphics().GetRenderer().SetWorldTransform(worldOriginScreenSpace, CELL_SIZE);

	return true;
}

void SnakeGame::Shutdown()
{
	printf("Beginning game shutdown sequence\n");
	ShutdownSDL();
}

const Vector2* SnakeGame::GetInputDirection(const Uint8* pKeyState)
{
	const Vector2* pInput = NULL;

	if (pKeyState[MOVE_NORTH])
	{
		pInput = &NORTH;
	}
	if (pKeyState[MOVE_EAST])
	{
		pInput = &EAST;
	}
	if (pKeyState[MOVE_SOUTH])
	{
		pInput = &SOUTH;
	}
	if (pKeyState[MOVE_WEST])
	{
		pInput = &WEST;
	}

	return pInput;
}

bool SnakeGame::ValidInputDirection(const Vector2& input) const
{
	const Vector2& snakeDir = m_pWorld->GetSnake()->GetDirection();

	// Cannot be opposite to our current direction
	return Vector2::Dot(snakeDir, input) != -1.0f;
}

void SnakeGame::ProcessInput()
{
	SDL_Event event;

	// While there are still events in the queue
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If this is an SDL_QUIT event, end game loop
			case SDL_QUIT:
				Terminate();
				break;
		}
	}

	const Uint8* pState = SDL_GetKeyboardState(NULL);

	// If escape key is pressed, also end game loop
	if (pState[SDL_SCANCODE_ESCAPE])
	{
		Terminate();
	}

	// See if player wants to turn snake
	const Vector2* pInput = GetInputDirection(pState);

	bool validInput = false;
	if (pInput )
	{
		validInput = ValidInputDirection(*pInput);
		if (validInput)
		{
			m_pLastInputDir = pInput;
		}
	}

	// Fill out input data
	InputData input;
	input.pLastInputDir = m_pLastInputDir;
	input.inputDirThisFrame = validInput;
	input.pKeyboardState = pState;

	// Send new input to the brain
	m_pBrain->SetInput(input);
}

void SnakeGame::Update()
{
	AdvanceTimestep();
	float deltaTime = GetDeltaTime();

	m_nextUpdateTime -= deltaTime;

	// Ready to update snake?
	if (m_nextUpdateTime <= 0.0f)
	{
		m_nextUpdateTime += SNAKE_DELAY;

		m_pWorld->Update(*m_pBrain.get());
	}
}

void SnakeGame::Render()
{
	auto& renderer = GetGraphics().GetRenderer();

	renderer.SetDrawColour(0, 0, 0, 255);
	renderer.Clear();

	m_pWorld->Render(renderer);

	renderer.SwapBuffers();
}

Vector2 SnakeGame::CalculateRenderOrigin(int renderAreaW, int renderAreaH,
	int worldWidth, int worldHeight) const
{
	// Calculate the total space left in the render area in both dimensions
	int pixelGapX = renderAreaW - worldWidth  * CELL_SIZE;
	int pixelGapY = renderAreaH - worldHeight * CELL_SIZE;

	// Centre the view of the world
	return Vector2(pixelGapX / 2.0f, pixelGapY / 2.0f);
}