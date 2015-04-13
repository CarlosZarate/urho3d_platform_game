
#pragma once

#include "Engine.h"
#include "StateManager.h"
#include "PlayerEntity.h"
#include "EnemyEntity.h"

namespace Urho3D
{
class Node;
class Scene;
class Sprite;
class Text;
}

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;


//-----------------------------------------------------------------------------
// State
//-----------------------------------------------------------------------------
class GamePlayState : public State
{
	// Enable type information.
	OBJECT(GamePlayState);


public:

	//-------------------------------------------------------------------------
	// Constructor & Destructor
	//-------------------------------------------------------------------------
	GamePlayState(Context* context);
	~GamePlayState();

	//-------------------------------------------------------------------------
	// Public Virtual Methods
	//-------------------------------------------------------------------------
	// State Activation
	/// Init is called directly after state is registered
	virtual bool	Initialize();
	virtual bool	Begin();
	virtual void	End();

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------

private:
	//-------------------------------------------------------------------------
	// Private Methods
	//-------------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	/// Construct the scene content.
    void CreateScene();
	/// Construct UI.
	void CreateUI();
    /// Construct an instruction text to the UI.
    void CreateInstructions();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();
    /// Subscribe to application-wide logic update events.
    void SubscribeToEvents();
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void HandleBeginContact(StringHash eventType, VariantMap& eventData);

    void HandleEndContact(StringHash eventType, VariantMap& eventData);

    void HandleJoystickButtonDownPressed(StringHash eventType, VariantMap& eventData);

    void HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData);

    void ReduceHearts(StringHash eventType, VariantMap& eventData);

    void EnemyDied(StringHash eventType, VariantMap& eventData);

    void CreateHearts();

	SharedPtr<Scene> scene_;

	SharedPtr<Node> cameraNode_;

	SharedPtr<Text> instructionText;

	SharedPtr<Node> nodeWall;

	Node* bgNode_;

    int player_hearts = 0;

	/// The controllable character component.
    WeakPtr<PlayerEntity> player_;
    WeakPtr<EnemyEntity> enemy_;

    Vector<Node*> RemoveBulletList;
    Vector<Sprite*> CountHeart;

    bool upPressButoon = true;
    bool isPause = false;
    bool isDead = false;
    float timerrestar = 0;

protected:
    Vector2 GetMousePositionXY();
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------

};
