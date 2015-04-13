#include "MovablePlatform.h"
#include "iostream"
#include "Node.h"
#include "CollisionPolygon2D.h"
#include "RigidBody2D.h"
#include "Sprite2D.h"
#include "StaticSprite2D.h"
#include "ResourceCache.h"

MovablePlatform::MovablePlatform(Context* context): LogicComponent(context)
{
	SetUpdateEventMask(USE_UPDATE);
}

MovablePlatform::~MovablePlatform()
{

}

void MovablePlatform::RegisterObject(Context* context)
{
	context->RegisterFactory<MovablePlatform>();
}

void MovablePlatform::Start()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    PODVector<Vector2> vertices;
    vertices.Push(Vector2(-0.7f,0.1f));
    vertices.Push(Vector2(0.7f,0.1f));
    vertices.Push(Vector2(0.7f,-0.1f));
    vertices.Push(Vector2(-0.7f,-0.1f));

    Sprite2D* movplatformsprite = cache->GetResource<Sprite2D>("Urho2D/movplatform.png");
    if (!movplatformsprite)
        return;

    StaticSprite2D* movplatformstaticSprite = node_->CreateComponent<StaticSprite2D>();
    movplatformstaticSprite->SetSprite(movplatformsprite);

    RigidBody2D* platfotmbody = node_->CreateComponent<RigidBody2D>();
    platfotmbody->SetBodyType(BT_KINEMATIC);
    platfotmbody->SetFixedRotation(true);

    CollisionPolygon2D* box = node_->CreateComponent<CollisionPolygon2D>();
    box->SetVertices(vertices);
    box->SetDensity(1.0f);
    box->SetFriction(0.4f);
    box->SetRestitution(0.1f);
    box->SetCategoryBits(32768);
}

void MovablePlatform::DelayedStart()
{

}

void MovablePlatform::Stop()
{

}

void MovablePlatform::Update(float timeStep)
{
    RigidBody2D* body = GetComponent<RigidBody2D>();
    if(!isreturned)
    {
        if(node_->GetPosition2D().Length() > pointB_.Length())
        {
            isreturned = true;
            body->SetLinearVelocity(body->GetLinearVelocity()*-1);
        }
    }
    else
    {
        if(node_->GetPosition2D().Length() < pointA_.Length())
        {
            isreturned = false;
            body->SetLinearVelocity(body->GetLinearVelocity()*-1);
        }
    }

}

void MovablePlatform::SetPoints(Vector2 pointA, Vector2 pointB)
{
    RigidBody2D* body = GetComponent<RigidBody2D>();
    if(pointA.Length() < pointB.Length())
    {
        pointA_ = pointA;
        pointB_ = pointB;
    }
    else
    {
        isreturned = true;
        pointB_ = pointA;
        pointA_ = pointB;
    }
    Vector2 dir(pointB - pointA);
    dir.Normalize();
    node_->SetPosition2D(pointA_);
    body->SetLinearVelocity(dir);
}

