//
// Copyright (c) 2008-2013 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "SplashState.h"
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
	SubscribeToEvent(E_UPDATE, HANDLER(SplashState, HandleUpdate));

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


