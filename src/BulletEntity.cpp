
#include "iostream"

#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Node.h"
#include "Urho3D/Urho2D/RigidBody2D.h"

#include "BulletEntity.h"

BulletEntity::BulletEntity(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

BulletEntity::~BulletEntity()
{
    //dtor
}

void BulletEntity::Start()
{

}

void BulletEntity::RegisterObject(Context* context)
{
	context->RegisterFactory<BulletEntity>();
}

void BulletEntity::DelayedStart()
{
}

void BulletEntity::Stop()
{
}

void BulletEntity::Update(float timeStep)
{
    RigidBody2D* body = GetComponent<RigidBody2D>();
    Vector2 vel = body->GetLinearVelocity();
    vel.Normalize();
    float angle = fmod(atan2(vel.y_, vel.x_)*180/M_PI,360);
    if(node_->GetName() == "p_bullet")
        node_->SetRotation2D(angle-180);
    else
        node_->SetRotation2D(angle-90);
    if(duration_ != 0)
    {
        currentTime+= timeStep;
        if(currentTime > duration_)
            node_->Remove();
    }
}

void BulletEntity::SetDuration(float duration)
{
    duration_ = duration;
}

