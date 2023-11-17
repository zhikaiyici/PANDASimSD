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
/// \file PANDASimScinitillatorSD.cc
/// \brief Implementation of the PANDASimScinitillatorSD class

#include "PANDASimScinitillatorSD.hh"

#include "globals.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include "G4OpticalPhoton.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimScinitillatorSD::PANDASimScinitillatorSD(
                            const G4String& name, 
                            const G4String& hitsCollectionName,
                            G4int nofHits)
 : G4VSensitiveDetector(name),
   fHitsCollection(nullptr), fEventAction(nullptr),
    fHitsNum(nofHits)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimScinitillatorSD::~PANDASimScinitillatorSD() 
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimScinitillatorSD::Initialize(G4HCofThisEvent* hce)
{
    // Create hits collection
    fHitsCollection
        = new PANDASimScinHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Add this collection in hce
    G4int hcID
        = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);

    // Create hits and fill  hits with zero energy deposition
    for (G4int i = 0; i < fHitsNum; i++)
    {
        fHitsCollection->insert(new PANDASimScinitillatorHit());
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PANDASimScinitillatorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4Track* theTrack = step->GetTrack();
    const G4ParticleDefinition* particleDefinition = theTrack->GetDynamicParticle()->GetParticleDefinition();
    const G4String& strPrtclName = particleDefinition->GetParticleName();

    //if (particleDefinition == G4OpticalPhoton::OpticalPhotonDefinition()) return false;
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
        G4Exception("PANDASimScinitillatorSD::ProcessHits()",
            "MyCode0004", FatalException, msg);
    }
    if (!fEventAction)
        fEventAction = static_cast<PANDASimEventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());

    auto stepPoint = step->GetPostStepPoint();
    //auto stepPoint = step->GetPreStepPoint();
    const G4String processName = stepPoint->GetProcessDefinedStep()->GetProcessName();
    //const G4String processName_ = preStepPoint->GetProcessDefinedStep()->GetProcessName();
    //G4cout << "processName: " << processName << G4endl;
    
    // 判断是否为中子
    if (strPrtclName == "neutron")
    {
        //G4cout << "GetCurrentStepNumber: " << theTrack->GetCurrentStepNumber() << G4endl;
        //G4cout << "particle name : " << strPrtclName << G4endl;
        //G4cout << "particle created process : " << theTrack->GetCreatorProcess()->GetProcessName() << G4endl;
        //G4cout << "track id: " << theTrack->GetTrackID() << G4endl;
        //G4cout << "parent id: " << theTrack->GetParentID() << G4endl;
        //G4cout << "scin here" << G4endl;
        //getchar();

        if (processName == "nCapture")
        {
            const G4double capTimeH = stepPoint->GetGlobalTime() / us;
            //const G4double capTimeH = postStepPoint->GetLocalTime() / us;
            hit->TimeH(capTimeH);
            fEventAction->SetDelayFlagH(true);
        }
    }
    else if (strPrtclName == "mu+" || strPrtclName == "mu-")
    {
        if (processName == "Decay")
        {
            const G4double muDecayTime = stepPoint->GetGlobalTime() / us;
            //const G4double muDecayTime = postStepPoint->GetLocalTime() / us;
            hit->TimeMu(muDecayTime);
            fEventAction->SetDecayFlagMu(true);
        }
    }

    // energy deposit
    G4double edep = step->GetTotalEnergyDeposit();

    if (edep == 0.) return false;

    // Add values
    G4bool delayFlagH = fEventAction->GetDelayFlagH();
    G4bool delayFlagGd = fEventAction->GetDelayFlagGd();
    G4bool decayFlagMu = fEventAction->GetDecayFlagMu();
    if (delayFlagH)
    {
        //auto ke = theTrack->GetKineticEnergy();
        //if (ke > 2.5 * MeV)
        //{
        //    G4cout << "ke: " << ke << G4endl;
        //    G4cout << "particle name : " << strPrtclName << G4endl;
        //    G4cout << "particle created process : " << theTrack->GetCreatorProcess()->GetProcessName() << G4endl;
        //    G4cout << "track id: " << theTrack->GetTrackID() << G4endl;
        //    G4cout << "parent id: " << theTrack->GetParentID() << G4endl;
        //    getchar();
        //}
        hit->AddH(edep);
    }
    else if (delayFlagGd)
        hit->AddGd(edep);
    else if (decayFlagMu)
        hit->AddMu(edep);
    else
        hit->Add(edep);

    return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimScinitillatorSD::EndOfEvent(G4HCofThisEvent*)
{
    //if (verboseLevel > 1) {
    //    auto nofHits = fHitsCollection->entries();
    //    G4cout
    //        << G4endl
    //        << "-------->Hits Collection: in this event they are " << nofHits
    //        << " hits in the tracker chambers: " << G4endl;
    //    for (std::size_t i = 0; i < nofHits; ++i) (*fHitsCollection)[i]->Print();
    //}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
