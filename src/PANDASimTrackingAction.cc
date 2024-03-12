
#include "PANDASimTrackingAction.hh"

#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalPhoton.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimTrackingAction::PANDASimTrackingAction(PANDASimRunAction* runAction)
	: G4UserTrackingAction(), 
	flagHe8(false), flagLi9(false), flagNeutron(false),
	parentTime(0.),
	secondariesNumber(0),
	fRunAction(runAction)
{
	secondariesTime = new std::vector<G4double>;
}

PANDASimTrackingAction::~PANDASimTrackingAction()
{
	secondariesTime->clear();
	delete secondariesTime;
}

void PANDASimTrackingAction::PreUserTrackingAction(const G4Track* track)
{
	if (track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhoton()) return;
	flagHe8 = false;
	flagLi9 = false;
	flagNeutron= false;
	//G4int parentID = track->GetParentID();
	//if (parentID == 0) return;
	//G4String processName = track->GetCreatorProcess()->GetProcessName();
	//G4String particleName = track->GetParticleDefinition()->GetParticleName();
	//if (processName == "RadioactiveDecay" && particleName == "e-")
	//{
	//	G4double betaKE = track->GetKineticEnergy() / keV;
	//	G4cout << "betaKE:" << betaKE << G4endl;
	//}

	secondariesNumber = 0;
	secondariesTime->clear();
	if (track->GetCurrentStepNumber() == 0)
	{
		parentTime = track->GetGlobalTime();
	}
}

void PANDASimTrackingAction::PostUserTrackingAction(const G4Track* track)
{
	if (track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhoton()) return;
	auto secondariesTrack = track->GetStep()->GetSecondary();
	secondariesNumber = secondariesTrack->size();
	for (auto aTrack : *secondariesTrack)
	{
		G4double secondaryTime = aTrack->GetGlobalTime();
		secondariesTime->push_back(secondaryTime - parentTime);
	}
	auto processDefinedStep = track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep();
	G4String processName = "";
	if (processDefinedStep)
		processName = processDefinedStep->GetProcessName();

	//G4RadioactiveDecay is replaced with G4Radioactivation since 11.1.3
	if (processName == "Radioactivation" || processName == "RadioactiveDecay") 
	{
		G4String particleName = track->GetParticleDefinition()->GetParticleName();
		const std::vector<const G4Track*>* secondaries = track->GetStep()->GetSecondaryInCurrentStep();
		G4double decayTime = track->GetLocalTime() / us;
		if (particleName == "Li9")
		{
			fRunAction-> PushDecayTimeLi9(decayTime);
			//G4cout << "Li9 time decay:" << decayTime << G4endl;
			if (secondaries->size() > 0)
			{
				for (auto itr = secondaries->begin(); itr != secondaries->end(); ++itr)
				{
					const G4String name = (*itr)->GetParticleDefinition()->GetParticleName();
					if (name == "e-")
					{
						auto energy = (*itr)->GetKineticEnergy() / keV;
						fRunAction->PushBetaKELi9(energy);
						//G4cout << "betaKE:" << energy << G4endl;
					}
				}
			}
		}
		else if (particleName == "He8")
		{
			fRunAction->PushDecayTimeHe8(decayTime);
			//G4cout << "He8 time decay:" << decayTime << G4endl;
			if (secondaries->size() > 0)
			{
				for (auto itr = secondaries->begin(); itr != secondaries->end(); ++itr)
				{
					const G4String name = (*itr)->GetParticleDefinition()->GetParticleName();
					if (name == "e-")
					{
						auto energy = (*itr)->GetKineticEnergy() / keV;
						fRunAction->PushBetaKEHe8(energy);
						//G4cout << "betaKE:" << energy << G4endl;
					}
				}
			}
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PANDASimTrackingAction::IsInPlasticScintillator(const G4Track* track)
{
	return true;
}