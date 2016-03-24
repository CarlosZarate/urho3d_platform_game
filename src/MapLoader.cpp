
#include "iostream"
#include "Urho3D/Urho2D/PhysicsWorld2D.h"
#include "Urho3D/Urho2D/CollisionPolygon2D.h"
#include "Urho3D/Urho2D/RigidBody2D.h"
#include "Urho3D/IO/File.h"
#include "Urho3D/Resource/JSONFile.h"
#include "Urho3D/Resource/JSONValue.h"

#include "MovablePlatform.h"
#include "PlayerEntity.h"
#include "EnemyEntity.h"
#include "MapLoader.h"


MapLoader::MapLoader(Context* context): Component(context)
{

}

void MapLoader::RegisterObject(Context* context)
{
	context->RegisterFactory<MapLoader>();
}

void MapLoader::Load(String path)
{
    PhysicsWorld2D* physicsWorld = GetScene()->GetComponent<PhysicsWorld2D>();
    nodeWall = GetScene()->GetChild("NodoWall",false);
    nodeWall->RemoveAllChildren();

    JSONFile* data = new JSONFile(context_);
    File datafile(context_, path);
    data->Load(datafile);
    JSONValue rootjson = data->GetRoot();

    JSONValue poligonsarray = rootjson.GetChild("triangles");
    for(int i = 0 ; i < poligonsarray.GetSize() ; i++)
    {
        JSONValue poligon = poligonsarray.GetChild(i,JSON_ARRAY);
        Node* poligonnode = nodeWall->CreateChild("Wall");
        RigidBody2D* body = poligonnode->CreateComponent<RigidBody2D>();
        body->SetBodyType(BT_STATIC);
        for(int j = 0; j < poligon.GetSize(); j++)
        {
            JSONValue triangle = poligon.GetChild(j,JSON_OBJECT);

            PODVector<Vector2> vertices;
            vertices.Push(triangle.GetVector2("p1_"));
            vertices.Push(triangle.GetVector2("p2_"));
            vertices.Push(triangle.GetVector2("p3_"));

            CollisionPolygon2D* poligonshape = poligonnode->CreateComponent<CollisionPolygon2D>();
            poligonshape->SetVertices(vertices);
            poligonshape->SetDensity(1.0f);
            poligonshape->SetFriction(0.0f);
            poligonshape->SetRestitution(0.0f);
            poligonshape->SetCategoryBits(32768);
        }
    }

    JSONValue platforms = rootjson.GetChild("platforms");
    for(int i = 0 ; i < platforms.GetSize() ; i++)
    {
        JSONValue platformdata = platforms.GetChild(i,JSON_OBJECT);
        String type = platformdata.GetString("type");
        if(type == "movplatform")
            CreateMovablePlatform(platformdata.GetVector2("p1"), platformdata.GetVector2("p2"));
        else
            CreatePlatform(platformdata.GetVector2("p1"),platformdata.GetVector2("p2"),type);
    }

    JSONValue objects = rootjson.GetChild("objects");
    for(int i = 0 ; i < objects.GetSize() ; i++)
    {
        JSONValue objectdata = objects.GetChild(i,JSON_OBJECT);
        String type = objectdata.GetString("type");
        if(type == "enemy")
            CreateEnemy(objectdata.GetVector2("pos"));
    }

    CreatePlayer(rootjson.GetVector2("playerpos"));
}

void MapLoader::CreatePlatform(Vector2 p1, Vector2 p2, String typeplatform)
{
    float mwith = fabs(p2.x_ - p1.x_)/2;
    if(mwith == 0)
        return;
    float mheigth = 0.35f;
    Vector2 pos((p2.x_ + p1.x_)/2, (p2.y_ + p1.y_)/2);

    Node* node  = nodeWall->CreateChild("Wall");
    node->SetPosition2D(pos);

    RigidBody2D* body = node->CreateComponent<RigidBody2D>();
    body->SetBodyType(BT_STATIC);

    PODVector<Vector2> vertices;
    if(typeplatform == "platform")
    {
        vertices.Push(Vector2(-mwith,mheigth/2));
        vertices.Push(Vector2(mwith,mheigth/2));
        vertices.Push(Vector2(mwith,-mheigth));
        vertices.Push(Vector2(-mwith,-mheigth));
    }
    else{
        vertices.Push(Vector2(-mwith,mheigth/2));
        vertices.Push(Vector2(mwith,mheigth/2));
        vertices.Push(Vector2(mwith,0));
        vertices.Push(Vector2(-mwith,0));
    }

    CollisionPolygon2D* box = node->CreateComponent<CollisionPolygon2D>();
    box->SetVertices(vertices);
    box->SetDensity(1.0f);
    box->SetFriction(0.0f);
    box->SetRestitution(0.1f);
    box->SetCategoryBits(32768);

    Node* pnode  = nodeWall->CreateChild("Platform");
    pnode->SetPosition2D(pos);
    RigidBody2D* pbody = pnode->CreateComponent<RigidBody2D>();
    pbody->SetBodyType(BT_STATIC);

    PODVector<Vector2> pvertices;
    pvertices.Push(Vector2(-mwith,mheigth));
    pvertices.Push(Vector2(mwith,mheigth));
    pvertices.Push(Vector2(mwith,mheigth/2));
    pvertices.Push(Vector2(-mwith,mheigth/2));

    CollisionPolygon2D* pbox = pnode->CreateComponent<CollisionPolygon2D>();
    pbox->SetVertices(pvertices);
    pbox->SetDensity(1.0f);
    pbox->SetFriction(0.5f);
    pbox->SetRestitution(0.1f);
    pbox->SetCategoryBits(32768);
}

void MapLoader::CreateMovablePlatform(Vector2 p1, Vector2 p2)
{
    Node* nodeplatform = nodeWall->CreateChild("Platform");
    MovablePlatform* movplat = nodeplatform->CreateComponent<MovablePlatform>();
    movplat->SetPoints(p1, p2);
}

void MapLoader::CreatePlayer(Vector2 pos)
{
    SharedPtr<Node> playernode(GetScene()->CreateChild("Player"));
    playernode->SetPosition2D(pos);
    playernode->SetScale(0.2f);
    playernode->CreateComponent<PlayerEntity>();
}

void MapLoader::CreateEnemy(Vector2 pos)
{
    SharedPtr<Node> enemy(GetScene()->CreateChild("Enemy"));
    enemy->SetScale(0.3f);
    enemy->SetPosition2D(pos);
    EnemyEntity* enemy_ = enemy->CreateComponent<EnemyEntity>();
}
