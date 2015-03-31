#pragma once
#include "LogicComponent.h"

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

class BulletEntity :  public LogicComponent
{
	OBJECT(Bullet);
    public:
        BulletEntity(Context* context);
        ~BulletEntity();
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
        void SetDuration(float duration);
    protected:
    private:
        float duration_ = 0;
        float currentTime = 0;
};
