
#include "Object.h"
#include "Context.h"
#include "Ptr.h"
#include "List.h"
#include "HashMap.h"
#include "UIElement.h"
#include "Sprite.h"
#include "Node.h"
#include "Scene.h"
#include "Text.h"
#include "MainMenuState.h"
#include "Object.h"
#include "CoreEvents.h"
#include "ResourceCache.h"
#include "List.h"
#include "UI.h"

#include "Button.h"
#include "CheckBox.h"
#include "CoreEvents.h"
#include "Engine.h"
#include "Input.h"
#include "LineEdit.h"
#include "Text.h"
#include "UIEvents.h"
#include "Window.h"

#include "DebugNew.h"
#include "Texture2D.h"
#include "ResourceCache.h"
#include "XMLFile.h"
#include "Octree.h"
#include "StaticModel.h"
#include "Material.h"
#include "Model.h"
#include "Light.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Camera.h"
#include "Renderer.h"
#include "Viewport.h"
#include "Font.h"
#include "MessageBox.h"

///////////////////////////////////////////////////////////////////////////////
// AppState Member Functions
///////////////////////////////////////////////////////////////////////////////

MenuState::MenuState(Context* context) : State(context)
{
}

MenuState::~MenuState()
{
}

bool MenuState::Initialize()
{
	uiRoot_ = GetSubsystem<UI>()->GetRoot();

	return true;
}

bool MenuState::Begin()
{
	// Enable OS cursor
	GetSubsystem<Input>()->SetMouseVisible(true);

	// Load XML file containing default UI style sheet
	SharedPtr<ResourceCache> cache(GetSubsystem<ResourceCache>());
	SharedPtr<XMLFile> style(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

	// Set the loaded style as default style
	uiRoot_->SetDefaultStyle(style);

	// Initialize Window
	InitWindow();

	// Create and add some controls to the Window
	InitControls();

	// Subscribe key down event
	SubscribeToEvent(E_KEYDOWN, HANDLER(MenuState, HandleKeyDown));


	// Call base class implementation
	return State::Begin();;
}

void MenuState::End()
{
	if (GetSubsystem<Input>())
		GetSubsystem<Input>()->SetMouseVisible(false);

	uiRoot_->RemoveChild(window_);
	uiRoot_->RemoveChild(backgroundSprite_);

	if (GetSubsystem<UI>())
		GetSubsystem<UI>()->SetFocusElement(NULL);

	UnsubscribeFromAllEvents();
	// Call base class implementation
	State::End();
}

void MenuState::InitWindow()
{
	// Load XML file containing default UI style sheet
	SharedPtr<ResourceCache> cache(GetSubsystem<ResourceCache>());

	window_ = GetSubsystem<UI>()->LoadLayout(cache->GetResource<XMLFile>("GUI/MainMenu.xml"));
	window_->SetOpacity(0.7f);
	uiRoot_->AddChild(window_);

	window_->BringToFront();
}

void MenuState::InitControls()
{
	if (!window_)
		return;

	Button* button = (Button*)window_->GetChild("PlayButton", true);
	SubscribeToEvent(button, E_RELEASED, HANDLER(MenuState, HandlePlayButton));

	button = (Button*)window_->GetChild("OptionsButton", true);
	SubscribeToEvent(button, E_RELEASED, HANDLER(MenuState, HandleOptionsButton));

	button = (Button*)window_->GetChild("CreditsButton", true);
	SubscribeToEvent(button, E_RELEASED, HANDLER(MenuState, HandleCreditsButton));

	button = (Button*)window_->GetChild("QuitButton", true);
	SubscribeToEvent(button, E_RELEASED, HANDLER(MenuState, HandleQuitButton));
}

void MenuState::HandleQuitButton(StringHash eventType, VariantMap& eventData)
{
	Quit();
}

void MenuState::HandleCreditsButton(StringHash eventType, VariantMap& eventData)
{
}

void MenuState::HandleOptionsButton(StringHash eventType, VariantMap& eventData)
{
}

void MenuState::HandlePlayButton(StringHash eventType, VariantMap& eventData)
{
	stateManager_->PushToStack("GamePlayState");
}

void MenuState::Quit()
{
	SharedPtr<Urho3D::MessageBox> messageBox(new Urho3D::MessageBox(context_, "Do you really want to exit the Game ?", "Quit Game ?"));

	if (messageBox->GetWindow() != NULL)
	{
		Button* cancelButton = (Button*)messageBox->GetWindow()->GetChild("CancelButton", true);
		cancelButton->SetVisible(true);
		cancelButton->SetFocus(true);
		SubscribeToEvent(messageBox, E_MESSAGEACK, HANDLER(MenuState, HandleQuitMessageAck));
	}
	messageBox->AddRef();
}

void MenuState::HandleQuitMessageAck(StringHash eventType, VariantMap& eventData)
{
	using namespace MessageACK;

	bool ok_ = eventData[P_OK].GetBool();

	if (ok_)
		GetSubsystem<Engine>()->Exit();
}

void MenuState::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;

	int key = eventData[P_KEY].GetInt();
	// Close console (if open) or exit when ESC is pressed
	if (key == KEY_ESC)
	{
		Quit();
	}
}
