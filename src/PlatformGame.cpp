
#include "PlatformGame.h"
#include "SplashState.h"
#include "MainMenuState.h"
#include "GamePlayState.h"

#include "DebugNew.h"

// Number of static sprites to draw
static const unsigned NUM_SPRITES = 200;
static const StringHash VAR_MOVESPEED("MoveSpeed");
static const StringHash VAR_ROTATESPEED("RotateSpeed");

DEFINE_APPLICATION_MAIN(PlatformGame)

PlatformGame::PlatformGame(Context* context) :
    BaseApplication(context)
{
}

void PlatformGame::Start()
{
    stateManager_ = new StateManager(context_);
    // Execute base class startup
    BaseApplication::Start();

    // Main Menu
	State * mainMenuState = new MenuState(context_);
	mainMenuState->SetStateId("MainMenu");
	stateManager_->RegisterState(mainMenuState);

	// Game State
	State * gameplayState = new GamePlayState(context_);
	gameplayState->SetStateId("GamePlayState");
	stateManager_->RegisterState(gameplayState);

    SplashState * splashState = new SplashState(context_);
	splashState->SetStateId("SplashState");
	stateManager_->RegisterState(splashState);

	splashState->SetNextState("MainMenu");

	stateManager_->PushToStack("SplashState");
}
