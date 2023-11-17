#ifndef PANDASimPhotocathodeSD_h
#define PANDASimPhotocathodeSD_h 1

#include "G4VSensitiveDetector.hh"

#include "PANDASimPhotocathodeHit.hh"
#include "PANDASimEventAction.hh"

class G4Step;
class G4HCofThisEvent;

class PANDASimPhotocathodeSD : public G4VSensitiveDetector
{
public:
    PANDASimPhotocathodeSD(const G4String& name,
        const G4String& hitsCollectionName,
        G4int nofHits);
    virtual ~PANDASimPhotocathodeSD();

    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

private:
    PANDASimPhotocathodeHitsCollection* fHitsCollection;
    PANDASimEventAction* fEventAction;
    G4int  fHitsNum;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

