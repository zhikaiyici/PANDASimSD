#ifndef PANDASimPhotocathodeHit_h
#define PANDASimPhotocathodeHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Threading.hh"

class PANDASimPhotocathodeHit : public G4VHit
{
public:
    PANDASimPhotocathodeHit();
    PANDASimPhotocathodeHit(const PANDASimPhotocathodeHit&);
    virtual ~PANDASimPhotocathodeHit();

    // operators
    const PANDASimPhotocathodeHit& operator=(const PANDASimPhotocathodeHit&);
    G4bool operator==(const PANDASimPhotocathodeHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print() {}

    // methods to handle data
    void Add(G4double calPhe);
    void AddH(G4double calPhe);
    void AddGd(G4double calPhe);
    void AddMu(G4double calPhe);

    // get methods
    G4double GetPheNum() const;
    G4double GetPheNumH() const;
    G4double GetPheNumGd() const;
    G4double GetPheNumMu() const;

private:
    G4double fCalPheNum;
    G4double fCalPheNumH;
    G4double fCalPheNumGd;
    G4double fCalPheNumMu;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using PANDASimPhotocathodeHitsCollection = G4THitsCollection<PANDASimPhotocathodeHit>;

extern G4ThreadLocal G4Allocator<PANDASimPhotocathodeHit>* PANDASimPhotocathodeHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* PANDASimPhotocathodeHit::operator new(size_t)
{
    if (!PANDASimPhotocathodeHitAllocator) {
        PANDASimPhotocathodeHitAllocator = new G4Allocator<PANDASimPhotocathodeHit>;
    }
    void* hit;
    hit = (void*)PANDASimPhotocathodeHitAllocator->MallocSingle();
    return hit;
}

inline void PANDASimPhotocathodeHit::operator delete(void* hit)
{
    if (!PANDASimPhotocathodeHitAllocator) {
        PANDASimPhotocathodeHitAllocator = new G4Allocator<PANDASimPhotocathodeHit>;
    }
    PANDASimPhotocathodeHitAllocator->FreeSingle((PANDASimPhotocathodeHit*)hit);
}

inline void PANDASimPhotocathodeHit::Add(G4double calPhe) {
    fCalPheNum += calPhe;
}

inline void PANDASimPhotocathodeHit::AddH(G4double calPhe) {
    fCalPheNumH += calPhe;
}

inline void PANDASimPhotocathodeHit::AddGd(G4double calPhe) {
    fCalPheNumGd += calPhe;
}

inline void PANDASimPhotocathodeHit::AddMu(G4double calPhe) {
    fCalPheNumMu += calPhe;
}

inline G4double PANDASimPhotocathodeHit::GetPheNum() const {
    return fCalPheNum;
}

inline G4double PANDASimPhotocathodeHit::GetPheNumH() const {
    return fCalPheNumH;
}

inline G4double PANDASimPhotocathodeHit::GetPheNumGd() const {
    return fCalPheNumGd;
}

inline G4double PANDASimPhotocathodeHit::GetPheNumMu() const {
    return fCalPheNumMu;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

