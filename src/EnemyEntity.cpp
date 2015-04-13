
#include "iostream"

#include "Context.h"
#include "Component.h"
#include "Node.h"
#include "RigidBody2D.h"
#include "AnimatedSprite2D.h"
#include "AnimationSet2D.h"
#include "PhysicsEvents2D.h"
#include "BulletEntity.h"
#include "StaticSprite2D.h"
#include "ResourceCache.h"
#include "Scene.h"
#include "CollisionCircle2D.h"
#include "RigidBody2D.h"
#include "Input.h"
#include "Vector3.h"
#include "Graphics.h"
#include "Camera.h"
#include "Color.h"
#include "math.h"
#include "DebugRenderer.h"
#include "PhysicsWorld2D.h"
#include "StaticSprite2D.h"

#include "EnemyEntity.h"

EnemyEntity::EnemyEntity(Context* context) : LogicComponent(context)
{
	SetUpdateEventMask(USE_UPDATE);
}

EnemyEntity::~EnemyEntity()
{
}

void EnemyEntity::RegisterObject(Context* context)
{
	context->RegisterFactory<EnemyEntity>();
}

void EnemyEntity::Start()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    AnimationSet2D* animationSet = cache->GetResource<AnimationSet2D>("Urho2D/tortuga2.scml");
    if (!animationSet)
        return;

    AnimatedSprite2D* animatedSprite = node_->CreateComponent<AnimatedSprite2D>();
    animatedSprite->SetLayer(2);
    // Set animation
    animatedSprite->SetAnimation(animationSet, "walk");
    animatedSprite->SetSpeed(1.0f);

    RigidBody2D* bodysprite = node_->CreateComponent<RigidBody2D>();
    bodysprite->SetBodyType(BT_DYNAMIC);
    bodysprite->SetFixedRotation(true);
    //bodysprite->SetBullet(false);
    //bodysprite->SetLinearVelocity(Vector2::ZERO);
    CollisionCircle2D* circle = node_->CreateComponent<CollisionCircle2D>();
    // Set radius
    circle->SetRadius(1.2f);
    circle->SetDensity(1.0f);
    circle->SetFriction(0.4f);
    circle->SetRestitution(0.0f);
    circle->SetCenter(0,0.35f);
    circle->SetCategoryBits(8192);
    circle->SetMaskBits(65533);

    vel = Vector2(-2.0f,0);
}

void EnemyEntity::DelayedStart()
{

}

void EnemyEntity::Stop()
{
}

void EnemyEntity::Update(float timeStep)
{
    if(isBusy && isDead)
    {
        CurrentTime+= timeStep;
        if(CurrentTime > timeChargue)
        {
            using namespace EnemyDied;
            VariantMap& eventData = GetEventDataMap();
            eventData[P_NODE] = node_;
            SendEvent(E_ENEMYDIED, eventData);
            isBusy = false;
        }
        return;
    }

    if(isDead)
        return;

    UpdateCast();
    RigidBody2D* body = GetComponent<RigidBody2D>();
    float y = body->GetLinearVelocity().y_;

    if(isBusy)
    {
        CurrentTime+= timeStep;
        if(isCharge)
        {
            if(CurrentTime > timeChargue)
            {
                isCharge = false;
                Shoot();
                isCooldDown = true;
                timeCoolDown = 1;
            }
        }
        if(CurrentTime > timeBusy)
        {
            isBusy = false;
            CurrentTime = 0;
        }
    }
    else
    {
        AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
        if(isCooldDown)
        {
            if(animatesprite->GetAnimation()!= "idle")
            {
                animatesprite->SetAnimation("idle", LM_DEFAULT);
            }
            CurrentTime+= timeStep;
            if(CurrentTime > timeCoolDown)
                isCooldDown = false;
        }
        else
        {
            if(animatesprite->GetAnimation()!= "walk")
            {
                animatesprite->SetAnimation("walk", LM_DEFAULT);
            }
            body->SetLinearVelocity(Vector2::ZERO);
            body->SetLinearVelocity(Vector2(vel.x_, y));
        }
    }
}

void EnemyEntity::UpdateCast()
{
    DebugRenderer* debug = GetScene()->GetComponent<DebugRenderer>();
    PhysicsWorld2D* physicsWorld = GetScene()->GetComponent<PhysicsWorld2D>();
    RigidBody2D* body = GetComponent<RigidBody2D>();

    /*cast ground*/
    Vector2 originpos = node_->GetPosition2D();
    Vector2 FinishC;
    Vector2 FinishD;

    if(!isLeft)
    {
        FinishC = originpos + Vector2(-0.60,-0.35);
        FinishD = originpos + Vector2(-0.60,0);
    }

    else
    {
        FinishC = originpos + Vector2(0.60,-0.35);
        FinishD = originpos + Vector2(0.60,0);
    }


    PhysicsRaycastResult2D RayCastC;
    physicsWorld->RaycastSingle(RayCastC, originpos , FinishC,1);
    if ( !RayCastC.body_ && body->GetLinearVelocity().y_ >= -0.05f)
    {
        SetReturn();
    }

    PhysicsRaycastResult2D RayCastD;
    physicsWorld->RaycastSingle(RayCastD, originpos , FinishD,1);
    if ( RayCastD.body_)
    {
        SetReturn();
    }
    /*end cast ground*/

    /*cast player*/
    if(!isBusy && !isCooldDown)
    {
        Node* target = GetScene()->GetChild("Player",false);
        if(target)
        {
            Vector2 targetpos = target->GetPosition2D();
            Vector2 direction = targetpos - originpos;
            direction.Normalize();
            Vector2 FinishT = originpos + Vector2(direction.x_*2.5f,direction.y_*2.5);
            PhysicsRaycastResult2D RayCastT;
            physicsWorld->RaycastSingle(RayCastT, originpos , FinishT, 4);
            if (RayCastT.body_)
            {
                Node* node = RayCastT.body_->GetNode();
                if(node->GetName() == "Player")
                {
                    if(direction.x_ < 0)
                        setRight();
                    else
                        setLeft();
                    SetAtack();
                }
            }

            debug->AddLine( Vector3(originpos.x_, originpos.y_,0),
                        Vector3(FinishT.x_, FinishT.y_,0),
                        Color(0, 1, 1, 1),
                        false );

        }

    }

    /*end castplayer*/
    debug->AddLine( Vector3(originpos.x_, originpos.y_,0),
                            Vector3(FinishC.x_, FinishC.y_,0),
                            Color(1, 0, 0, 1),
                            false );

    debug->AddLine( Vector3(originpos.x_, originpos.y_,0),
                            Vector3(FinishD.x_, FinishD.y_,0),
                            Color(1, 0, 0, 1),
                            false );

}

void EnemyEntity::SetAtack()
{
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    if(animatesprite->GetAnimation()!= "atack")
    {
        animatesprite->SetAnimation("atack", LM_FORCE_CLAMPED);
        isBusy = true;
        isCharge = true;
        CurrentTime = 0;
        timeBusy = 1.0f;
        timeChargue = 0.8f;
    }
}

void EnemyEntity::SetHurt(Vector2 pos)
{
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    if(animatesprite->GetAnimation()!= "hurt")
    {
        animatesprite->SetAnimation("hurt", LM_FORCE_CLAMPED);
        isBusy = true;
        CurrentTime = 0;
        life-=2;
        if(life >= 0)
        {
            timeBusy = 0.26f;
            RigidBody2D* body = GetComponent<RigidBody2D>();
            if(pos.x_>node_->GetPosition2D().x_)
                body->ApplyLinearImpulse(Vector2(-0.5f,0.5f),node_->GetPosition2D(),true );
            else
                body->ApplyLinearImpulse(Vector2(0.5f,0.5f),node_->GetPosition2D(),true );
        }
        else{
            isDead = true;
            animatesprite->SetAnimation("dead", LM_FORCE_CLAMPED);
            timeBusy = 2.0f;
            node_->RemoveComponent<RigidBody2D>();
        }

    }
}


void EnemyEntity::Shoot()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Sprite2D* bulletSprite = cache->GetResource<Sprite2D>("Urho2D/bullet2.png");
    if (!bulletSprite)
        return;

	SharedPtr<Node> bulletNode_(GetScene()->CreateChild("e_bullet"));
	bulletNode_->SetPosition2D(node_->GetPosition2D());
	//bulletNode_->SetDirection(Vector3(dir.x_,-dir.y_,0));
	StaticSprite2D* staticSprite = bulletNode_->CreateComponent<StaticSprite2D>();
	staticSprite->SetSprite(bulletSprite);
	staticSprite->SetLayer(6 * 10);

	RigidBody2D* bulletBody = bulletNode_->CreateComponent<RigidBody2D>();
    bulletBody->SetBodyType(BT_DYNAMIC);
    bulletBody->SetFixedRotation(true);
    bulletBody->SetBullet(true);

    CollisionCircle2D* circleshape = bulletNode_->CreateComponent<CollisionCircle2D>();
    circleshape->SetRadius(0.08f);
    circleshape->SetDensity(1.0f);
    circleshape->SetFriction(0.0f);
    circleshape->SetRestitution(0.0f);
    circleshape->SetMaskBits(57340);
    circleshape->SetCategoryBits(2);

	bulletNode_->CreateComponent<BulletEntity>();

	Node* nodebullet2 = bulletNode_->Clone();
	Node* nodebullet3 = bulletNode_->Clone();
	Node* nodebullet4 = bulletNode_->Clone();

    nodebullet2->GetComponent<RigidBody2D>()->ApplyLinearImpulse(Vector2(0.030f,0.13f),node_->GetPosition2D(),true );
    nodebullet3->GetComponent<RigidBody2D>()->ApplyLinearImpulse(Vector2(-0.02f,0.13f),node_->GetPosition2D(),true );
    nodebullet4->GetComponent<RigidBody2D>()->ApplyLinearImpulse(Vector2(-0.030f,0.13f),node_->GetPosition2D(),true );
	bulletBody->ApplyLinearImpulse(Vector2(0.02f,0.13f),node_->GetPosition2D(),true );
}

void EnemyEntity::setRight()
{
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    animatesprite->SetFlipX(false);
    isLeft = false;
    vel.x_ = -0.6f;
}

void EnemyEntity::setLeft()
{
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    animatesprite->SetFlipX(true);
    vel.x_ = 0.6f;
    isLeft = true;
}

void EnemyEntity::SetReturn()
{
    if(isLeft)
        setRight();
    else
        setLeft();
}
