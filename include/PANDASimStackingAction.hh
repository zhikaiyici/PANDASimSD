
#ifndef PANDASimStackingAction_h
#define PANDASimStackingAction_h 1

#include "G4UserStackingAction.hh"
#include "globals.hh"
#include "G4StackManager.hh"
#include "G4GenericMessenger.hh"
#include "PANDASimTrackingAction.hh"

/// Stacking action class : manage the newly generated particles
///
/// One wishes do not track secondary neutrino.Therefore one kills it
/// immediately, before created particles will  put in a stack.

class PANDASimEventAction;

class PANDASimStackingAction : public G4UserStackingAction
{
public:
	PANDASimStackingAction() = default;
	PANDASimStackingAction(PANDASimEventAction* eventAction, PANDASimTrackingAction* trackingAction);
	~PANDASimStackingAction() override;// = default;

	G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*) override;
	void NewStage() override;

	//G4int GetNWaiting() const { return stackManager->GetNWaitingTrack(); }
private:
	PANDASimEventAction* fEventAction;
	PANDASimTrackingAction* fTrackingAction;
	G4GenericMessenger* fMessenger;
	G4double timeInterval;
};


#endif