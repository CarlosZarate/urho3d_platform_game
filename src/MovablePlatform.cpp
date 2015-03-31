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

    RigidBody2D* platfotmbody = node_->CreateComponent<RigidBody2D>();
    platfotmbody->SetBodyType(BT_KINEMATIC);
    platfotmbody->SetFixedRotation(true);

    PODVector<Vector2> vertices;
    vertices.Push(Vector2(-0.7f,0.2f));
    vertices.Push(Vector2(0.7f,0.2f));
    vertices.Push(Vector2(0.7f,-0.2f));
    vertices.Push(Vector2(-0.7f,-0.2f));

    CollisionPolygon2D* box = node_->CreateComponent<CollisionPolygon2D>();
    box->SetVertices(vertices);
    // Set density
    box->SetDensity(1.0f);
    // Set friction.
    box->SetFriction(0.5f);
    // Set restitution
    box->SetRestitution(0.0f);

    box->SetCategoryBits(32768);

    platfotmbody->SetLinearVelocity(Vector2(1,0));
    Sprite2D* bgsprite = cache->GetResource<Sprite2D>("Urho2D/platform.png");
    if (!bgsprite)
        return;

    StaticSprite2D* bgstaticsprite = node_->CreateComponent<StaticSprite2D>();
    bgstaticsprite->SetSprite(bgsprite);
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
        if(node_->GetPosition2D().x_ > pointB_.x_)
        {
            isreturned = true;
            body->SetLinearVelocity(body->GetLinearVelocity()*-1);
        }
    }
    else
    {
        if(node_->GetPosition2D().x_ < pointA_.x_)
        {
            isreturned = false;
            body->SetLinearVelocity(body->GetLinearVelocity()*-1);
        }
    }

}

void MovablePlatform::SetPoints(Vector2 pointA, Vector2 pointB)
{
    pointA_ = pointA;
    pointB_ = pointB;
    node_->SetPosition2D(pointA_);
}

