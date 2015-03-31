//
// Copyright (c) 2008-2013 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "StateManager.h"
#include "Object.h"
#include "CoreEvents.h"
#include "ResourceCache.h"
#include "List.h"
namespace Urho3D
{

	///////////////////////////////////////////////////////////////////////////////
	// AppStateManager Member Functions
	///////////////////////////////////////////////////////////////////////////////

	StateManager::StateManager(Context* context) :Object(context)
	{
		// Initialize variables to sensible defaults
		activeState_ = NULL;
	}

	StateManager::~StateManager()
	{
		// End all currently running states
		Stop();

		// Reset variables
		activeState_.Reset();

		// Destroy any registered states
		registeredStates_.Clear();

		stateStack_.Clear();
	}

	bool StateManager::RegisterState(State * pGameState)
	{
		StateMap::Iterator itState;

		// Validate requirements
		if (!pGameState) return false;

		// State already exists in the state map?
		itState = registeredStates_.Find(pGameState->GetStateId());
		if (itState != registeredStates_.End()) return false;

		// If it doesn't exist, add it to the list of available state types
		registeredStates_[pGameState->GetStateId()] = pGameState;

		// Set up state properties
		pGameState->stateManager_ = this;

		// Initialize the state and return result
		return pGameState->Initialize();
	}

	State * StateManager::GetActiveState()
	{
		return activeState_;
	}

	bool StateManager::SetActiveState(const String & stateId)
	{
		StateMap::Iterator itState;

		// First find the requested state
		itState = registeredStates_.Find(stateId);
		if (itState == registeredStates_.End()) return false;

		// The state was found, end any currently executing states
		if (activeState_) activeState_->End();

		// Link this new state
		activeState_ = itState->second_;

		// Start it running.
		if (activeState_->IsBegun() == false)
			activeState_->Begin();

		// Success!!
		return true;
	}

	State * StateManager::GetState(const String & stateId)
	{
		// First retrieve the details about the state specified, if none
		// was found this is clearly an invalid state identifier.
		StateMap::Iterator itState = registeredStates_.Find(stateId);
		if (itState == registeredStates_.End())
			return NULL;
		return itState->second_;
	}

	void StateManager::Stop()
	{
		// Stop all active states
		if (activeState_) activeState_->End();
	}

	void StateManager::StateEnded(State * pState)
	{
		// Paranoia check :-P
		if (pState == NULL) return;

		// If this was at the root of the state history, we're all out of states
		if (pState == activeState_)
			activeState_ = NULL;


	}

	void StateManager::ClearStack()
	{
		stateStack_.Clear();
	}

	bool StateManager::PushToStack(const String & stateId)
	{
		bool b = SetActiveState(stateId);
		if (b)
			stateStack_.Push(activeState_);
		return b;
	}

	const String& StateManager::PopStack()
	{
		if (stateStack_.Size() != 0)
		{
			// pop active state
			stateStack_.Pop();
			// get the last active state
			State* s = NULL;
			if (stateStack_.Size() != 0)
			{
				s = stateStack_.Back();
			}


			if (s && SetActiveState(s->GetStateId()))
				return s->GetStateId();
		}

		return String::EMPTY;
	}


	///////////////////////////////////////////////////////////////////////////////
	// AppState Member Functions
	///////////////////////////////////////////////////////////////////////////////

	State::State(Context* context) : Object(context)
	{
		// Initialize variables to sensible defaults
		stateManager_ = NULL;
		stateSuspended_ = false;
		begun_ = false;
	}

	State::~State()
	{
		// Clean up
		Dispose();
	}

	void State::Dispose()
	{
		// End the state if it is begun.
		if (begun_ == true)
			End();

		// Clear variables
		stateManager_ = NULL;
		stateSuspended_ = false;

		begun_ = false;
	}

	bool State::IsActive() const
	{
		if (!stateManager_) return false;
		return (this == stateManager_->GetActiveState());
	}

	bool State::IsSuspended() const
	{
		return stateSuspended_;
	}



	const String & State::GetStateId() const
	{
		return stateId_;
	}

	StateManager * State::GetManager()
	{
		return stateManager_;
	}


	bool State::Initialize()
	{

		// Success!
		return true;
	}

	bool State::Begin()
	{
		bool bResult = true;

		if (bResult)
			begun_ = true;
		return bResult;
	}

	void State::End()
	{
		// Notify manager that the state ended
		stateManager_->StateEnded(this);

		// We are no longer attached to anything (simply being stored in
		// the registered state list maintained by the manager).
		stateSuspended_ = false;
		begun_ = false;
	}


	void State::Suspend()
	{
		// Is this a no-op?
		if (stateSuspended_)
			return;

		// Suspend
		stateSuspended_ = true;
	}

	void State::Resume()
	{
		// Is this a no-op?
		if (!stateSuspended_)
			return;

		// Resume
		stateSuspended_ = false;
	}

	void State::SetStateId(const String & stateId)
	{
		stateId_ = stateId;
	}

}
