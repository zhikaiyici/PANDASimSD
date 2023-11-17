#include "PANDASimPhotocathodeSD.hh"
#include "Spline.hh"

#include "globals.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimPhotocathodeSD::PANDASimPhotocathodeSD(const G4String& name, const G4String& hitsCollectionName, G4int nofHits)
	: G4VSensitiveDetector(name),
	fHitsCollection(nullptr), fEventAction(nullptr),
	fHitsNum(nofHits)
{
	collectionName.insert(hitsCollectionName);
}

PANDASimPhotocathodeSD::~PANDASimPhotocathodeSD()
{
}

void PANDASimPhotocathodeSD::Initialize(G4HCofThisEvent* hce)
{
	// Create hits collection
	fHitsCollection
		= new PANDASimPhotocathodeHitsCollection(SensitiveDetectorName, collectionName[0]);

	// Add this collection in hce
	G4int hcID
		= G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	hce->AddHitsCollection(hcID, fHitsCollection);

	// Create hits and fill  hits with zero
	for (G4int i = 0; i < fHitsNum; i++)
	{
		fHitsCollection->insert(new PANDASimPhotocathodeHit());
	}
}

G4bool PANDASimPhotocathodeSD::ProcessHits(G4Step* step, G4TouchableHistory* history)
{
	G4Track* theTrack = step->GetTrack();
	const G4ParticleDefinition* particleDefinition = theTrack->GetDynamicParticle()->GetParticleDefinition();
	const G4String& strPrtclName = particleDefinition->GetParticleName();

	if (strPrtclName != "opticalphoton") return false;

	const G4VTouchable* preTouch = step->GetPreStepPoint()->GetTouchable();
	const G4int moduleRowReplicaNumber = preTouch->GetReplicaNumber(4);
	const G4int moduleRepliaNumber = preTouch->GetReplicaNumber(3);
	const G4int PMTCopyNumber = preTouch->GetCopyNumber(2);

	G4int PMTNumber = moduleRepliaNumber * sqrt(fHitsNum / 2) + moduleRowReplicaNumber + PMTCopyNumber * fHitsNum / 2;

	auto hit = (*fHitsCollection)[PMTNumber];
	if (!hit) {
		G4ExceptionDescription msg;
		msg << "Cannot access hit " << PMTNumber;
		G4Exception("PANDASimGdFilmSD::ProcessHits()",
			"MyCode0004", FatalException, msg);
	}

	G4ThreadLocalStatic G4double photonEnergy[] =
	{ 1.8338 * eV, 1.8419 * eV, 1.8527 * eV, 1.8721 * eV, 1.8890 * eV, 1.9172 * eV, 1.9530 * eV, 1.9800 * eV,
	  2.0022 * eV, 2.0413 * eV, 2.0845 * eV, 2.1479 * eV, 2.2163 * eV, 2.2922 * eV, 2.4194 * eV, 2.5563 * eV,
	  2.7037 * eV, 2.8891 * eV, 3.0268 * eV, 3.1703 * eV, 3.3728 * eV, 3.6556 * eV, 3.9353 * eV, 4.0806 * eV,
	  4.2007 * eV, 4.2506 * eV };
	G4ThreadLocalStatic const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double);

	G4ThreadLocalStatic G4double quatumnEfficiency[] =
	{ 0.0005, 0.0006, 0.0009, 0.0013, 0.0021, 0.0034, 0.0068, 0.0093, 0.0129, 0.0184, 0.0289, 0.0436,
	  0.0624, 0.0903, 0.1354, 0.1785, 0.2165, 0.2461, 0.2530, 0.2460, 0.2268, 0.1802, 0.1222, 0.0847,
	  0.0510, 0.0387 };
	assert(sizeof(quatumnEfficiency) == sizeof(photonEnergy));

	try
	{
		static G4ThreadLocal SplineSpace::SplineInterface* sp = new SplineSpace::Spline(photonEnergy, quatumnEfficiency, nEntries);
		const G4double photonKE = step->GetPreStepPoint()->GetKineticEnergy();
		G4double pheNum;
		sp->SinglePointInterp(photonKE, pheNum);
		//G4cout << "x =" << photonKE / eV << ": " << peNum << G4endl;
		if (pheNum != 0.)
		{
			if (!fEventAction)
			{
				fEventAction = static_cast<PANDASimEventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
			}
			G4bool delayFlagH = fEventAction->GetDelayFlagH();
			G4bool delayFlagGd = fEventAction->GetDelayFlagGd();
			G4bool decayFlagMu = fEventAction->GetDecayFlagMu();
			if (delayFlagH)
				hit->AddH(pheNum);
			else if (delayFlagGd)
				hit->AddGd(pheNum);
			else if (decayFlagMu)
				hit->AddMu(pheNum);
			else
				hit->Add(pheNum);
		}
		theTrack->SetTrackStatus(fStopAndKill);
	}
	catch (SplineSpace::SplineFailure sf)
	{
		G4cout << sf.GetMessage() << G4endl;
	}

	return true;
}

void PANDASimPhotocathodeSD::EndOfEvent(G4HCofThisEvent* hitCollection)
{
}
