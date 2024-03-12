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
#include "PANDASimRunAction.hh"
#include "PANDASimDetectorConstruction.hh"
#include "UserDataInput.hh"

#include "globals.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4StackManager.hh"

#include "G4OpticalPhoton.hh"

#include "G4Trajectory.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimScinitillatorSD::PANDASimScinitillatorSD(
                            const G4String& name, 
                            const G4String& hitsCollectionName,
                            G4int nofHits)
 : G4VSensitiveDetector(name), fHitsCollection(nullptr), fHitsNum(nofHits),
   fRunAction(nullptr), fEventAction(nullptr), /*fStackManager(nullptr),*/ fTrackingAction(nullptr)
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

G4bool PANDASimScinitillatorSD::ProcessHits(G4Step* step, G4TouchableHistory* history)
{
    G4Track* theTrack = step->GetTrack();
    const G4ParticleDefinition* particleDefinition = theTrack->GetDynamicParticle()->GetParticleDefinition();
    G4String strPrtclName = particleDefinition->GetParticleName();

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

    if (!fTrackingAction)
        fTrackingAction = static_cast<PANDASimTrackingAction*>(G4EventManager::GetEventManager()->GetTrackingManager()->GetUserTrackingAction());

    if (!fRunAction)
    {
        //if (!fTrackingAction)
        //    fTrackingAction = static_cast<PANDASimTrackingAction*>(G4EventManager::GetEventManager()->GetTrackingManager()->GetUserTrackingAction());
        //fRunAction = fTrackingAction->GetPANDASimRunAction();
        fRunAction = const_cast<PANDASimRunAction*>(static_cast<const PANDASimRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));
    }

    //if (!fStackManager)
    //    fStackManager = G4EventManager::GetEventManager()->GetStackManager();

    auto postStepPoint = step->GetPostStepPoint();
    //auto stepPoint = step->GetPreStepPoint();
    auto processDefinedStep = postStepPoint->GetProcessDefinedStep();
    G4String processName = "";
    if (processDefinedStep)
        processName = processDefinedStep->GetProcessName();
    //const G4String processName_ = preStepPoint->GetProcessDefinedStep()->GetProcessName();
    //G4cout << "processName: " << processName << G4endl;
    
    G4int parentID = theTrack->GetParentID();

    //auto currentEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();
    //G4TrajectoryContainer* trajectoryContainer = currentEvent->GetTrajectoryContainer();
    //if (trajectoryContainer)
    //{
    //    // 遍历轨迹容器，查找与特定母粒子track ID 相关的轨迹
    //    for (size_t i = 0; i < trajectoryContainer->size(); i++) {
    //        G4VTrajectory* trajectory = (*trajectoryContainer)[i];
    //        if (trajectory->GetTrackID() == parentID)
    //        {
    //            //auto parentTime = trajectory->GetPoint(0)->get
    //            G4String particleNameP = trajectory->GetParticleName();
    //            G4cout << "Mother particle with ID " << parentID << " which is a " << particleNameP << G4endl;
    //        }
    //    }
    //}

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

        G4bool flagNeutron = fTrackingAction->GetFlagNeutron();
        if (parentID != 0 && !flagNeutron)
        {
            hit->AddNNeutron();
            const G4double genicTime = step->GetPreStepPoint()->GetGlobalTime() / us;
            hit->SetNeutronGenicTime(genicTime);
            fRunAction->PushNeutronGenicTime(genicTime);

            auto energy = theTrack->GetKineticEnergy() / keV;
            hit->SetNeutronKE(energy);
            fRunAction->PushNeutronKE(energy);

            fTrackingAction->SetFlagNeutron(true);

            //G4cout << "Neutron genic time (SD::Hits): " << genicTime << G4endl;
            //G4cout << "Neutron kinetic energy (SD::Hits): " << energy << G4endl;
        }

        if (processName == "nCapture")
        {
            //const G4double capTimeH1 = stepPoint->GetGlobalTime() / us;
            const G4double capTimeH = postStepPoint->GetLocalTime() / us;
            hit->SetTimeH(capTimeH);
            fEventAction->SetDelayFlagH(true);
            fRunAction->PushCapTimeH(capTimeH);
            //G4int nWaiting = fStackManager->GetNWaitingTrack();
            //G4int nUrgent = fStackManager->GetNUrgentTrack();
            //G4cout << "global capTimeH1: " << capTimeH1 << G4endl;
            //G4cout << "local capTimeH: " << capTimeH1 << G4endl;
        }
    }
    else if (strPrtclName == "mu+" || strPrtclName == "mu-")
    {
        if (processName == "Decay")
        {
            //const G4double muDecayTime = stepPoint->GetGlobalTime() / us;
            const G4double muDecayTime = postStepPoint->GetLocalTime() / us;
            hit->SetTimeMu(muDecayTime);
            fEventAction->SetDecayFlagMu(true);
        }
        G4double trackLength = step->GetStepLength() / mm;
        //G4cout << "trackLength:" << trackLength / mm << G4endl;
        hit->AddMuTrack(trackLength);
        G4double muEdep = step->GetTotalEnergyDeposit();
        hit->AddMuEdep(muEdep);
    }

    G4bool fLi9 = fTrackingAction->GetFlagLi9();
    G4bool fHe8 = fTrackingAction->GetFlagHe8();
    if (strPrtclName == "He8" && !fHe8)
    {
        if (parentID != 0)
        {
            hit->AddNHe8();
            fTrackingAction->SetFlagHe8(true);
            //G4cout << "muon He8" << G4endl;
            //getchar();
        }
    }
    else if (strPrtclName == "Li9" && !fLi9)
    {
        if (parentID != 0)
        {
            hit->AddNLi9();
            fTrackingAction->SetFlagLi9(true);
            //G4cout << "muon Li9" << G4endl;
            //getchar();
        }
    }

    // energy deposit
    G4double edep = step->GetTotalEnergyDeposit();

    if (edep == 0.) return false;

    // Add values
    G4bool delayedFlag = fEventAction->GetDelayedFlag();
    G4bool delayFlagH = fEventAction->GetDelayFlagH();
    G4bool delayFlagGd = fEventAction->GetDelayFlagGd();
    G4bool decayFlagMu = fEventAction->GetDecayFlagMu();
    //G4int nWaiting = fStackManager->GetNWaitingTrack();
    //G4int nUrgent = fStackManager->GetNUrgentTrack();
    //G4cout << "nWaiting: " << nWaiting << G4endl;
    //G4cout << "nUrgent: " << nUrgent << G4endl;
    if (delayedFlag)
    {
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
            hit->AddEdepDecay(edep);
    }
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

    if (!fRunAction)
    {
        //if (!fTrackingAction)
        //    fTrackingAction = static_cast<PANDASimTrackingAction*>(G4EventManager::GetEventManager()->GetTrackingManager()->GetUserTrackingAction());
        //fRunAction = fTrackingAction->GetPANDASimRunAction();
        fRunAction = const_cast<PANDASimRunAction*>(static_cast<const PANDASimRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));
    }

    //G4ThreadLocalStatic auto fRunAction = static_cast<const PANDASimRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
    
    G4int arraySize = 
        (static_cast<const PANDASimDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction()))->GetArrySize();// UserDataInput::GetSizeOfArray();
    std::vector<std::vector<G4int> > numHe8(arraySize, std::vector<G4int>(arraySize, 0));
    std::vector<std::vector<G4int> > numLi9(arraySize, std::vector<G4int>(arraySize, 0));
    std::vector<std::vector<G4int> > numNeutron(arraySize, std::vector<G4int>(arraySize, 0));

    std::vector<std::vector<G4double> > nGenicT(arraySize, std::vector<G4double>(arraySize, 0.));
    std::vector<std::vector<G4double> > nKineticE(arraySize, std::vector<G4double>(arraySize, 0.));

    std::vector<std::vector<G4double> > edepDecay(arraySize, std::vector<G4double>(arraySize, 0.));

    auto nofHits = fHitsCollection->entries();
    for (G4int i = 0; i < nofHits; i++)
    {
        G4int moduleRowReplicaNumber = i % arraySize;
        G4int moduleRepliaNumber = i / arraySize;
        PANDASimScinitillatorHit* hit = (*fHitsCollection)[i];

        G4int nHe8 = hit->GetNHe8();
        numHe8[moduleRepliaNumber][moduleRowReplicaNumber] = nHe8;

        G4int nLi9 = hit->GetNLi9();
        numLi9[moduleRepliaNumber][moduleRowReplicaNumber] = nLi9;

        G4int nNeutron = hit->GetNNeutron();
        numNeutron[moduleRepliaNumber][moduleRowReplicaNumber] = nNeutron;

        G4double nGT = hit->GetNeutronGenicTime();
        nGenicT[moduleRepliaNumber][moduleRowReplicaNumber] = nGT;

        G4double nKE = hit->GetNeutronKE();
        nKineticE[moduleRepliaNumber][moduleRowReplicaNumber] = nKE;

        G4double eDecay = hit->GetEdepDecay();
        edepDecay[moduleRepliaNumber][moduleRowReplicaNumber] = eDecay;
    }

    std::vector <std::vector<G4int> > empty2DVec(arraySize, std::vector<G4int>(arraySize, 0));
    if (numHe8 != empty2DVec)
        fRunAction->AddNHe8(numHe8);
    if (numLi9 != empty2DVec)
        fRunAction->AddNLi9(numLi9);
    if (numNeutron != empty2DVec)
        fRunAction->AddNNeutron(numNeutron);

    std::vector <std::vector<G4double> > empty2DVecDoub(arraySize, std::vector<G4double>(arraySize, 0));
    if (nGenicT != empty2DVecDoub)
        fRunAction->PushNeutronGenicTime(nGenicT);
    if (nKineticE != empty2DVecDoub)
        fRunAction->PushNeutronKE(nKineticE);
    if (edepDecay != empty2DVecDoub)
        fRunAction->PushEdepDecay(edepDecay);

    //G4cout << "-----------------------EndOfEventFromScintillatorSD------------------------------" << G4endl << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
