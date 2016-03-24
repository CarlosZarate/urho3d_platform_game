#include "PlayerEntity.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Component.h"
#include "Urho3D/Scene/Node.h"
#include "Urho3D/Urho2D/RigidBody2D.h"
#include "iostream"
#include "Urho3D/Urho2D/AnimatedSprite2D.h"
#include "Urho3D/Urho2D/AnimationSet2D.h"
#include "Urho3D/Urho2D/PhysicsEvents2D.h"
#include "BulletEntity.h"
#include "Urho3D/Urho2D/StaticSprite2D.h"
#include "Urho3D/Resource/ResourceCache.h"
#include "Urho3D/Scene/Scene.h"
#include "Urho3D/Urho2D/CollisionCircle2D.h"
#include "Urho3D/Urho2D/CollisionPolygon2D.h"
#include "Urho3D/Urho2D/ConstraintWheel2D.h"
#include "Urho3D/Urho2D/RigidBody2D.h"
#include "Urho3D/Input/Input.h"
#include "Urho3D/Math/Vector3.h"
#include "Urho3D/Graphics/Graphics.h"
#include "Urho3D/Graphics/Camera.h"
#include "Urho3D/Urho2D/PhysicsWorld2D.h"
#include "Urho3D/Graphics/DebugRenderer.h"
#include "EnemyEntity.h"
#include <math.h>
#include "Box2D/Box2D.h"

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

PlayerEntity::PlayerEntity(Context* context) : LogicComponent(context)
{
	SetUpdateEventMask(USE_UPDATE);
}

PlayerEntity::~PlayerEntity()
{
}

void PlayerEntity::RegisterObject(Context* context)
{
	context->RegisterFactory<PlayerEntity>();
}

void PlayerEntity::Start()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    AnimationSet2D* animationSet = cache->GetResource<AnimationSet2D>("Urho2D/gladiador.scml");
    if (!animationSet)
        return;

    AnimatedSprite2D* animatedSprite = node_->CreateComponent<AnimatedSprite2D>();
    animatedSprite->SetLayer(-1);
    // Set animation
    animatedSprite->SetAnimationSet(animationSet);
    animatedSprite->SetAnimation("idle");
    animatedSprite->SetSpeed(1.5f);

    RigidBody2D* bodysprite = node_->CreateComponent<RigidBody2D>();
    bodysprite->SetBodyType(BT_DYNAMIC);
    bodysprite->SetFixedRotation(true);

    PODVector<Vector2> vertices;
    vertices.Push(Vector2(-1,3));
    vertices.Push(Vector2(1,3));
    vertices.Push(Vector2(-1,-1));
    vertices.Push(Vector2(1,-1));

    CollisionPolygon2D* box = node_->CreateComponent<CollisionPolygon2D>();
    box->SetVertices(vertices);
    box->SetDensity(1.0f);
    box->SetFriction(0.0f);
    box->SetRestitution(0.1f);
    box->SetMaskBits(65534);
    box->SetCategoryBits(16384);

    CollisionCircle2D* circle = node_->CreateComponent<CollisionCircle2D>();
    circle->SetCenter(0,-0.6f);
    circle->SetRadius(1.0f);
    circle->SetDensity(1.0f);
    circle->SetFriction(0.7f);
    circle->SetRestitution(0.0f);
    circle->SetMaskBits(65534);
    circle->SetCategoryBits(16384);

    bodysprite->SetLinearVelocity(Vector2::ZERO);
}

void PlayerEntity::DelayedStart()
{

}

void PlayerEntity::Stop()
{
}

void PlayerEntity::Update(float timeStep)
{
    RigidBody2D* body = GetComponent<RigidBody2D>();
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();


    if(isDead)
    {
        if(CastGround())
            node_->RemoveComponent<RigidBody2D>();
        if(animatesprite->GetAnimation()!= "dead")
            animatesprite->SetAnimation("dead", LM_DEFAULT);
        return;
    }

    Vector2 vel = body->GetLinearVelocity();

    if (controls_.IsDown(CTRL_LEFT))
    {
        vel.x_ = -2.0f;
        animatesprite->SetFlipX(false);
    }

    if (controls_.IsDown(CTRL_RIGHT))
    {
        vel.x_ = 2.0f;
        animatesprite->SetFlipX(true);
    }

    if(!isBusy)
    {
        if(CastGround())
        {
            if(!controls_.IsDown(CTRL_RIGHT) && !controls_.IsDown(CTRL_LEFT))
            {
                if(animatesprite->GetAnimation()!= "idle")
                    animatesprite->SetAnimation("idle", LM_DEFAULT);
            }
            else
            {
                if(animatesprite->GetAnimation()!= "run")
                    animatesprite->SetAnimation("run", LM_DEFAULT);
            }
        }
        else
        {
            if(animatesprite->GetAnimation()!= "jumpidle")
                animatesprite->SetAnimation("jumpidle", LM_FORCE_CLAMPED);
        }
    }

    else{
        timeBusy+= timeStep;
        if(timeBusy > timeanim)
        {
            isBusy = false;
            timeBusy = 0;
        }
    }
    body->SetLinearVelocity(Vector2::ZERO);
    body->SetLinearVelocity(vel);

    if (controls_.IsDown(CTRL_UP))
    {
        body->ApplyLinearImpulse(Vector2(0,2.4f),node_->GetPosition2D(),true );
        controls_.Set(CTRL_UP,false);
    }
}

void PlayerEntity::Shoot()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();

    Vector2 dir;

    if(animatesprite->GetFlipX())
        dir = Vector2(1,0);
    else
        dir = Vector2(-1,0);

	SharedPtr<Node> bulletNode_(GetScene()->CreateChild("p_bullet"));
	bulletNode_->SetPosition2D(node_->GetPosition2D());

    AnimationSet2D* animationSet = cache->GetResource<AnimationSet2D>("Urho2D/shooteffects.scml");
    if (!animationSet)
        return;

    AnimatedSprite2D* animatedSprite = bulletNode_->CreateComponent<AnimatedSprite2D>();
    animatedSprite->SetLayer(100);
    animatedSprite->SetAnimationSet(animationSet);
    animatedSprite->SetAnimation("shoot1");
    animatedSprite->SetLoopMode(LM_FORCE_LOOPED);

	RigidBody2D* bulletBody = bulletNode_->CreateComponent<RigidBody2D>();
    bulletBody->SetBodyType(BT_DYNAMIC);
    //bulletBody->SetFixedRotation(true);
    bulletBody->SetBullet(true);
    bulletBody->SetGravityScale(0);

    CollisionCircle2D* circleshape = bulletNode_->CreateComponent<CollisionCircle2D>();
    circleshape->SetRadius(0.08f);
    circleshape->SetDensity(1.0f);
    circleshape->SetFriction(0.0f);
    circleshape->SetRestitution(0.0f);
    circleshape->SetMaskBits(49146);


	/// create Enemy component which controls the Enemy behavior

	bulletBody->SetLinearVelocity(dir*5);
	BulletEntity* b = bulletNode_->CreateComponent<BulletEntity>();
    b->SetDuration(0.25f);
}

void PlayerEntity::SetAtack()
{
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    if(CastGround())
    {
        if(animatesprite->GetAnimation()!= "atack" && animatesprite->GetAnimation()!= "hurt")
        {
            animatesprite->SetAnimation("atack", LM_FORCE_CLAMPED);
            isBusy = true;
            timeBusy = 0;
            timeanim = 0.25f;
            Shoot();
        }
    }
    else
    {
        if(animatesprite->GetAnimation()!= "jumpatack" && animatesprite->GetAnimation()!= "hurt")
        {
            animatesprite->SetAnimation("jumpatack", LM_FORCE_CLAMPED);
            isBusy = true;
            timeBusy = 0;
            timeanim = 0.25f;
            Shoot();
        }
    }
}

void PlayerEntity::SetHurt(Vector2 pos)
{
    if(!CountHeart)
        return;
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    if(animatesprite->GetAnimation()!= "hurt")
    {
        animatesprite->SetAnimation("hurt", LM_FORCE_CLAMPED);
        isBusy = true;
        timeBusy = 0;
        timeanim = 0.26f;
        RigidBody2D* body = GetComponent<RigidBody2D>();
        if(pos.x_>node_->GetPosition2D().x_)
            body->ApplyLinearImpulse(Vector2(-0.5f,0.5f),node_->GetPosition2D(),true );
        else
            body->ApplyLinearImpulse(Vector2(0.5f,0.5f),node_->GetPosition2D(),true );
        ReduceHeart();
    }
}

void PlayerEntity::ReduceHeart()
{
    CountHeart--;
    {
		using namespace PlayerHurt;
        if(!CountHeart)
            isDead = true;
		VariantMap& eventData = GetEventDataMap();
		eventData[P_ISDEAD] = isDead;

		SendEvent(E_PLAYERHURT, eventData);
	}
}

void PlayerEntity::SetHeart(int count)
{
    CountHeart = count;
}

void PlayerEntity::SetJump()
{
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    if(CastGround())
    {
        controls_.Set(CTRL_UP,true);
        if(animatesprite->GetAnimation()!= "jump")
        {
            animatesprite->SetAnimation("jump", LM_FORCE_CLAMPED);
            isBusy = true;
            timeBusy = 0;
            timeanim = 0.35f;
        }
    }
}

bool PlayerEntity::CastGround()
{
    DebugRenderer* debug = GetScene()->GetComponent<DebugRenderer>();
    PhysicsWorld2D* physicsWorld = GetScene()->GetComponent<PhysicsWorld2D>();
    /*cast ground*/
    Vector2 originpos = node_->GetPosition2D();
    Vector2 FinishC = originpos + Vector2(-0.16f,-0.35);
    Vector2 FinishD = originpos + Vector2(0.16f,-0.35);

    PhysicsRaycastResult2D RayCastC;
    physicsWorld->RaycastSingle(RayCastC, originpos , FinishC,1);
    debug->AddLine( Vector3(originpos.x_, originpos.y_,0),
                            Vector3(FinishC.x_, FinishC.y_,0),
                            Color(1, 0, 0, 1),
                            false );
    if ( RayCastC.body_)
    {
        return true;
    }

    PhysicsRaycastResult2D RayCastD;
    physicsWorld->RaycastSingle(RayCastD, originpos , FinishD,1);
    debug->AddLine( Vector3(originpos.x_, originpos.y_,0),
                            Vector3(FinishD.x_, FinishD.y_,0),
                            Color(1, 0, 0, 1),
                            false );
    if ( RayCastD.body_)
    {
        return true;
    }
    return false;
}
