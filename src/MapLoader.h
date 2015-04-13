
#pragma once

#include "Object.h"
#include "Context.h"
#include "Vector2.h"
#include "Component.h"
#include "Scene.h"

using namespace Urho3D;

class MapLoader: public Component
{
    OBJECT(MapLoader);
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
