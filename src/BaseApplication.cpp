
#include "Sprite.h"
#include "Console.h"
#include "Cursor.h"
#include "DebugHud.h"
#include "Engine.h"
#include "FileSystem.h"
#include "Graphics.h"
#include "Input.h"
#include "ResourceCache.h"
#include "Scene.h"

#include "BaseApplication.h"

BaseApplication::BaseApplication(Context* context) :
    Application(context)
{
}

void BaseApplication::Setup()
{
    // Modify engine startup parameters
    engineParameters_["WindowTitle"] = "April's Quest";
    engineParameters_["LogName"]     = GetTypeName() + ".log";
    engineParameters_["VSync"] = true;
    engineParameters_["FrameLimiter"] = 60;
    engineParameters_["FullScreen"]  = false;
    engineParameters_["Headless"]    = false;
    engineParameters_["WindowWidth"] = 800;
    engineParameters_["WindowHeight"]= 600;
    engineParameters_["LogQuiet"] = false;
}

void BaseApplication::Start()
{
    // Set custom window Title & Icon
    SetWindowTitleAndIcon();

    // Subscribe key down event
    SubscribeToEvent(E_KEYDOWN, HANDLER(BaseApplication, HandleKeyDown));

    // Create console and debug HUD
    CreateConsoleAndDebugHud();
}

void BaseApplication::Stop()
{
    engine_->DumpResources(true);
}

void BaseApplication::SetWindowTitleAndIcon()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Graphics* graphics = GetSubsystem<Graphics>();
    Image* icon = cache->GetResource<Image>("Textures/UrhoIcon.png");
    graphics->SetWindowIcon(icon);
    graphics->SetWindowTitle("Urho3D Sample");
}

void BaseApplication::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    // Close console (if open) or exit when ESC is pressed
    if (key == KEY_ESC)
    {
        engine_->Exit();
    }// Toggle console with F1
    else if (key == KEY_F1)
        GetSubsystem<Console>()->Toggle();

    // Toggle debug HUD with F2
    else if (key == KEY_F2)
        GetSubsystem<DebugHud>()->ToggleAll();

    else if (key == '9')
        {
            Graphics* graphics = GetSubsystem<Graphics>();
            Image screenshot(context_);
            graphics->TakeScreenShot(screenshot);
            // Here we save in the Data folder with date and time appended
            screenshot.SavePNG(GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Screenshot_" +
                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
        }
}

void BaseApplication::CreateConsoleAndDebugHud()
{
    // Get default style
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);
}
