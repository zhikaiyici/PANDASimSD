
#include "PANDASimStackingAction.hh"
#include "PANDASimEventAction.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimStackingAction::PANDASimStackingAction(PANDASimEventAction* eventAction) 
	: G4UserStackingAction(), fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack PANDASimStackingAction::ClassifyNewTrack(const G4Track* track)
{
	if (track->GetParentID() == 0) return fUrgent;
	const G4ParticleDefinition* particleDefinition = track->GetDynamicParticle()->GetParticleDefinition();
	const G4String& strPrtclName = particleDefinition->GetParticleName();
	const G4String processName = track->GetCreatorProcess()->GetProcessName();
	if (processName == "nCapture" || (processName == "Decay" && (strPrtclName == "mu+" || strPrtclName == "mu-")))
	{
		// G4cout << "waiting" << G4endl;
		return fWaiting;
	}
	else
		return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimStackingAction::NewStage()
{
	// This method is called by G4StackManager when the urgentStack
	// becomes empty and contents in the waitingStack are transferred
	// to the urgentStack.
	fEventAction->SetDelayedFlag(true);
}