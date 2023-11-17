#ifndef PANDASimGdFilmHit_h
#define PANDASimGdFilmHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Threading.hh"

class PANDASimGdFilmHit : public G4VHit
{
public:
    PANDASimGdFilmHit();
    PANDASimGdFilmHit(const PANDASimGdFilmHit&);
    virtual ~PANDASimGdFilmHit();

    // operators
    const PANDASimGdFilmHit& operator=(const PANDASimGdFilmHit&);
    G4bool operator==(const PANDASimGdFilmHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print() {}

    // methods to handle data
    void TimeGd(G4double t);

    // get methods
    G4double GetCapTimeGd() const;

private:
    G4double capTimeGd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using PANDASimGdFilmHitsCollection = G4THitsCollection<PANDASimGdFilmHit>;

extern G4ThreadLocal G4Allocator<PANDASimGdFilmHit>* PANDASimGdFilmHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* PANDASimGdFilmHit::operator new(size_t)
{
    if (!PANDASimGdFilmHitAllocator) {
        PANDASimGdFilmHitAllocator = new G4Allocator<PANDASimGdFilmHit>;
    }
    void* hit;
    hit = (void*)PANDASimGdFilmHitAllocator->MallocSingle();
    return hit;
}

inline void PANDASimGdFilmHit::operator delete(void* hit)
{
    if (!PANDASimGdFilmHitAllocator) {
        PANDASimGdFilmHitAllocator = new G4Allocator<PANDASimGdFilmHit>;
    }
    PANDASimGdFilmHitAllocator->FreeSingle((PANDASimGdFilmHit*)hit);
}

inline void PANDASimGdFilmHit::TimeGd(G4double t) {
    capTimeGd = t;
}

inline G4double PANDASimGdFilmHit::GetCapTimeGd() const {
    return capTimeGd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
