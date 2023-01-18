#include "SnakeBrain.h"
#include "SnakeGame.h"
#include "../Engine/Math/Vector2.h"
#include "../Engine/Util.h"

#include <SDL/SDL.h>

SnakeBrain::~SnakeBrain()
{
	Util::DebugPrint("SnakeBrain destroyed!\n");
}

void NormalBrain::Update(Snake* pSnake)
{
	pSnake->Simulate(m_inputData.pLastInputDir);
}

#if _DEBUG
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
#endif