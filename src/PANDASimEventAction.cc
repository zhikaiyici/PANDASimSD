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
// $Id: PANDASimEventAction.cc 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file PANDASimEventAction.cc
/// \brief Implementation of the PANDASimEventAction class

#include "PANDASimAnalysis.hh"
#include "PANDASimRun.hh"
#include "PANDASimEventAction.hh"
#include "PANDASimRunAction.hh"

#include "PANDASimScinitillatorSD.hh"
#include "PANDASimScinitillatorHit.hh"
#include "PANDASimGdFilmHit.hh"
#include "PANDASimGdFilmSD.hh"
#include "PANDASimPhotocathodeHit.hh"
#include "PANDASimPhotocathodeSD.hh"

#include "UserDataInput.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimEventAction::PANDASimEventAction(/*PANDASimRunAction* runAction*/ )
	: G4UserEventAction(),
	//fRunAction(runAction),
	fEdep(0.),
	fScinHCID(-1), fGdHCID(-1), fPhocathHCID(-1),
	//nAbsorbedOpPhoton(2), nDetectedOpPhoton(2),
	delayFlagH(false), delayFlagGd(false), decayFlagMu(false)
{
	arraySize = UserDataInput::GetSizeOfArray();
	ResizeVector(energyDeposit, arraySize);
	ResizeVector(energyDepositDelayH, arraySize);
	ResizeVector(energyDepositDelayGd, arraySize);
	ResizeVector(energyDepositDecayMu, arraySize);

	ResizeVector(capTimeH, arraySize);
	ResizeVector(capTimeGd, arraySize);
	ResizeVector(decayTimeMu, arraySize);

	ResizeVector(muTrackLength, arraySize);

	//ResizeVector(nAbPhVec, arraySize);
	//ResizeVector(nDtPhVec, arraySize);

	ResizeVector(nCalPhVec, arraySize);
	ResizeVector(nCalPhDelayHVec, arraySize);
	ResizeVector(nCalPhDelayGdVec, arraySize);
	ResizeVector(nCalPhDecayMuVec, arraySize);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimEventAction::~PANDASimEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimEventAction::BeginOfEventAction(const G4Event* /*event*/)
{
	InitializeVector(energyDeposit);
	InitializeVector(energyDepositDelayH);
	InitializeVector(energyDepositDelayGd);
	InitializeVector(energyDepositDecayMu);

	InitializeVector(capTimeH);
	InitializeVector(capTimeGd);
	InitializeVector(decayTimeMu);

	InitializeVector(muTrackLength);

	//InitializeVector(nAbPhVec);
	//InitializeVector(nDtPhVec);

	InitializeVector(nCalPhVec);
	InitializeVector(nCalPhDelayHVec);
	InitializeVector(nCalPhDelayGdVec);
	InitializeVector(nCalPhDecayMuVec);

	fEdep = 0.;

	delayFlagH = false;
	delayFlagGd = false;
	decayFlagMu = false;

	//G4cout << "----------------------BeginOfEventAction------------------------------" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimEventAction::EndOfEventAction(const G4Event* event)
{
	// Get hits collections IDs (only once)
	if (fScinHCID == -1) {
		fScinHCID = G4SDManager::GetSDMpointer()->GetCollectionID("ScinitillatorHitsCollection");
	}
	if (fGdHCID == -1) {
		fGdHCID = G4SDManager::GetSDMpointer()->GetCollectionID("GdFilmHitsCollection");
	}
	if (fPhocathHCID == -1) {
		fPhocathHCID = G4SDManager::GetSDMpointer()->GetCollectionID("PhotocathodeHitsCollection");
	}

	// Get hits collections
	PANDASimScinHitsCollection* scinHC = static_cast<PANDASimScinHitsCollection*>(GetHitsCollection(fScinHCID, event));
	size_t hitsNum = scinHC->entries();
	for (G4int i = 0; i < hitsNum; i++)
	{
		G4int moduleRowReplicaNumber = i % arraySize;
		G4int moduleRepliaNumber = i / arraySize;
		PANDASimScinitillatorHit* scinHit = (*scinHC)[i];

		G4double edep = scinHit->GetEdep();
		energyDeposit[moduleRepliaNumber][moduleRowReplicaNumber] = edep;

		G4double edepH = scinHit->GetEdepH();
		energyDepositDelayH[moduleRepliaNumber][moduleRowReplicaNumber] = edepH;

		G4double edepGd = scinHit->GetEdepGd();
		energyDepositDelayGd[moduleRepliaNumber][moduleRowReplicaNumber] = edepGd;

		G4double edepMu = scinHit->GetEdepMu();
		energyDepositDecayMu[moduleRepliaNumber][moduleRowReplicaNumber] = edepMu;
		
		G4double tH = scinHit->GetCapTimeH();
		capTimeH[moduleRepliaNumber][moduleRowReplicaNumber] = tH;

		G4double tMu = scinHit->GetDecayTimeMu();
		decayTimeMu[moduleRepliaNumber][moduleRowReplicaNumber] = tMu;

		G4double muTrack = scinHit->GetMuTrack();
		muTrackLength[moduleRepliaNumber][moduleRowReplicaNumber] = muTrack;
	}

	PANDASimGdFilmHitsCollection* gdHC = static_cast<PANDASimGdFilmHitsCollection*>(GetHitsCollection(fGdHCID, event));
	hitsNum = gdHC->entries();
	for (int i = 0; i < hitsNum; i++)
	{
		G4int moduleRowReplicaNumber = i % arraySize;
		G4int moduleRepliaNumber = i / arraySize;
		auto gdHit = (*gdHC)[i];
	
		G4double tGd = gdHit->GetCapTimeGd();
		capTimeGd[moduleRepliaNumber][moduleRowReplicaNumber] = tGd;
	}

	auto* phocathHC = static_cast<PANDASimPhotocathodeHitsCollection*>(GetHitsCollection(fPhocathHCID, event));
	hitsNum = phocathHC->entries();
	for (int i = 0; i < hitsNum; i++)
	{
		G4int PMTCopyNumber = i / (arraySize * arraySize);
		G4int moduleRowReplicaNumber = (i - PMTCopyNumber * arraySize * arraySize) % arraySize;
		G4int moduleRepliaNumber = (i - PMTCopyNumber * arraySize * arraySize) / arraySize;
		auto phocathHit = (*phocathHC)[i];

		G4double nCalPhe = phocathHit->GetPheNum();
		nCalPhVec[moduleRepliaNumber][moduleRowReplicaNumber][PMTCopyNumber] = nCalPhe;

		G4double nCalPheH = phocathHit->GetPheNumH();
		nCalPhDelayHVec[moduleRepliaNumber][moduleRowReplicaNumber][PMTCopyNumber] = nCalPheH;

		G4double nCalPheGd = phocathHit->GetPheNumGd();
		nCalPhDelayGdVec[moduleRepliaNumber][moduleRowReplicaNumber][PMTCopyNumber] = nCalPheGd;

		G4double nCalPheMu = phocathHit->GetPheNumMu();
		nCalPhDecayMuVec[moduleRowReplicaNumber][moduleRepliaNumber][PMTCopyNumber] = nCalPheMu;
	}

	PANDASimRun* fPANDASimRun = static_cast<PANDASimRun*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
	if (fEdep != 0)
		fPANDASimRun->PushBackEnergyDeposit(fEdep);

	vector <vector<G4double> > double_empty2DVec(arraySize, vector<G4double>(arraySize));
	if (energyDeposit != double_empty2DVec)
		fPANDASimRun->PushBackModuleEnergyDeposit(energyDeposit);
	if (energyDepositDelayH != double_empty2DVec)
		fPANDASimRun->PushBackModuleEnergyDepositDelayH(energyDepositDelayH);
	if (energyDepositDelayGd != double_empty2DVec)
		fPANDASimRun->PushBackModuleEnergyDepositDelayGd(energyDepositDelayGd);
	if (energyDepositDecayMu != double_empty2DVec)
		fPANDASimRun->PushBackModuleEnergyDepositDecayMu(energyDepositDecayMu);

	if (capTimeGd != double_empty2DVec)
		fPANDASimRun->PushBackModuleCapTimeGd(capTimeGd);
	if (capTimeH != double_empty2DVec)
		fPANDASimRun->PushBackModuleCapTimeH(capTimeH);
	if (decayTimeMu != double_empty2DVec)
		fPANDASimRun->PushBackModuleDecayTimeMu(decayTimeMu);

	if (muTrackLength != double_empty2DVec)
		fPANDASimRun->PushBackModuleMuTrackLength(muTrackLength);

	/*vector <vector<G4int> > int_empty2DVec(arraySize, vector<G4int>(2));
	vector<vector<vector<G4int> > > int_empty3DVec(arraySize, int_empty2DVec);
	if (nAbPhVec != int_empty3DVec)
	{
		fPANDASimRun->PushBackModuleAbPh(nAbPhVec);
	}
	if (nDtPhVec != int_empty3DVec)
	{
		fPANDASimRun->PushBackModuleDtPh(nDtPhVec);
	}*/

	vector <vector<G4double> > double_empty2DVec32(arraySize, vector<G4double>(2));
	vector<vector<vector<G4double> > > double_empty3DVec(arraySize, double_empty2DVec32);
	if (nCalPhVec != double_empty3DVec)
		fPANDASimRun->PushBackModuleCalPh(nCalPhVec);
	if (nCalPhDelayHVec != double_empty3DVec)
		fPANDASimRun->PushBackModuleCalPhDelayH(nCalPhDelayHVec);
	if (nCalPhDelayGdVec != double_empty3DVec)
		fPANDASimRun->PushBackModuleCalPhDelayGd(nCalPhDelayGdVec);
	if (nCalPhDecayMuVec != double_empty3DVec)
		fPANDASimRun->PushBackModuleCalPhDecayMu(nCalPhDecayMuVec);

	G4int eventID = event->GetEventID();
	G4int eventNumber = UserDataInput::GetNumberOfEvents();
	if (eventID == 0 || ((eventID + 1) % (eventNumber / 10) == 0))
	{
		G4int per =(G4int) ((1. * eventID + 1) / (eventNumber * 0.01));
		//G4cout << " eventNumber: "<< eventNumber << " eventID: "<< eventID <<G4endl;
		auto seconds = time(NULL); // 格林威治时间
		seconds = seconds + 8 * 3600; // 北京时间
		G4int secondNow = seconds % 60;
		auto minutes = (seconds - secondNow) / 60;
		G4int minuteNow = minutes % 60;
		auto hours = (minutes - minuteNow) / 60;
		G4int hourNow = hours % 24;
		G4cout 
			<< " Time now: " << setw(2) << hourNow << ":" << setw(2) << minuteNow << ":" << setw(2) << secondNow << ". " 
			<< setw(3) << per << "% of simulation completed."
			<< G4endl;
		//getchar();
	}
	//G4cout << "-----------------------EndOfEventAction------------------------------" << G4endl << G4endl;
}

G4VHitsCollection* PANDASimEventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
	auto hitsCollection	= event->GetHCofThisEvent()->GetHC(hcID);

	if (!hitsCollection) {
		G4ExceptionDescription msg;
		msg << "Cannot access hitsCollection ID " << hcID;
		G4Exception("PANDASimEventAction::GetHitsCollection()",
			"MyCode0003", FatalException, msg);
	}

	return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PANDASimEventAction::ResizeVector(vector<vector<G4double> >& edepVec, G4int arrayNumber)
{
	edepVec.resize(arrayNumber);
	for (G4int i = 0; i < arrayNumber; i++)
	{
		edepVec[i].resize(arrayNumber);
	}
}

void PANDASimEventAction::ResizeVector(vector<vector<vector<G4int> > >& nPhVec, G4int arrayNumber)
{
	nPhVec.resize(arrayNumber);
	for (G4int i = 0; i < arrayNumber; ++i)
	{
		nPhVec[i].resize(arrayNumber);
		for (G4int j = 0; j < arrayNumber; ++j)
		{
			nPhVec[i][j].resize(2);
		}
	}
}

void PANDASimEventAction::ResizeVector(vector<vector<vector<G4double> > >& nPhVec, G4int arrayNumber)
{
	nPhVec.resize(arrayNumber);
	for (G4int i = 0; i < arrayNumber; ++i)
	{
		nPhVec[i].resize(arrayNumber);
		for (G4int j = 0; j < arrayNumber; ++j)
		{
			nPhVec[i][j].resize(2);
		}
	}
}

void PANDASimEventAction::InitializeVector(vector<vector<G4double> >& edepVec)
{
	for (G4int i = 0; i < edepVec.size(); i++)
	{
		for (G4int j = 0; j < edepVec[i].size(); j++)
		{
			edepVec[i][j] = 0.;
		}
	}
}

void PANDASimEventAction::InitializeVector(vector<vector<vector<G4int> > >& nPhVec)
{
	for (G4int i = 0; i < nPhVec.size(); i++)
	{
		for (G4int j = 0; j < nPhVec[i].size(); j++)
		{
			for (G4int k = 0; k < nPhVec[i][j].size(); ++k)
			{
				nPhVec[i][j][k] = 0;
			}
		}
	}
}

void PANDASimEventAction::InitializeVector(vector<vector<vector<G4double> > >& nPhVec)
{
	for (G4int i = 0; i < nPhVec.size(); i++)
	{
		for (G4int j = 0; j < nPhVec[i].size(); j++)
		{
			for (G4int k = 0; k < nPhVec[i][j].size(); ++k)
			{
				nPhVec[i][j][k] = 0.;
			}
		}
	}
}
