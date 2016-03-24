
#pragma once

#include "Urho3D/Core/Object.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Math/Vector2.h"
#include "Urho3D/Scene/Component.h"
#include "Urho3D/Scene/Scene.h"

using namespace Urho3D;

class MapLoader: public Component
{
    URHO3D_OBJECT(MapLoader, Component);
public:
    MapLoader(Context* context);
    static void RegisterObject(Context* context);
    void Load(String path);
private:
    Node* nodeWall;
    void CreatePlatform(Vector2 p1, Vector2 p2, String typeplatform);
    void CreateMovablePlatform(Vector2 p1, Vector2 p2);
    void CreatePlayer(Vector2 pos);
    void CreateEnemy(Vector2 pos);
};
