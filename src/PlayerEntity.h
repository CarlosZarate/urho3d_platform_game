
#pragma once
#include "LogicComponent.h"
#include "Controls.h"
#include "Node.h"
#include "Vector.h"

// All Urho3D classes reside in namespace Urho3D
namespace Urho3D
{
	EVENT(E_PLAYERHURT, PlayerHurt)
	{
		PARAM(P_ISDEAD, IsDead);                 // bool
	}
}

using namespace Urho3D;

const int CTRL_UP = 1;
const int CTRL_DOWN= 2;
const int CTRL_LEFT = 4;
const int CTRL_RIGHT = 8;
const int LOOK_LEFT = 16;
const int CHANGE_TARGET = 32;

const float MOVE_FORCE = 3.0f;

class PlayerEntity : public LogicComponent
{
	OBJECT(PlayerEntity);
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	PlayerEntity(Context* context);
	~PlayerEntity();

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
    void SetJump();
    void SetAtack();
    void SetHurt(Vector2 pos);
    void SetHeart(int count);

	/// Movement controls. Assigned by the main program each frame.
    Controls controls_;
private:
    bool CastGround();
    void ReduceHeart();

    bool isDead = false;
    bool isjump = true;
    bool isAtack = false;
    float timeanim = 0;
    float isBusy = false;
    float timeBusy = 0;
    int CountHeart = 0;
};
