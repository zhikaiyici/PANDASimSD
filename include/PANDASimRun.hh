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
// $Id: B1Run.hh 66536 2012-12-19 14:32:36Z ihrivnac $
//
/// \file B1Run.hh
/// \brief Definition of the B1Run class

#ifndef PANDASimRun_h
#define PANDASimRun_h 1

#include "G4Run.hh"
#include "globals.hh"

#include <list>

//using namespace std;

class G4Event;

/// Run class
///

class PANDASimRun : public G4Run
{
  public:
    PANDASimRun();
    virtual ~PANDASimRun();

    // method from the base class
    virtual void Merge(const G4Run*);
    
    void PushBackEnergyDeposit(const G4double& edep);

    void PushBackCaptureTimeH(const G4double& capTime);
    void PushBackCaptureTimeGd(const G4double& capTime);
    void PushBackDecayTimeMu(const G4double& decayTime);

    void PushBackAllAbPh(const std::vector<G4int>& nAbPh);
    void PushBackAllDtPh(const std::vector<G4int>& nDtPh);

    void PushBackModuleEnergyDeposit(const std::vector<std::vector<G4double>>& edepVec);
    void PushBackModuleEnergyDepositDelayH(const std::vector<std::vector<G4double>>& energyDepositDelayH);
    void PushBackModuleEnergyDepositDelayGd(const std::vector<std::vector<G4double>>& energyDepositDelayGd);
    void PushBackModuleEnergyDepositDecayMu(const std::vector<std::vector<G4double>>& energyDepositDecayMu);

    void PushBackModuleCapTimeGd(const std::vector<std::vector<G4double>>& moduleCapTimeGd);
    void PushBackModuleCapTimeH(const std::vector<std::vector<G4double>>& moduleCapTimeH);
    void PushBackModuleDecayTimeMu(const std::vector<std::vector<G4double>>& moduleDecayTimeMu);

    void PushBackModuleMuTrackLength(const std::vector<std::vector<G4double>>& moduleMuTrackLength);
    void PushBackModuleMuEdep(const std::vector<std::vector<G4double>>& moduleMuEdep);

    void PushBackModuleAbPh(const std::vector<std::vector<std::vector<G4int>>>& nAbPhVec);
    void PushBackModuleDtPh(const std::vector<std::vector<std::vector<G4int>>>& nDtPhVec);

    void PushBackModuleCalPh(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec);
    void PushBackModuleCalPhDelayH(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec);
    void PushBackModuleCalPhDelayGd(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec);
    void PushBackModuleCalPhDecayMu(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec);

    // get methods
    inline std::list<G4double> GetEnergyDeposit() const { return energyDeposit; }

    inline std::list<G4double> GetCaptureTimeH() const { return capTimeH; }
    inline std::list<G4double> GetCaptureTimeGd() const { return capTimeGd; }
    inline std::list<G4double> GetDecayTimeMu() const { return decayTimeMu; }

    inline std::list<std::vector<G4int>> GetAllAbPh() const { return allAbPh; }
    inline std::list<std::vector<G4int>> GetAllDtPh() const { return allDtPh; }

    inline std::list<std::vector<std::vector<G4double>>> GetModuleEnergyDeposit() const { return moduleEnergyDeposit; }
    inline std::list<std::vector<std::vector<G4double>>> GetModuleEnergyDepositDelayH() const { return moduleEnergyDepositDelayH; }
    inline std::list<std::vector<std::vector<G4double>>> GetModuleEnergyDepositDelayGd() const { return moduleEnergyDepositDelayGd; }
    inline std::list<std::vector<std::vector<G4double>>> GetModuleEnergyDepositDecayMu() const { return moduleEnergyDepositDecayMu; }

    inline std::list<std::vector<std::vector<G4double>>> GetModuleCapTimeH() const { return moduleCapTimeH; }
    inline std::list<std::vector<std::vector<G4double>>> GetModuleCapTimeGd() const { return moduleCapTimeGd; }
    inline std::list<std::vector<std::vector<G4double>>> GetModuleDecayTimeMu() const { return moduleDecayTimeMu; }

    inline std::list<std::vector<std::vector<G4double>>> GetModuleMuTrackLength() const { return moduleMuTrackLength; }
    inline std::list<std::vector<std::vector<G4double>>> GetModuleMuEdep() const { return moduleMuEdep; }

    inline std::list<std::vector<std::vector<std::vector<G4int>>>> GetModuleAbPh() const { return moduleAbPh; }
    inline std::list<std::vector<std::vector<std::vector<G4int>>>> GetModuleDtPh() const { return moduleDtPh; }

    inline std::list<std::vector<std::vector<std::vector<G4double>>>> GetModuleCalPh() const { return moduleCalPh; }
    inline std::list<std::vector<std::vector<std::vector<G4double>>>> GetModuleCalPhDelayH() const { return moduleCalPhDelayH; }
    inline std::list<std::vector<std::vector<std::vector<G4double>>>> GetModuleCalPhDelayGd() const { return moduleCalPhDelayGd; }
    inline std::list<std::vector<std::vector<std::vector<G4double>>>> GetModuleCalPhDecayMu() const { return moduleCalPhDecayMu; }

  private:
    std::list<G4double> energyDeposit;

    std::list<G4double> capTimeH;
    std::list<G4double> capTimeGd;
    std::list<G4double> decayTimeMu;

    std::list<std::vector<G4int>> allAbPh;
    std::list<std::vector<G4int>> allDtPh;

    std::list<std::vector<std::vector<G4double>>> moduleEnergyDeposit;
    std::list<std::vector<std::vector<G4double>>> moduleEnergyDepositDelayH;
    std::list<std::vector<std::vector<G4double>>> moduleEnergyDepositDelayGd;
    std::list<std::vector<std::vector<G4double>>> moduleEnergyDepositDecayMu;

    std::list<std::vector<std::vector<G4double>>> moduleCapTimeH;
    std::list<std::vector<std::vector<G4double>>> moduleCapTimeGd;
    std::list<std::vector<std::vector<G4double>>> moduleDecayTimeMu;

    std::list<std::vector<std::vector<G4double>>> moduleMuTrackLength;
    std::list<std::vector<std::vector<G4double>>> moduleMuEdep;

    std::list<std::vector<std::vector<std::vector<G4int>>>> moduleAbPh;
    std::list<std::vector<std::vector<std::vector<G4int>>>> moduleDtPh;

    std::list<std::vector<std::vector<std::vector<G4double>>>> moduleCalPh;
    std::list<std::vector<std::vector<std::vector<G4double>>>> moduleCalPhDelayH;
    std::list<std::vector<std::vector<std::vector<G4double>>>> moduleCalPhDelayGd;
    std::list<std::vector<std::vector<std::vector<G4double>>>> moduleCalPhDecayMu;
};


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
inline void PANDASimRun::PushBackEnergyDeposit(const G4double& edep)
{
    energyDeposit.push_back(edep);
    //for (std::list<G4double>::iterator itr = energyDeposit.begin(); itr != energyDeposit.end(); ++itr)
    //{
    //	G4cout << *itr << G4endl;
    //}
}

inline void PANDASimRun::PushBackCaptureTimeH(const G4double& capTime)
{
    capTimeH.push_back(capTime);
}

inline void PANDASimRun::PushBackCaptureTimeGd(const G4double& capTime)
{
    capTimeGd.push_back(capTime);
}

inline void PANDASimRun::PushBackDecayTimeMu(const G4double& decayTime)
{
    decayTimeMu.push_back(decayTime);
}

inline void PANDASimRun::PushBackAllAbPh(const std::vector<G4int>& nAbPh)
{
    allAbPh.push_back(nAbPh);
}

inline void PANDASimRun::PushBackAllDtPh(const std::vector<G4int>& nDtPh)
{
    allDtPh.push_back(nDtPh);
}

inline void PANDASimRun::PushBackModuleEnergyDeposit(const std::vector<std::vector<G4double>>& edepVec)
{
    moduleEnergyDeposit.push_back(edepVec);
}

inline void PANDASimRun::PushBackModuleEnergyDepositDelayH(const std::vector<std::vector<G4double>>& edepVec)
{
    moduleEnergyDepositDelayH.push_back(edepVec);
}

inline void PANDASimRun::PushBackModuleEnergyDepositDelayGd(const std::vector<std::vector<G4double>>& edepVec)
{
    moduleEnergyDepositDelayGd.push_back(edepVec);
}

inline void PANDASimRun::PushBackModuleEnergyDepositDecayMu(const std::vector<std::vector<G4double>>& edepVec)
{
    moduleEnergyDepositDecayMu.push_back(edepVec);
}

inline void PANDASimRun::PushBackModuleCapTimeGd(const std::vector<std::vector<G4double>>& moduleCapTime)
{
    moduleCapTimeGd.push_back(moduleCapTime);
}

inline void PANDASimRun::PushBackModuleCapTimeH(const std::vector<std::vector<G4double>>& moduleCapTime)
{
    moduleCapTimeH.push_back(moduleCapTime);
}

inline void PANDASimRun::PushBackModuleDecayTimeMu(const std::vector<std::vector<G4double>>& moduleDecayTime)
{
    moduleDecayTimeMu.push_back(moduleDecayTime);
}

inline void PANDASimRun::PushBackModuleMuTrackLength(const std::vector<std::vector<G4double>>& moduleMuTrack)
{
    moduleMuTrackLength.push_back(moduleMuTrack);
}

inline void PANDASimRun::PushBackModuleMuEdep(const std::vector<std::vector<G4double>>& moduleMuE)
{
    moduleMuEdep.push_back(moduleMuE);
}

inline void PANDASimRun::PushBackModuleAbPh(const std::vector<std::vector<std::vector<G4int>>>& nAbPhVec)
{
    moduleAbPh.push_back(nAbPhVec);
}

inline void PANDASimRun::PushBackModuleDtPh(const std::vector<std::vector<std::vector<G4int>>>& nDtPhVec)
{
    moduleDtPh.push_back(nDtPhVec);
}

inline void PANDASimRun::PushBackModuleCalPh(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec)
{
    moduleCalPh.push_back(nCalPhVec);
}

inline void PANDASimRun::PushBackModuleCalPhDelayH(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec)
{
    moduleCalPhDelayH.push_back(nCalPhVec);
}

inline void PANDASimRun::PushBackModuleCalPhDelayGd(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec)
{
    moduleCalPhDelayGd.push_back(nCalPhVec);
}

inline void PANDASimRun::PushBackModuleCalPhDecayMu(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec)
{
    moduleCalPhDecayMu.push_back(nCalPhVec);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

