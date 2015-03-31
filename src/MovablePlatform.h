#pragma once

#include "Object.h"
#include "Context.h"
#include "LogicComponent.h"
#include "Vector2.h"

using namespace Urho3D;

class MovablePlatform : public LogicComponent
{
OBJECT(MovablePlatform);
public:
    MovablePlatform(Context* context);
    ~MovablePlatform();

    static void RegisterObject(Context* context);
    /// Called when the component is added to a scene node. Other components may not yet exist.
	virtual void Start();
	/// Called before the first update. At this point all other components of the node should exist. Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.
	virtual void DelayedStart();
	/// Called when the component is detached from a scene node, usually on destruction.
	virtual void Stop();
	/// Called on scene update, variable timestep.
	virtual void Update(float timeStep);

	void SetPoints(Vector2 pointA, Vector2 pointB);

private:

    Vector2 pointA_;
    Vector2 pointB_;
    bool isreturned = false;
};
