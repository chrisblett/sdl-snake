#include "SnakeBrain.h"
#include "SnakeGame.h"
#include "../Engine/Math/Vector2.h"

#include <SDL/SDL.h>

void NormalBrain::Update(Snake* pSnake)
{
	pSnake->Simulate(m_inputData.pLastInputDir);
}

void DebugBrain::Update(Snake* pSnake)
{
	// Eat requested?
	if (m_inputData.pKeyboardState[SDL_SCANCODE_SPACE])
	{
		pSnake->EatFood(1);
	}

	// Only move if player hit a movement key this frame
	if (m_inputData.dirInputThisFrame)
	{
		assert(m_inputData.pLastInputDir && "Move key was recorded but the associated input is NULL!");
		pSnake->Simulate(m_inputData.pLastInputDir);
	}
}