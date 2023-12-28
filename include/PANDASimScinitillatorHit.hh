//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file PANDASimScinitillatorHit.hh
/// \brief Definition of the PANDASimScinitillatorHit class

#ifndef PANDASimScinitillatorHit_h
#define PANDASimScinitillatorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Threading.hh"

class PANDASimScinitillatorHit : public G4VHit
{
public:
    PANDASimScinitillatorHit();
    PANDASimScinitillatorHit(const PANDASimScinitillatorHit&);
    virtual ~PANDASimScinitillatorHit();

    // operators
    const PANDASimScinitillatorHit& operator=(const PANDASimScinitillatorHit&);
    G4bool operator==(const PANDASimScinitillatorHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print() {}

    // methods to handle data
    //void AddEnergyDeposit(G4int i, G4int j, G4double edep);
    void Add(G4double de);
    void AddH(G4double de);
    void AddGd(G4double de);
    void AddMu(G4double de);
    void TimeH(G4double t);
    void TimeMu(G4double t);
    void AddMuTrack(G4double tl);
    void AddMuEdep(G4double de);

    void AddNLi9();
    void AddNHe8();

    // get methods
    G4double GetEdep() const;
    G4double GetEdepH() const;
    G4double GetEdepGd() const;
    G4double GetEdepMu() const;
    G4double GetCapTimeH() const;
    G4double GetDecayTimeMu() const;
    G4double GetMuTrack() const; 
    G4double GetMuEdep() const;

    G4int GetNLi9() const; 
    G4int GetNHe8() const; 

private:
    G4double fEdep;
    G4double fEdepH;
    G4double fEdepGd;
    G4double fEdepMu; // edep after muon decay
    G4double capTimeH;
    G4double decayTimeMu;
    G4double muTrack;
    G4double muEdep; // edep of muon itself

    G4int numLi9;
    G4int numHe8;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using PANDASimScinHitsCollection = G4THitsCollection<PANDASimScinitillatorHit>;

extern G4ThreadLocal G4Allocator<PANDASimScinitillatorHit>* PANDASimScinHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* PANDASimScinitillatorHit::operator new(size_t)
{
    if (!PANDASimScinHitAllocator) {
        PANDASimScinHitAllocator = new G4Allocator<PANDASimScinitillatorHit>;
    }
    void* hit;
    hit = (void*)PANDASimScinHitAllocator->MallocSingle();
    return hit;
}

inline void PANDASimScinitillatorHit::operator delete(void *hit)
{
    if (!PANDASimScinHitAllocator) {
        PANDASimScinHitAllocator = new G4Allocator<PANDASimScinitillatorHit>;
    }
    PANDASimScinHitAllocator->FreeSingle((PANDASimScinitillatorHit*)hit);
}

inline G4double PANDASimScinitillatorHit::GetEdep() const { 
  return fEdep;
}

inline G4double PANDASimScinitillatorHit::GetEdepH() const {
    return fEdepH;
}

inline G4double PANDASimScinitillatorHit::GetEdepGd() const {
    return fEdepGd;
}

inline G4double PANDASimScinitillatorHit::GetEdepMu() const {
    return fEdepMu;
}

inline G4double PANDASimScinitillatorHit::GetCapTimeH() const {
    return capTimeH;
}

inline G4double PANDASimScinitillatorHit::GetDecayTimeMu() const {
    return decayTimeMu;
}

inline G4double PANDASimScinitillatorHit::GetMuTrack() const
{
    return muTrack;
}

inline G4double PANDASimScinitillatorHit::GetMuEdep() const
{
    return muEdep;
}

inline void PANDASimScinitillatorHit::AddNLi9()
{
    numLi9++;
}

inline void PANDASimScinitillatorHit::AddNHe8()
{
    numHe8++;
}

inline void PANDASimScinitillatorHit::Add(G4double de) {
    fEdep += de;
}

inline void PANDASimScinitillatorHit::AddH(G4double de) {
    fEdepH += de;
}

inline void PANDASimScinitillatorHit::AddGd(G4double de) {
    fEdepGd += de;
}

inline void PANDASimScinitillatorHit::AddMu(G4double de) {
    fEdepMu += de;
}

inline void PANDASimScinitillatorHit::TimeH(G4double t) {
    capTimeH = t;
}

inline void PANDASimScinitillatorHit::TimeMu(G4double t) {
    decayTimeMu = t;
}

inline void PANDASimScinitillatorHit::AddMuTrack(G4double tl)
{
    muTrack += tl;
}

inline void PANDASimScinitillatorHit::AddMuEdep(G4double de)
{
    muEdep += de;
}

inline G4int PANDASimScinitillatorHit::GetNLi9() const
{
    return numLi9;
}

inline G4int PANDASimScinitillatorHit::GetNHe8() const
{
    return numHe8;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
