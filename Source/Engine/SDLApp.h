#pragma once

#include <SDL/SDL.h>
#include <memory>

#include "../Engine/SDLAppRenderer.h"
#include "../Engine/SDLWindow.h"

class SDLApp
{
public:
	SDLApp();
	virtual ~SDLApp() {};

	virtual bool Init()         = 0;
	virtual void Shutdown()     = 0;
	virtual void ProcessInput() = 0;
	virtual void Update()       = 0;
	virtual void Render()       = 0;

	// Begins the game loop
	void Run();

protected:
	/*
	* Initialises the necessary SDL subsystems and components.
	* You should call this first during the initialisation of your derived object.
	*/
	bool InitSDL();

	/*
	* Shuts down any SDL subsystems and components that have been created.
	* You should call this first during the shutdown of your derived object.
	*/
	void ShutdownSDL();

	// Calculates the new delta time value for this frame
	void AdvanceTimestep();

	void SetDrawColour(SDL_Color color) const;

	// Clear the screen to the clear colour
	void ClearScreen() const;

	// Display the result of rendering to the screen
	void SwapBuffers() const;

	// Exits the game loop, initiating the app's shutdown process
	void Terminate() { m_isRunning = false; }

	// Returns the time (in seconds) since the last frame
	float GetDeltaTime()  const { return m_deltaTime; }

	SDLAppRenderer&       GetRenderer()   const { return *(m_pRenderer.get()); }
	SDLWindow::WindowSize GetWindowSize() const { return m_pWindow->GetSize(); }

	void SetWindowTitle(const char* pTitle) { m_pWindow->SetTitle(pTitle); }

private:
	// Value to clamp delta time to if it grows too large
	static const float MAX_DELTA_VALUE;

	std::unique_ptr<SDLAppRenderer> m_pRenderer;
	std::unique_ptr<SDLWindow>      m_pWindow;
	Uint32 m_ticksCount;
	float  m_deltaTime;
	bool   m_isRunning;
};