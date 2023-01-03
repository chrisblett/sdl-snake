#pragma once

#include "../Engine/SDLApp.h"
#include "../Engine/Array2D.h"
#include "../Engine/Math/Vector2.h"

namespace Assets
{
	extern const char* SNAKE_HEAD_TEXTURE_PATH;
	extern const char* SNAKE_TAIL_TEXTURE_PATH;
	extern const char* SNAKE_TURN_TEXTURE_PATH;
	extern const char* SNAKE_BODY_TEXTURE_PATH;
	extern const char* SNAKE_FOOD_TEXTURE_PATH;
}

struct InputData
{
	const Vector2* pLastInputDir; 
	const Uint8* pKeyboardState; // Access to the keyboard state
	bool dirInputThisFrame; // Stores whether the player hit a valid directional key this frame
};

enum SnakeStatus
{
	STATUS_ACTIVE, // There is food to be eaten (Still playing)
	STATUS_DONE, // All food eaten, (Player won!)
	STATUS_DEAD, 
};

class World;
class SnakeBrain;

class SnakeGame : public SDLApp
{
public:
	SnakeGame();

	virtual bool Init()         override;
	virtual void Shutdown()     override;
	virtual void ProcessInput() override;
	virtual void Update()       override;
	virtual void Render()       override;
	
	// Direction vectors representing the four cardinal directions
	// that the snake can move in
	static const Vector2 NORTH;
	static const Vector2 EAST;
	static const Vector2 SOUTH;
	static const Vector2 WEST;

private:
	// Begin game over sequence
	void DoGameOver();

	// Calculate the top-left pos that the renderer will draw the world from
	Vector2 CalculateRenderOrigin(int renderAreaW, int renderAreaH,
		int worldWidth, int worldHeight) const;

	static const Vector2* GetInputDirection(const Uint8* pKeyState);
	bool ValidInputDirection(const Vector2& input) const;

	// Size of an individual cell in pixels
	static const int CELL_SIZE;

	std::unique_ptr<SnakeBrain> m_pBrain;
	std::unique_ptr<World> m_pWorld;
	const Vector2* m_pLastInputDir; // Last direction that the player requested
	float m_nextUpdateTime; // Time until the next update
	bool m_gameEnding; // Whether the game is ending this frame
};