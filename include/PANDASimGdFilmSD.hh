#ifndef PANDASimGdFilmSD_h
#define PANDASimGdFilmSD_h 1

#include "G4VSensitiveDetector.hh"

#include "PANDASimGdFilmHit.hh"
#include "PANDASimEventAction.hh"

class G4Step;
class G4HCofThisEvent;

/// GdFilm sensitive detector class
///
/// The values are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step.

class PANDASimGdFilmSD : public G4VSensitiveDetector
{
public:
    PANDASimGdFilmSD(const G4String& name,
        const G4String& hitsCollectionName,
        G4int nofHits);
    virtual ~PANDASimGdFilmSD();

    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

private:
    PANDASimGdFilmHitsCollection* fHitsCollection;
    PANDASimEventAction* fEventAction;
    G4int  fHitsNum;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

