
#include "PANDASimTrackingAction.hh"
#include "PANDASimDetectorConstruction.hh"

#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalPhoton.hh"
#include "G4Neutron.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimTrackingAction::PANDASimTrackingAction(PANDASimRunAction* runAction)
	: G4UserTrackingAction(), 
	flagHe8(false), flagLi9(false), flagNeutron(false),
	parentTime(0.),
	secondariesNumber(0),
	fRunAction(runAction)
{
	secondariesTime = new std::vector<G4double>;
	arraySize = (static_cast<const PANDASimDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction()))->GetArrySize();
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
		//if (IsInPlasticScintillator(track))
		//{
		//	if (track->GetParticleDefinition() == G4Neutron::Neutron())
		//		fRunAction->AddNNeutron();
		//}
	}
}

void PANDASimTrackingAction::PostUserTrackingAction(const G4Track* track)
{
	if (track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhoton()) return;

	// When optical-photons are generated, current track will be suspended 
	// with stopping code 4(fSuspend, defined in  G4TrackStatus.hh) and
	// PostUserTrackingAction() will be invoked. StackingAction::ClassifyNewTrack()
	// will be invoked to decide which stack this suspended track should be stored into.
	// 
	// This if() code block fixs bug of overrange in StackingAction::ClassifyNewTrack()
	// in a specific situation: current track generates non-optical-photons before optical-photons.
	// This if() code block's logic: The suspended track is treated as its own secondray track with generated time = 0.
	if (track->GetTrackStatus() == fSuspend)
		secondariesTime->push_back(0.);

	auto secondariesTrack = track->GetStep()->GetSecondary();
	for (auto aTrack : *secondariesTrack)
	{
		if (aTrack->GetParticleDefinition() != G4OpticalPhoton::OpticalPhoton())
		{
			G4double secondaryTime = aTrack->GetGlobalTime();
			secondariesTime->push_back(secondaryTime - parentTime);
		}
	}
	secondariesNumber = secondariesTime->size();

	std::vector<std::vector<G4int>> nNeutron = std::vector(arraySize, std::vector<G4int>(arraySize, 0));
	for (auto aTrack : *secondariesTrack)
	{
		// aTrack doesn't have LogicalVolumeAtVertex object at this moment
		// It is in the state of stepNumber = 0.
		if (IsInPlasticScintillator(aTrack))
		{
			if (aTrack->GetParticleDefinition() == G4Neutron::Neutron())
			{
				const G4VTouchable* preTouch = aTrack->GetTouchable();
				const G4int moduleRowReplicaNumber = preTouch->GetReplicaNumber(2);
				const G4int moduleRepliaNumber = preTouch->GetReplicaNumber(1);
				nNeutron[moduleRepliaNumber][moduleRowReplicaNumber]++;
				fRunAction->AddCorrectedNNeutron();
			}
		}
	}
	if (IsVertexInPlasticScintillator(track) && IsInPlasticScintillator(track) && track->GetParticleDefinition() == G4Neutron::Neutron())
	{
		auto currentSecondaries = track->GetStep()->GetSecondaryInCurrentStep();
		for (auto aTrack : *currentSecondaries)
		{
			//G4String processName = aTrack->GetCreatorProcess()->GetProcessName();
			//if (processName == "neutronInelastic")
			//{
			//}
			if (aTrack->GetParticleDefinition() == G4Neutron::Neutron())
			{
				const G4VTouchable* preTouch = track->GetTouchable();
				const G4int moduleRowReplicaNumber = preTouch->GetReplicaNumber(2);
				const G4int moduleRepliaNumber = preTouch->GetReplicaNumber(1);
				nNeutron[moduleRepliaNumber][moduleRowReplicaNumber]--;
				fRunAction->SubtractNNeutron();
				break;
			}
		}
	}
	fRunAction->AddCorrectedNNeutron(nNeutron);

	auto processDefinedStep = track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep();
	G4String processName = "";
	if (processDefinedStep)
		processName = processDefinedStep->GetProcessName();

	//G4RadioactiveDecay is replaced with G4Radioactivation since 11.1.3
	if (processName == "Radioactivation" || processName == "RadioactiveDecay") 
	{
		G4String particleName = track->GetParticleDefinition()->GetParticleName();
		const std::vector<const G4Track*>* currentSecondaries = track->GetStep()->GetSecondaryInCurrentStep();
		G4double decayTime = track->GetLocalTime() / us;
		if (particleName == "Li9")
		{
			fRunAction-> PushDecayTimeLi9(decayTime);
			//G4cout << "Li9 time decay:" << decayTime << G4endl;
			if (currentSecondaries->size() > 0)
			{
				for (auto itr = currentSecondaries->begin(); itr != currentSecondaries->end(); ++itr)
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
			if (currentSecondaries->size() > 0)
			{
				for (auto itr = currentSecondaries->begin(); itr != currentSecondaries->end(); ++itr)
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
	auto& volumeName = track->GetVolume()->GetLogicalVolume()->GetName();
	if (volumeName == "PlasticScintillatorLV")
		return true;
	else
		return false;
}

G4bool PANDASimTrackingAction::IsVertexInPlasticScintillator(const G4Track* track)
{
	auto& volumeName = track->GetLogicalVolumeAtVertex()->GetName();
	if (volumeName == "PlasticScintillatorLV")
		return true;
	else
		return false;
}