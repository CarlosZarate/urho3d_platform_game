
#pragma once
#include "Object.h"
#include "Context.h"
#include "Ptr.h"
#include "List.h"
#include "HashMap.h"

// All Urho3D classes reside in namespace Urho3D
namespace Urho3D
{
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------
class State;

//-----------------------------------------------------------------------------
// State Manager
//-----------------------------------------------------------------------------
class StateManager : public Object
{
	// Enable type information.
	OBJECT(StateManager);
	//-------------------------------------------------------------------------
	// Friend List
	//-------------------------------------------------------------------------
	friend class State;

public:
	//-------------------------------------------------------------------------
	// Constructor & Destructor
	//-------------------------------------------------------------------------
	StateManager(Context* context);
	virtual ~StateManager();

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	void		Stop();

	//  Getters & Setters
	bool		RegisterState(State * pGameState);
	State*		GetState(const String & stateId);
	bool		SetActiveState(const String & stateId);
	State*		GetActiveState();

	// Stack Management
	/// clear the history stack of the states
	void		ClearStack();
	/// add a state to the state stack and SetActiveState. The active state will be stopped (Stop function call)
	/// and the new state will be started (Begin function call)
	/// returns true if state was found and started
	bool		PushToStack(const String & stateId);
	/// the active state will be stopped (Stop function call) and the last state pushed
	/// to the stack will be started (Begin function call)
	/// returns the new active state name/id or empty string
	const String&	PopStack();


private:
	//-------------------------------------------------------------------------
	// Private Structures, Enumerations & Typedefs
	//-------------------------------------------------------------------------
	typedef HashMap<String, SharedPtr<State> > StateMap;
	typedef List<SharedPtr<State> > StateStack;
	//-------------------------------------------------------------------------
	// Private Methods
	//-------------------------------------------------------------------------
	void            StateEnded(State * state);

	//-------------------------------------------------------------------------
	// Private Variables
	//-------------------------------------------------------------------------
	/// All state types that have been registered with the system
	StateMap        registeredStates_;
	/// The currently active state that the game is currently in (restorable states will be available via its parent)
	SharedPtr<State> activeState_;
	/// The State Stack
	StateStack stateStack_;
};


//-----------------------------------------------------------------------------
// State
//-----------------------------------------------------------------------------
class State : public Object
{
	// Enable type information.
	OBJECT(State);
	BASEOBJECT(State);
	//-------------------------------------------------------------------------
	// Friend List
	//-------------------------------------------------------------------------
	friend class StateManager;

public:

	//-------------------------------------------------------------------------
	// Constructor & Destructor
	//-------------------------------------------------------------------------
	State(Context* context);
	virtual ~State();


	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	void			SetStateId(const String & stateId);
	const String&	GetStateId() const;
	bool			IsActive() const;
	bool			IsSuspended() const;
	StateManager*	GetManager();

	//-------------------------------------------------------------------------
	// Public Virtual Methods
	//-------------------------------------------------------------------------
	// State Activation
	/// Init is called directly after state is registered
	virtual bool	Initialize();
	virtual bool	Begin();
	/// after end : We are no longer attached to anything (simply being stored in
	///				the registered state list maintained by the manager).
	virtual void	End();
	// State Management
	virtual void	Suspend();
	virtual void	Resume();
	// Query Routines
	virtual bool	IsBegun() const { return begun_; }
	// delete all resources
	virtual void	Dispose();

protected:
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	StateManager     * stateManager_;          // Manager for this game state object
	String                stateId_;               // The unique identifier of this state as it has been registered with the manager.
	bool                    stateSuspended_;        // Is the state currently suspended?
	bool                    begun_;                 // Has the state already been begun?
};

}
