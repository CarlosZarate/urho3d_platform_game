#pragma once
#include "Urho3D/Scene/LogicComponent.h"
#include "Urho3D/Urho2D/StaticSprite2D.h"

// All Urho3D classes reside in namespace Urho3D
namespace Urho3D
{

	/// Enemy died
	URHO3D_EVENT(E_ENEMYDIED, EnemyDied)
	{                                           // bool
		URHO3D_PARAM(P_NODE, NodePtr);                 // Node Ptr
	}
}

using namespace Urho3D;

const int ELOOK_LEFT = 16;
const float EMOVE_FORCE = 1.5f;
const float MAX_TIME = 50;

class EnemyEntity : public LogicComponent
{
	URHO3D_OBJECT(EnemyEntity, LogicComponent);
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	EnemyEntity(Context* context);
	~EnemyEntity();

	/// Register object factory.
	static void RegisterObject(Context* context);

	/// Called when the component is added to a scene node. Other components may not yet exist.
	virtual void Start();
	/// Called before the first update. At this point all other components of the node should exist. Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.
	virtual void DelayedStart();
	/// Called when the component is detached from a scene node, usually on destruction.
	virtual void Stop();
	/// Called on scene update, variable timestep.
	virtual void Update(float timeStep);

	void Shoot();

	void SetTarget();

	void SetUntarget();

	void SetObjetive();

    void GotoPosition(Vector2 to);

    Vector<IntVector2*> route;

    void UpdateCast();
    void SetHurt(Vector2 pos);
    void SetTarget(Node* target);

private:

    void setRight();
    void setLeft();
    void SetReturn();
    void SetAtack();

	Node* target_ = 0;

    Vector2 vel;
    float preenemmyX = 0, preenemmyY = 0;
    bool flagdestinityX = true;
    bool flagdestinityY = true;
    bool gotoflag = false;
    bool findflag = true;
    bool hasrange = true;
    float facVel = 1;
    Vector2 findposition;

    SharedPtr<StaticSprite2D> objectsprite;
    bool isLeft = false;
    bool isBusy = false;
    bool isCooldDown = false;
    float timeCoolDown = 0;
    float timeAtack = 1;
    float timeBusy = 1;
    float CurrentTime = 0;
    bool isCharge = false;
    float timeChargue = 0;
    int life = 8;
    bool isDead = false;
};


