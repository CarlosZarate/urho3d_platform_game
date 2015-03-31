
#pragma once

#include "StateManager.h"

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;



//-----------------------------------------------------------------------------
// State
//-----------------------------------------------------------------------------
class MenuState : public State
{
	// Enable type information.
	OBJECT(MenuState);


public:

	//-------------------------------------------------------------------------
	// Constructor & Destructor
	//-------------------------------------------------------------------------
	MenuState(Context* context);
	~MenuState();

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

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------

private:
	//-------------------------------------------------------------------------
	// Private Methods
	//-------------------------------------------------------------------------
	/// Create and initialize a Window control.
	void InitWindow();
	/// Create and add various common controls for demonstration purposes.
	void InitControls();
	/// Handle close button pressed and released.
	void HandleQuitButton(StringHash eventType, VariantMap& eventData);
	void HandleCreditsButton(StringHash eventType, VariantMap& eventData);
	void HandleOptionsButton(StringHash eventType, VariantMap& eventData);
	void HandlePlayButton(StringHash eventType, VariantMap& eventData);
	/// Handle key down event to process key controls common to all samples.
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void Quit();

	void HandleQuitMessageAck(StringHash eventType, VariantMap& eventData);
protected:
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	/// The Window.
	SharedPtr<UIElement> window_;
	/// The UI's root UIElement.
	SharedPtr<UIElement> uiRoot_;

	SharedPtr<Sprite> backgroundSprite_;
};
