#include "Urho3D/Core/CoreEvents.h"
#include "Urho3D/UI/UI.h"
#include "Urho3D/Engine/Engine.h"
#include "Urho3D/Graphics/Texture2D.h"
#include "Urho3D/Resource/ResourceCache.h"

#include "SplashState.h"

///////////////////////////////////////////////////////////////////////////////
// AppState Member Functions
///////////////////////////////////////////////////////////////////////////////

SplashState::SplashState(Context* context) : State(context),
timer_(2000),
active_(true)
{
}

SplashState::~SplashState()
{

}


bool SplashState::Initialize()
{
	uiRoot_ = GetSubsystem<UI>()->GetRoot();

	return true;
}

bool SplashState::Begin()
{
	// Load XML file containing default UI style sheet
	SharedPtr<ResourceCache> cache(GetSubsystem<ResourceCache>());
	SharedPtr<XMLFile> style(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

	// Create Background sprite and add to the UI layout
	Texture2D* logoTexture = cache->GetResource<Texture2D>("Textures/Urho3dLogo.png");
	UI* ui = GetSubsystem<UI>();
	backgroundSprite_ = ui->GetRoot()->CreateChild<Sprite>();

	// Set Background sprite texture
	backgroundSprite_->SetTexture(logoTexture);

	int textureWidth = ui->GetRoot()->GetWidth();
	int textureHeight = ui->GetRoot()->GetHeight();

	// Set logo sprite scale
	//logoSprite_->SetScale(256.0f / textureWidth);

	// Set logo sprite size
	backgroundSprite_->SetSize(textureWidth/2, textureHeight/2);

	// Set logo sprite hot spot
	backgroundSprite_->SetHotSpot(textureWidth / 4, textureHeight / 4);

	// Set logo sprite alignment
	backgroundSprite_->SetAlignment(HA_CENTER, VA_CENTER);

	// Make logo not fully opaque to show the scene underneath
	backgroundSprite_->SetOpacity(1.00f);

	// Set a low priority for the logo so that other UI elements can be drawn on top
	backgroundSprite_->SetPriority(-100);


	// Set the loaded style as default style
	uiRoot_->SetDefaultStyle(style);

	GetSubsystem<Engine>()->RunFrame();

	// Subscribe HandleUpdate() function for processing update events
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(SplashState, HandleUpdate));

	// Call base class implementation
	return State::Begin();;
}

void SplashState::End()
{
	uiRoot_->RemoveChild(backgroundSprite_);
	UnsubscribeFromAllEvents();
	// Call base class implementation
	State::End();
}

void SplashState::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	if (active_ && timer_.Expired())
	{
		active_ = false;
		stateManager_->PopStack();
		stateManager_->PushToStack(nextStateID_);
	}
}

void SplashState::SetNextState(const char* id)
{
	nextStateID_ = id;
}


