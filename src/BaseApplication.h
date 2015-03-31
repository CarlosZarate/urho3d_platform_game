#pragma once

#include "Application.h"

namespace Urho3D
{
class Node;
class Scene;
class Sprite;
}

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

class BaseApplication : public Application
{
    // Enable type information.
    OBJECT(BaseApplication);

public:
    /// Construct.
    BaseApplication(Context* context);

    /// Setup before engine initialization. Modifies the engine parameters.
    virtual void Setup();
    /// Setup after engine initialization. Creates the logo, console & debug HUD.
    virtual void Start();

    virtual void Stop();

protected:
    /// Logo sprite.
    SharedPtr<Sprite> logoSprite_;
    /// Scene.
    SharedPtr<Scene> scene_;
    /// Camera scene node.
    SharedPtr<Node> cameraNode_;

private:
    /// Set custom window Title & Icon
    void SetWindowTitleAndIcon();
    /// Handle key down event to process key controls common to all samples.
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    /// Create console and debug HUD.
    void CreateConsoleAndDebugHud();
};
