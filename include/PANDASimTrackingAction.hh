
#ifndef PANDASimTrackingAction_h
#define PANDASimTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"

class PANDASimRunAction;

class PANDASimTrackingAction : public G4UserTrackingAction
{
public:
	PANDASimTrackingAction(PANDASimRunAction* runAction);
	~PANDASimTrackingAction() {};

	virtual void PreUserTrackingAction(const G4Track* track);
	virtual void PostUserTrackingAction(const G4Track* track);
	
	inline void SetFlagHe8(G4bool f) { flagHe8 = f; }
	inline void SetFlagLi9(G4bool f) { flagLi9 = f; }

	inline G4bool GetFlagHe8() const { return flagHe8; }
	inline G4bool GetFlagLi9() const { return flagLi9; }

	inline PANDASimRunAction* GetPANDASimRunAction() const { return fRunAction; }

private:
	PANDASimRunAction* fRunAction;

	G4bool flagHe8;
	G4bool flagLi9;

	G4bool IsInPlasticScintillator(const G4Track* track);

};


#endif