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

using namespace std;

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
    
    void PushBackEnergyDeposit(G4double edep);

    void PushBackCaptureTimeH(G4double capTime);
    void PushBackCaptureTimeGd(G4double capTime);
    void PushBackDecayTimeMu(G4double decayTime);

    void PushBackAllAbPh(vector<G4int> nAbPh);
    void PushBackAllDtPh(vector<G4int> nDtPh);

    void PushBackModuleEnergyDeposit(vector<vector<G4double> > edepVec);
    void PushBackModuleEnergyDepositDelayH(vector<vector<G4double> > energyDepositDelayH);
    void PushBackModuleEnergyDepositDelayGd(vector<vector<G4double> > energyDepositDelayGd);
    void PushBackModuleEnergyDepositDecayMu(vector<vector<G4double> > energyDepositDecayMu);

    void PushBackModuleCapTimeGd(vector<vector<G4double> > moduleCapTimeGd);
    void PushBackModuleCapTimeH(vector<vector<G4double> > moduleCapTimeH);
    void PushBackModuleDecayTimeMu(vector<vector<G4double> > moduleDecayTimeMu);

    void PushBackModuleAbPh(vector<vector<vector<G4int> > > nAbPhVec);
    void PushBackModuleDtPh(vector<vector<vector<G4int> > > nDtPhVec);

    void PushBackModuleCalPh(vector<vector<vector<G4double> > > nCalPhVec);
    void PushBackModuleCalPhDelayH(vector<vector<vector<G4double> > > nCalPhVec);
    void PushBackModuleCalPhDelayGd(vector<vector<vector<G4double> > > nCalPhVec);
    void PushBackModuleCalPhDecayMu(vector<vector<vector<G4double> > > nCalPhVec);

    // get methods
    inline list<G4double> GetEnergyDeposit() const { return energyDeposit; }

    inline list<G4double> GetCaptureTimeH() const { return capTimeH; }
    inline list<G4double> GetCaptureTimeGd() const { return capTimeGd; }
    inline list<G4double> GetDecayTimeMu() const { return decayTimeMu; }

    inline list<vector<G4int> > GetAllAbPh() const { return allAbPh; }
    inline list<vector<G4int> > GetAllDtPh() const { return allDtPh; }

    inline list<vector<vector<G4double> > > GetModuleEnergyDeposit() const { return moduleEnergyDeposit; }
    inline list<vector<vector<G4double> > > GetModuleEnergyDepositDelayH() const { return moduleEnergyDepositDelayH; }
    inline list<vector<vector<G4double> > > GetModuleEnergyDepositDelayGd() const { return moduleEnergyDepositDelayGd; }
    inline list<vector<vector<G4double> > > GetModuleEnergyDepositDecayMu() const { return moduleEnergyDepositDecayMu; }

    inline list<vector<vector<G4double> > > GetModuleCapTimeH() const { return moduleCapTimeH; }
    inline list<vector<vector<G4double> > > GetModuleCapTimeGd() const { return moduleCapTimeGd; }
    inline list<vector<vector<G4double> > > GetModuleDecayTimeMu() const { return moduleDecayTimeMu; }

    inline list<vector<vector<vector<G4int> > > > GetModuleAbPh() const { return moduleAbPh; }
    inline list<vector<vector<vector<G4int> > > > GetModuleDtPh() const { return moduleDtPh; }

    inline list<vector<vector<vector<G4double> > > > GetModuleCalPh() const { return moduleCalPh; }
    inline list<vector<vector<vector<G4double> > > > GetModuleCalPhDelayH() const { return moduleCalPhDelayH; }
    inline list<vector<vector<vector<G4double> > > > GetModuleCalPhDelayGd() const { return moduleCalPhDelayGd; }
    inline list<vector<vector<vector<G4double> > > > GetModuleCalPhDecayMu() const { return moduleCalPhDecayMu; }

  private:
    list<G4double> energyDeposit;

    list<G4double> capTimeH;
    list<G4double> capTimeGd;
    list<G4double> decayTimeMu;

    list<vector<G4int> > allAbPh;
    list<vector<G4int> > allDtPh;

    list<vector<vector<G4double> > > moduleEnergyDeposit;
    list<vector<vector<G4double> > > moduleEnergyDepositDelayH;
    list<vector<vector<G4double> > > moduleEnergyDepositDelayGd;
    list<vector<vector<G4double> > > moduleEnergyDepositDecayMu;

    list<vector<vector<G4double> > > moduleCapTimeH;
    list<vector<vector<G4double> > > moduleCapTimeGd;
    list<vector<vector<G4double> > > moduleDecayTimeMu;

    list<vector<vector<vector<G4int> > > > moduleAbPh;
    list<vector<vector<vector<G4int> > > > moduleDtPh;

    list<vector<vector<vector<G4double> > > > moduleCalPh;
    list<vector<vector<vector<G4double> > > > moduleCalPhDelayH;
    list<vector<vector<vector<G4double> > > > moduleCalPhDelayGd;
    list<vector<vector<vector<G4double> > > > moduleCalPhDecayMu;
};


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
inline void PANDASimRun::PushBackEnergyDeposit(G4double edep)
{
    energyDeposit.push_back(edep);
    //for (std::list<G4double>::iterator itr = energyDeposit.begin(); itr != energyDeposit.end(); ++itr)
    //{
    //	G4cout << *itr << G4endl;
    //}
}

inline void PANDASimRun::PushBackCaptureTimeH(G4double capTime)
{
    capTimeH.push_back(capTime);
}

inline void PANDASimRun::PushBackCaptureTimeGd(G4double capTime)
{
    capTimeGd.push_back(capTime);
}

inline void PANDASimRun::PushBackDecayTimeMu(G4double decayTime)
{
    decayTimeMu.push_back(decayTime);
}

inline void PANDASimRun::PushBackAllAbPh(vector<G4int> nAbPh)
{
    allAbPh.push_back(nAbPh);
}

inline void PANDASimRun::PushBackAllDtPh(vector<G4int> nDtPh)
{
    allDtPh.push_back(nDtPh);
}

inline void PANDASimRun::PushBackModuleEnergyDeposit(vector<vector<G4double> > edepVec)
{
    moduleEnergyDeposit.push_back(edepVec);
}

inline void PANDASimRun::PushBackModuleEnergyDepositDelayH(vector<vector<G4double> > edepVec)
{
    moduleEnergyDepositDelayH.push_back(edepVec);
}

inline void PANDASimRun::PushBackModuleEnergyDepositDelayGd(vector<vector<G4double> > edepVec)
{
    moduleEnergyDepositDelayGd.push_back(edepVec);
}

inline void PANDASimRun::PushBackModuleEnergyDepositDecayMu(vector<vector<G4double> > edepVec)
{
    moduleEnergyDepositDecayMu.push_back(edepVec);
}

inline void PANDASimRun::PushBackModuleCapTimeGd(vector<vector<G4double>> moduleCapTime)
{
    moduleCapTimeGd.push_back(moduleCapTime);
}

inline void PANDASimRun::PushBackModuleCapTimeH(vector<vector<G4double>> moduleCapTime)
{
    moduleCapTimeH.push_back(moduleCapTime);
}

inline void PANDASimRun::PushBackModuleDecayTimeMu(vector<vector<G4double>> moduleDecayTime)
{
    moduleDecayTimeMu.push_back(moduleDecayTime);
}

inline void PANDASimRun::PushBackModuleAbPh(vector<vector<vector<G4int> > > nAbPhVec)
{
    moduleAbPh.push_back(nAbPhVec);
}

inline void PANDASimRun::PushBackModuleDtPh(vector<vector<vector<G4int> > > nDtPhVec)
{
    moduleDtPh.push_back(nDtPhVec);
}

inline void PANDASimRun::PushBackModuleCalPh(vector<vector<vector<G4double> > > nCalPhVec)
{
    moduleCalPh.push_back(nCalPhVec);
}

inline void PANDASimRun::PushBackModuleCalPhDelayH(vector<vector<vector<G4double> > > nCalPhVec)
{
    moduleCalPhDelayH.push_back(nCalPhVec);
}

inline void PANDASimRun::PushBackModuleCalPhDelayGd(vector<vector<vector<G4double> > > nCalPhVec)
{
    moduleCalPhDelayGd.push_back(nCalPhVec);
}

inline void PANDASimRun::PushBackModuleCalPhDecayMu(vector<vector<vector<G4double> > > nCalPhVec)
{
    moduleCalPhDecayMu.push_back(nCalPhVec);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

