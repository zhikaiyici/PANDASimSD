
#include "PANDASimStackingAction.hh"
#include "PANDASimEventAction.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4EventManager.hh"
#include "G4OpticalPhoton.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimStackingAction::PANDASimStackingAction(PANDASimEventAction* eventAction, PANDASimTrackingAction* trackingAction)
	: G4UserStackingAction(), 
	fEventAction(eventAction), fTrackingAction(trackingAction),
	timeInterval(8. * us)
{
	// Define detector command directory using generic messenger class
	fMessenger = new G4GenericMessenger(this, "/time/", "Time interval.");

	auto& timeIntervalCMD = fMessenger->DeclarePropertyWithUnit("timeInterval", "us", timeInterval, "Set time interval between prompt and delayed signals.");
	timeIntervalCMD.SetParameterName("timeInterval", true);
	timeIntervalCMD.SetDefaultValue("8.");
	timeIntervalCMD.SetRange("timeInterval > 0");
}

PANDASimStackingAction::~PANDASimStackingAction()
{
	delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack PANDASimStackingAction::ClassifyNewTrack(const G4Track* track)
{
	if (track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhoton()) return fUrgent;
	G4int parentID = track->GetParentID();
	if (parentID == 0) return fUrgent;
	const G4ParticleDefinition* particleDefinition = track->GetParticleDefinition();
	const G4String& strPrtclName = particleDefinition->GetParticleName();
	const G4String processName = track->GetCreatorProcess()->GetProcessName();
	if (processName == "nCapture" || (processName == "Decay" && (strPrtclName == "mu+" || strPrtclName == "mu-")))
	{
		// G4cout << "waiting" << G4endl;
		return fWaiting;
	}
	else
	{
		auto secondariesTimes = fTrackingAction->GetSecondariesTime();
		G4int i = secondariesTimes->size() - fTrackingAction->GetSecondariesNumber();
		fTrackingAction->SetSecondariesNumber();
		//G4cout << "secondariesTimes->at(i): " << i << ": " << secondariesTimes->at(i) / us << G4endl;
		if (secondariesTimes->at(i) > timeInterval && parentID > 1)
		{
			//G4cout << "waiting" << G4endl;
			return fWaiting;
		}
		else
			return fUrgent;
	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimStackingAction::NewStage()
{
	// This method is called by G4StackManager when the urgentStack
	// becomes empty and contents in the waitingStack are transferred
	// to the urgentStack.
	fEventAction->SetDelayedFlag(true);
}