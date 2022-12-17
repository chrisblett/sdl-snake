#pragma once

#include "Snake.h"
#include "SnakeGame.h"

#include <SDL/SDL.h>

// Determines snake behaviour
class SnakeBrain
{
public:
	SnakeBrain() :
		m_inputData{} 
	{
	}

	virtual ~SnakeBrain() 
	{ 
		printf("SnakeBrain destroyed!\n");
	}

	void SetInput(const InputData& input) { m_inputData = input; }
	virtual void Update(Snake* pSnake) = 0;

protected:
	InputData m_inputData;
};

// Brain used under normal game conditions
class NormalBrain : public SnakeBrain
{
public:
	virtual void Update(Snake* pSnake) override;
};

// Brain used when debugging
class DebugBrain : public SnakeBrain
{
public:
	virtual void Update(Snake* pSnake) override;

};