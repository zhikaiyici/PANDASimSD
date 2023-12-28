#include "PANDASimGdFilmSD.hh"

#include "globals.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimGdFilmSD::PANDASimGdFilmSD(const G4String& name, const G4String& hitsCollectionName, G4int nofHits)
	: G4VSensitiveDetector(name),
	fHitsCollection(nullptr), fEventAction(nullptr),
	fHitsNum(nofHits)
{
	collectionName.insert(hitsCollectionName);
}

PANDASimGdFilmSD::~PANDASimGdFilmSD()
{
}

void PANDASimGdFilmSD::Initialize(G4HCofThisEvent* hce)
{
	// Create hits collection
	fHitsCollection
		= new PANDASimGdFilmHitsCollection(SensitiveDetectorName, collectionName[0]);

	// Add this collection in hce
	G4int hcID
		= G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	hce->AddHitsCollection(hcID, fHitsCollection);

	// Create hits and fill  hits with zero
	for (G4int i = 0; i < fHitsNum; i++)
	{
		fHitsCollection->insert(new PANDASimGdFilmHit());
	}
}

G4bool PANDASimGdFilmSD::ProcessHits(G4Step* step, G4TouchableHistory* history)
{
	G4Track* theTrack = step->GetTrack();
	const G4ParticleDefinition* particleDefinition = theTrack->GetDynamicParticle()->GetParticleDefinition();
	const G4String& strPrtclName = particleDefinition->GetParticleName();

	//if (strPrtclName == "Gd156")
	//{
	//	getchar();
	//}

	if (strPrtclName == "opticalphoton") return false;

	const G4VTouchable* preTouch = step->GetPreStepPoint()->GetTouchable();
	const G4int moduleRowReplicaNumber = preTouch->GetReplicaNumber(2);
	const G4int moduleRepliaNumber = preTouch->GetReplicaNumber(1);
	G4int moduleNumber = moduleRepliaNumber * sqrt(fHitsNum) + moduleRowReplicaNumber;

	// Get hit accounting data for this cell
	auto hit = (*fHitsCollection)[moduleNumber];
	if (!hit) {
		G4ExceptionDescription msg;
		msg << "Cannot access hit " << moduleNumber;
		G4Exception("PANDASimGdFilmSD::ProcessHits()",
			"MyCode0004", FatalException, msg);
	}
	if (!fEventAction)
	{
		fEventAction = static_cast<PANDASimEventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
	}

	// 判断是否为中子
	if (strPrtclName == "neutron")
	{
		//G4cout << "particle name : " << strPrtclName << G4endl;
		//G4cout << "particle created process : " << theTrack->GetCreatorProcess()->GetProcessName() << G4endl;
		//G4cout << "track id: " << theTrack->GetTrackID() << G4endl;
		//G4cout << "parent id: " << theTrack->GetParentID() << G4endl;
		//G4cout << "gd here" << G4endl;
		//getchar();
		const G4StepPoint* postStepPoint = step->GetPostStepPoint();
		const G4String processName = postStepPoint->GetProcessDefinedStep()->GetProcessName();
		//const G4String processName_ = preStepPoint->GetProcessDefinedStep()->GetProcessName();
		//G4cout << "processName: " << processName << G4endl;
		if (processName == "nCapture")
		{
			//const G4double capTimeGd1 = postStepPoint->GetGlobalTime() / us;
			const G4double capTimeGd = postStepPoint->GetLocalTime() / us;
			hit->TimeGd(capTimeGd);
			fEventAction->SetDelayFlagGd(true);
			//G4cout << "global capTimeGd1: " << capTimeGd1 << G4endl;
			//G4cout << "local capTimeGd: " << capTimeGd << G4endl;
		}
	}
	return true;
}

void PANDASimGdFilmSD::EndOfEvent(G4HCofThisEvent* hitCollection)
{
}
