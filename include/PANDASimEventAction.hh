﻿//
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
// $Id: PANDASimEventAction.hh 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file PANDASimEventAction.hh
/// \brief Definition of the PANDASimEventAction class

#ifndef PANDASimEventAction_h
#define PANDASimEventAction_h 1

#include "PANDASimScinitillatorHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <vector>
#include <list>

//using namespace std;

class PANDASimRunAction;

/// Event action class
///

class PANDASimEventAction : public G4UserEventAction
{
public:
	PANDASimEventAction(/*PANDASimRunAction* runAction*/);
	virtual ~PANDASimEventAction();

	virtual void BeginOfEventAction(const G4Event* event);
	virtual void EndOfEventAction(const G4Event* event);

	inline void AddEdep(G4double edep) { fEdep += edep; }

	void AddEnergyDeposit(G4int i, G4int j, G4double edep);
	void AddEnergyDepositDelayH(G4int i, G4int j, G4double edep);
	void AddEnergyDepositDelayGd(G4int i, G4int j, G4double edep);
	void AddEnergyDepositDecayMu(G4int i, G4int j, G4double edep);

	void CapTimeH(G4int i, G4int j, G4double t);
	void CapTimeGd(G4int i, G4int j, G4double t);
	void DecayTimeMu(G4int i, G4int j, G4double t);

	//void AddModuleAbPh(G4int i, G4int j, G4int k);
	//void AddModuleDtPh(G4int i, G4int j, G4int k);
	
	void AddModuleCalPh(G4int i, G4int j, G4int k, G4double nPh);
	void AddModuleCalPhDelayH(G4int i, G4int j, G4int k, G4double nPh);
	void AddModuleCalPhDelayGd(G4int i, G4int j, G4int k, G4double nPh);
	void AddModuleCalPhDecayMu(G4int i, G4int j, G4int k, G4double nPh);

	/*void AddAllAbPh(G4int i);
	void AddAllDtPh(G4int i);*/

	inline G4bool GetDelayedFlag() const { return delayedFlag; }
	inline G4bool GetDelayFlagH() const { return delayFlagH; }
	inline G4bool GetDelayFlagGd() const { return delayFlagGd; }
	inline G4bool GetDecayFlagMu() const { return decayFlagMu; }

	inline void SetDelayedFlag(G4bool flag) { delayedFlag = flag; }
	inline void SetDelayFlagH(G4bool flag) { delayFlagH = flag; }
	inline void SetDelayFlagGd(G4bool flag) { delayFlagGd = flag; }
	inline void SetDecayFlagMu(G4bool flag) { decayFlagMu = flag; }

private:
	//PANDASimRunAction* fRunAction;
	G4double fEdep;
	G4int arraySize;
	G4int fScinHCID;
	G4int fGdHCID;
	G4int fPhocathHCID;

	G4bool delayedFlag;
	G4bool delayFlagH;
	G4bool delayFlagGd;
	G4bool decayFlagMu;

	//vector<G4int> nAbsorbedOpPhoton;
	//vector<G4int> nDetectedOpPhoton;
	//vector<vector<vector<G4int> > > nAbPhVec;
	//vector<vector<vector<G4int> > > nDtPhVec;
	std::vector<std::vector<std::vector<G4double> > > nCalPhVec;
	std::vector<std::vector<std::vector<G4double> > > nCalPhDelayHVec;
	std::vector<std::vector<std::vector<G4double> > > nCalPhDelayGdVec;
	std::vector<std::vector<std::vector<G4double> > > nCalPhDecayMuVec;
	std::vector<std::vector<G4double> > energyDeposit;
	std::vector<std::vector<G4double> > energyDepositDelayH;
	std::vector<std::vector<G4double> > energyDepositDelayGd;
	std::vector<std::vector<G4double> > energyDepositDecayMu;
	std::vector<std::vector<G4double> > capTimeH;
	std::vector<std::vector<G4double> > capTimeGd;
	std::vector<std::vector<G4double> > decayTimeMu;
	std::vector<std::vector<G4double> > muTrackLength;
	std::vector<std::vector<G4double> > muEdep;

	G4VHitsCollection* GetHitsCollection(G4int hcID, const G4Event* event) const;

	void InitVector(std::vector<std::vector<G4double> >& energyDeposit, G4int arrayNumber);
	void InitVector(std::vector<std::vector<std::vector<G4int> > >& nPhVec, G4int arrayNumber);
	void InitVector(std::vector<std::vector<std::vector<G4double> > >& nPhVec, G4int arrayNumber);

	void InitializeVector(std::vector<std::vector<G4double> >& energyDeposit);
	void InitializeVector(std::vector<std::vector<std::vector<G4int> > >& nPhVec);
	void InitializeVector(std::vector<std::vector<std::vector<G4double> > >& nPhVec);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void PANDASimEventAction::AddEnergyDeposit(G4int i, G4int j, G4double edep)
{
	energyDeposit[i][j] += edep;
}

inline void PANDASimEventAction::AddEnergyDepositDelayH(G4int i, G4int j, G4double edep)
{
	energyDepositDelayH[i][j] += edep;
}

inline void PANDASimEventAction::AddEnergyDepositDelayGd(G4int i, G4int j, G4double edep)
{
	energyDepositDelayGd[i][j] += edep;
}

inline void PANDASimEventAction::AddEnergyDepositDecayMu(G4int i, G4int j, G4double edep)
{
	energyDepositDecayMu[i][j] += edep;
}

inline void PANDASimEventAction::CapTimeH(G4int i, G4int j, G4double t)
{
	capTimeH[i][j] = t;
}

inline void PANDASimEventAction::CapTimeGd(G4int i, G4int j, G4double t)
{
	capTimeGd[i][j] = t;
}

inline void PANDASimEventAction::DecayTimeMu(G4int i, G4int j, G4double t)
{
	decayTimeMu[i][j] = t;
}

/*inline void PANDASimEventAction::AddModuleAbPh(G4int i, G4int j, G4int k)
{
	nAbPhVec[i][j][k] += 1;
}

inline void PANDASimEventAction::AddModuleDtPh(G4int i, G4int j, G4int k)
{
	nDtPhVec[i][j][k] += 1;
}*/

inline void PANDASimEventAction::AddModuleCalPh(G4int i, G4int j, G4int k, G4double nPh)
{
	nCalPhVec[i][j][k] += nPh;
}

inline void PANDASimEventAction::AddModuleCalPhDelayH(G4int i, G4int j, G4int k, G4double nPh)
{
	nCalPhDelayHVec[i][j][k] += nPh;
}

inline void PANDASimEventAction::AddModuleCalPhDelayGd(G4int i, G4int j, G4int k, G4double nPh)
{
	nCalPhDelayGdVec[i][j][k] += nPh;
}

inline void PANDASimEventAction::AddModuleCalPhDecayMu(G4int i, G4int j, G4int k, G4double nPh)
{
	nCalPhDecayMuVec[i][j][k] += nPh;
}

/*inline void PANDASimEventAction::AddAllAbPh(G4int i)
{
	nAbsorbedOpPhoton[i] += 1;
}

inline void PANDASimEventAction::AddAllDtPh(G4int i)
{
	nDetectedOpPhoton[i] += 1;
}*/

#endif
