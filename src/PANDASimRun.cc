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
// $Id: B1Run.cc 66536 2012-12-19 14:32:36Z ihrivnac $
//
/// \file B1Run.cc
/// \brief Implementation of the B1Run class

#include "PANDASimRun.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimRun::PANDASimRun()
	: G4Run(),
	energyDeposit(0),
	capTimeH(0), capTimeGd(0), decayTimeMu(0),
	allAbPh(0), allDtPh(0),
	moduleEnergyDeposit(0), moduleEnergyDepositDelayH(0), moduleEnergyDepositDelayGd(0), moduleEnergyDepositDecayMu(0),
	moduleCapTimeH(0), moduleCapTimeGd(0), moduleDecayTimeMu(0),
	moduleMuTrackLength(0), moduleMuEdep(0),
	moduleAbPh(0), moduleDtPh(0),
	moduleCalPh(0), moduleCalPhDelayH(0), moduleCalPhDelayGd(0), moduleCalPhDecayMu(0)
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimRun::~PANDASimRun()
{} 
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimRun::Merge(const G4Run* run)
{
	// G4cout << "---------------------------Merge-----------------------" << G4endl;
	// getchar();

	const PANDASimRun* localRun = static_cast<const PANDASimRun*>(run);

	std::list<G4double> localEnergyDeposit = localRun->energyDeposit;
	energyDeposit.merge(localEnergyDeposit);

	std::list<G4double> localCaptureTimeH = localRun->capTimeH;
	capTimeH.merge(localCaptureTimeH);

	std::list<G4double> localCaptureTimeGd = localRun->capTimeGd;
	capTimeGd.merge(localCaptureTimeGd);

	std::list<G4double> localDecayTimeMu = localRun->decayTimeMu;
	decayTimeMu.merge(localDecayTimeMu);

	std::list<std::vector<G4int>> localAbsorbedPhoton = localRun->allAbPh;
	allAbPh.merge(localAbsorbedPhoton);

	std::list<std::vector<G4int>> localDetectedPhoton = localRun->allDtPh;
	allDtPh.merge(localDetectedPhoton);

	std::list<std::vector<std::vector<G4double>>> localModuleEnergyDeposit = localRun->moduleEnergyDeposit;
	moduleEnergyDeposit.merge(localModuleEnergyDeposit);

	std::list<std::vector<std::vector<G4double>>> localModuleEnergyDepositDelayH = localRun->moduleEnergyDepositDelayH;
	moduleEnergyDepositDelayH.merge(localModuleEnergyDepositDelayH);

	std::list<std::vector<std::vector<G4double>>> localModuleEnergyDepositDelayGd = localRun->moduleEnergyDepositDelayGd;
	moduleEnergyDepositDelayGd.merge(localModuleEnergyDepositDelayGd);

	std::list<std::vector<std::vector<G4double>>> localModuleEnergyDepositDecayMu = localRun->moduleEnergyDepositDecayMu;
	moduleEnergyDepositDecayMu.merge(localModuleEnergyDepositDecayMu);

	std::list<std::vector<std::vector<G4double>>> localModuleCapTimeGd = localRun->moduleCapTimeGd;
	moduleCapTimeGd.merge(localModuleCapTimeGd);

	std::list<std::vector<std::vector<G4double>>> localModuleCapTimeH = localRun->moduleCapTimeH;
	moduleCapTimeH.merge(localModuleCapTimeH);

	std::list<std::vector<std::vector<G4double>>> localModuleDecayTimeMu = localRun->moduleDecayTimeMu;
	moduleDecayTimeMu.merge(localModuleDecayTimeMu);

	std::list<std::vector<std::vector<G4double>>> localModuleMuTrackLength = localRun->moduleMuTrackLength;
	moduleMuTrackLength.merge(localModuleMuTrackLength);

	std::list<std::vector<std::vector<G4double>>> localModuleMuEdep = localRun->moduleMuEdep;
	moduleMuEdep.merge(localModuleMuEdep);

	std::list<std::vector<std::vector<std::vector<G4int>>>> localModuleAbPh = localRun->moduleAbPh;
	moduleAbPh.merge(localModuleAbPh);

	std::list<std::vector<std::vector<std::vector<G4int>>>> localModuleDtPh = localRun->moduleDtPh;
	moduleDtPh.merge(localModuleDtPh);

	std::list<std::vector<std::vector<std::vector<G4double>>>> localModuleCalPh = localRun->moduleCalPh;
	moduleCalPh.merge(localModuleCalPh);

	std::list<std::vector<std::vector<std::vector<G4double>>>> localModuleCalPhDelayH = localRun->moduleCalPhDelayH;
	moduleCalPhDelayH.merge(localModuleCalPhDelayH);

	std::list<std::vector<std::vector<std::vector<G4double>>>> localModuleCalPhDelayGd = localRun->moduleCalPhDelayGd;
	moduleCalPhDelayGd.merge(localModuleCalPhDelayGd);

	std::list<std::vector<std::vector<std::vector<G4double>>>> localModuleCalPhDecayMu = localRun->moduleCalPhDecayMu;
	moduleCalPhDelayGd.merge(localModuleCalPhDecayMu);

	G4Run::Merge(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


