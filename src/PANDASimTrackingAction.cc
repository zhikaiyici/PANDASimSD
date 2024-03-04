
#include "PANDASimTrackingAction.hh"
#include "PANDASimRunAction.hh"

#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimTrackingAction::PANDASimTrackingAction(PANDASimRunAction* runAction)
	: G4UserTrackingAction(), flagHe8(false), flagLi9(false), flagNeutron(false),
	fRunAction(runAction)
{
}

void PANDASimTrackingAction::PreUserTrackingAction(const G4Track* track)
{
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
}

void PANDASimTrackingAction::PostUserTrackingAction(const G4Track* track)
{
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