#include "SnakeGame.h"
#include "../Engine/Math/Vector2.h"
#include "../Engine/Math/Math.h"

#include <SDL/SDL.h>
#include <cmath>
#include <cassert>

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

SnakeGame::SnakeGame()
	: m_numRows(0)
	, m_numCols(0)
{
	static_assert(CELL_SIZE > 0, "Cell size is too small");

	m_pInputDir = &EAST;
	m_pSnakeDir = m_pInputDir;
}

bool SnakeGame::Init()
{
	if (!InitSDL())
		return false;

	SetWindowTitle("SDL Snake");

	// Calculate world size according to window size
	const auto& winSize = GetWindowSize();
	CalculateWorldDimensions(winSize.w, winSize.h);

	Vector2 worldOriginScreenSpace = CalculateRenderOrigin(winSize.w, winSize.h);
	GetRenderer().SetWorldTransform(worldOriginScreenSpace, CELL_SIZE);

	// Calculate snake position
	m_snakePos.x = static_cast<float>(m_numCols / 2);
	m_snakePos.y = static_cast<float>(m_numRows / 2);

	printf("SnakePos: (%f, %f)\n", m_snakePos.x, m_snakePos.y);

	return true;
}

void SnakeGame::Shutdown()
{
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

bool SnakeGame::ValidInputDirection(const Vector2& input)
{
	assert(m_pSnakeDir);

	// Cannot be opposite to our current direction
	return Vector2::Dot(*m_pSnakeDir, input) != -1.0f;
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

	if (pInput && ValidInputDirection(*pInput))
	{
		m_pInputDir = pInput;
	}
}

void SnakeGame::Update()
{
	AdvanceTimestep();
	float deltaTime = GetDeltaTime();

	/*
	printf("SnakeDir: (%f, %f) InputDir: (%f, %f)\n",
		m_pSnakeDir->x, m_pSnakeDir->y, m_pInputDir->x, m_pInputDir->y);
	*/
}

void SnakeGame::Render()
{
	auto& renderer = GetRenderer();

	renderer.SetDrawColour(0, 0, 0, 255);
	ClearScreen();

	RenderGrid(renderer);
	RenderSnake(renderer);

	SwapBuffers();
}

void SnakeGame::RenderGrid(const SDLAppRenderer& renderer) const
{
	// Set grid colour
	renderer.SetDrawColour(255, 255, 255, 255);

	// Draw row lines
	for (int i = 0; i <= m_numRows; i++)
	{
		Vector2 start(0, 1.0f * i);
		Vector2 end(1.0f * m_numCols, 1.0f * i);

		renderer.DrawLine(
			renderer.WorldToScreen(start),
			renderer.WorldToScreen(end)
		);
	}

	// Draw column lines
	for (int i = 0; i <= m_numCols; i++)
	{
		Vector2 start(1.0f * i, 0);
		Vector2 end(1.0f * i, 1.0f * m_numRows);

		renderer.DrawLine(
			renderer.WorldToScreen(start),
			renderer.WorldToScreen(end)
		);
	}
}

void SnakeGame::RenderSnake(const SDLAppRenderer& renderer) const
{
	// R, G, B, A
	#define OUTER_COLOUR 0, 103, 65, 255
	#define INNER_COLOUR 0, 146, 64, 255

	// Draw outer
	renderer.SetDrawColour(OUTER_COLOUR);
	renderer.FillRect(renderer.WorldToScreen(m_snakePos.x, m_snakePos.y, 1, 1));

	// Draw inner
	const float innerScale = .8f;
	Vector2 centeredPos = Math::GetCenteredPosition(m_snakePos, innerScale, innerScale);

	renderer.SetDrawColour(INNER_COLOUR);

	// Move from top-left to midpoint of the cell and draw
	renderer.FillRect(renderer.WorldToScreen(
		centeredPos.x + .5f,
		centeredPos.y + .5f,
		innerScale,
		innerScale)
	);
}

void SnakeGame::CalculateWorldDimensions(int renderAreaW, int renderAreaH)
{
	// Calculate the number of cells (to the nearest integer) 
	// that can fit within the given length of pixels. 
	// Subtract a row and col to leave some space between the 
	// boundary of the render area
	m_numRows = static_cast<int>(std::roundf(1.0f * renderAreaH / CELL_SIZE)) - 1;
	m_numCols = static_cast<int>(std::roundf(1.0f * renderAreaW / CELL_SIZE)) - 1;
}

Vector2 SnakeGame::CalculateRenderOrigin(int renderAreaW, int renderAreaH) const
{
	// Calculate the total space left in the render area in both dimensions
	int pixelGapX = renderAreaW - m_numCols * CELL_SIZE;
	int pixelGapY = renderAreaH - m_numRows * CELL_SIZE;

	// Centre the view of the world
	return Vector2(pixelGapX / 2.0f, pixelGapY / 2.0f);
}