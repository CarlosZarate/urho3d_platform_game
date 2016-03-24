#pragma once
#include "Urho3D/Core/Object.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Container/Ptr.h"
#include "Urho3D/Container/List.h"
#include "Urho3D/Container/HashMap.h"
#include "Urho3D/UI/UIElement.h"
#include "Urho3D/UI/Sprite.h"
#include "Urho3D/Scene/Node.h"
#include "Urho3D/Scene/Scene.h"
#include "Urho3D/UI/Text.h"
#include "ExpirationTimer.h"

#include "StateManager.h"

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;



//-----------------------------------------------------------------------------
// State
//-----------------------------------------------------------------------------
class SplashState : public State
{
	// Enable type information.
	URHO3D_OBJECT(SplashState, State);


public:

	//-------------------------------------------------------------------------
	// Constructor & Destructor
	//-------------------------------------------------------------------------
	SplashState(Context* context);
	~SplashState();

	//-------------------------------------------------------------------------
	// Public Virtual Methods
	//-------------------------------------------------------------------------
	// State Activation
	/// Init is called directly after state is registered
	virtual bool	Initialize();
	virtual bool	Begin();
	/// after end : We are no longer attached to anything (simply being stored in
	///				the registered state list maintained by the manager).
	virtual void	End();

	/// Handle the logic update event.
	void HandleUpdate(StringHash eventType, VariantMap& eventData);

	void SetNextState(const char* id);
protected:
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	/// The UI's root UIElement.
	SharedPtr<UIElement> uiRoot_;
	SharedPtr<Sprite> backgroundSprite_;
	ExpirationTimer timer_;
	bool active_;
	String nextStateID_;
};
