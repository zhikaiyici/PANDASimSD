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
// $Id: PANDASimRunAction.cc 99560 2016-09-27 07:03:29Z gcosmo $
//
/// \file PANDASimRunAction.cc
/// \brief Implementation of the PANDASimRunAction class

#include "PANDASimRun.hh"
#include "PANDASimRunAction.hh"
#include "PANDASimPrimaryGeneratorAction.hh"
#include "PANDASimDetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "PANDASimAnalysis.hh"
#include "PANDASimEventAction.hh"
#include "UserDataInput.hh"

#include <fstream>
#include <iomanip>
#include <cmath>
#include <sstream>

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimRunAction::PANDASimRunAction()
	: G4UserRunAction()
{
	arraySize = UserDataInput::GetSizeOfArray();
	myAccu = new PANDASimAccumulable();

	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->RegisterAccumulable(myAccu);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimRunAction::~PANDASimRunAction()
{
	delete myAccu;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run* PANDASimRunAction::GenerateRun()
{
	return new PANDASimRun;
}

void PANDASimRunAction::BeginOfRunAction(const G4Run* run)
{
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Reset();

	if (IsMaster())
	{
		G4int nofEvents = run->GetNumberOfEventToBeProcessed();
		G4double doubNumOfEvents = nofEvents;
		G4double dtctrX = UserDataInput::GetDectorDimensionX() / cm;
		G4double dtctrY = UserDataInput::GetDectorDimensionY() / cm;
		G4double dtctrZ = UserDataInput::GetDectorDimensionZ() / cm;
		G4cout << G4endl << " Initialization completed. " << doubNumOfEvents << " event(s) will be simulated."
			   << G4endl
			   << " The dimension of detector module is " << dtctrX << " cm x " << dtctrY << " cm x " << dtctrZ << " cm."
			   << " The size of detector is " << arraySize << " x " << arraySize
			   << G4endl
			   << G4endl;

		//G4int nofEvents = UserDataInput::GetNumberOfEvents();
		//G4int nofEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed();
		arraySize = UserDataInput::GetSizeOfArray();
		G4double neutrinoPercentage = UserDataInput::GetNeutrinoPercentage() * 100.;
		//G4double doubNumOfEvents = nofEvents;
		G4double gdFilmThickness = UserDataInput::GetGdFilmThickness() / cm;
		ostringstream ostrsGdFilmThickness, ossArraySize, ossEventNumber, ossNeutrinoPercentage;
		ossEventNumber << setprecision(1) << doubNumOfEvents;
		ossArraySize << arraySize;
		ossNeutrinoPercentage << neutrinoPercentage;
		ostrsGdFilmThickness << gdFilmThickness;

		G4String strArraySize = ossArraySize.str();
		G4String strEventNumber = ossEventNumber.str();
		G4String strNeutrinoPercentage = ossNeutrinoPercentage.str();
		G4String strGdFilmThickness = ostrsGdFilmThickness.str();

		array<G4int, 2> neutrinoPosition = UserDataInput::GetPositionOfNeutrino();
		G4String strNeutrinoPosition;
		if (neutrinoPosition[0] < arraySize * arraySize)
		{
			G4String tmp;
			if (neutrinoPosition[1] < 5)
				tmp = to_string(neutrinoPosition[1]);
			else
				tmp = "Random";

			strNeutrinoPosition = to_string(neutrinoPosition[0]) + "_" + tmp;
		}
		else
			strNeutrinoPosition = "Random";

		G4String sourceType = UserDataInput::GetSoureType();
		G4String sourcePosition = UserDataInput::GetSourePosition();

		runCondition = "(" + strArraySize + "x" + strArraySize + "_" + strEventNumber + "_";
		if (sourceType != "NEUTRINO" && sourceType != "COSMICNEUTRON" && sourceType != "CRY" && sourceType != "He8" && sourceType != "Li9")
			runCondition += sourceType + "_" + sourcePosition + ")";
		else 
		{
			if (sourceType != "NEUTRINO")
				runCondition += sourceType + ")";
			else
				runCondition += sourceType + "_" + strNeutrinoPercentage + "%_" + strNeutrinoPosition + ")";
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimRunAction::EndOfRunAction(const G4Run* run)
{
	G4int nofEvents = run->GetNumberOfEvent();
	if (nofEvents == 0) return;

	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Merge();

	std::list<G4double> betaKEHe8 = myAccu->GetBetaKEHe8();
	std::list<G4double> betaKELi9 = myAccu->GetBetaKELi9();
	std::list<G4double> decayTimeHe8 = myAccu->GetDecayTimeHe8();
	std::list<G4double> decayTimeLi9 = myAccu->GetDecayTimeLi9();

	std::vector<std::vector<G4int> > numHe8 = myAccu->GetNHe8();
	std::vector<std::vector<G4int> > numLi9 = myAccu->GetNLi9();

	// Print
	//  
	if (IsMaster())
	{
		//G4int nofEvents = UserDataInput::GetNumberOfEvents();
		//arraySize = UserDataInput::GetSizeOfArray();
		//G4double neutrinoPercentage = UserDataInput::GetNeutrinoPercentage() * 100.;
		//G4double numofevent = nofEvents;
		//G4double gdFilmThickness = UserDataInput::GetGdFilmThickness() / cm;
		//ostringstream ostrsGdFilmThickness, ossArraySize, ossEventNumber, ossNeutrinoPercentage;
		//ossEventNumber << setprecision(1) << numofevent;
		//ossArraySize << arraySize;
		//ossNeutrinoPercentage << neutrinoPercentage;
		//ostrsGdFilmThickness << gdFilmThickness;
		//
		//G4String strArraySize = ossArraySize.str();
		//G4String strEventNumber = ossEventNumber.str();
		//G4String strNeutrinoPercentage = ossNeutrinoPercentage.str();
		//G4String strGdFilmThickness = ostrsGdFilmThickness.str();
		//
		//array<G4int, 2> neutrinoPosition = UserDataInput::GetPositionOfNeutrino();
		//G4String strNeutrinoPosition;
		//if (neutrinoPosition[0] < arraySize * arraySize)
		//{
		//	G4String tmp;
		//	if (neutrinoPosition[1] < 5)
		//		tmp = to_string(neutrinoPosition[1]);
		//	else
		//		tmp = "Random";
		//
		//	strNeutrinoPosition = to_string(neutrinoPosition[0]) + "_" + tmp;
		//}
		//else
		//	strNeutrinoPosition = "Random";
		//
		//G4String sourceType = UserDataInput::GetSoureType();
		//G4String sourcePosition = UserDataInput::GetSourePosition();
		//
		//runCondition = "(" + strArraySize + "x" + strArraySize + "_" + strEventNumber + "_";
		//if (sourceType != "NEUTRINO")
		//{
		//	runCondition += sourceType + "_" + sourcePosition + ")";
		//}
		//else
		//{
		//	runCondition += "neutrino_" + strNeutrinoPercentage + "%_" + strNeutrinoPosition + ")";
		//}

		G4String betaKEHe8FileName = "output/betaKEHe8" + runCondition + ".txt";
		WriteDataToFile(betaKEHe8FileName, betaKEHe8);

		G4String betaKELi9FileName = "output/betaKELi9" + runCondition + ".txt";
		WriteDataToFile(betaKELi9FileName, betaKELi9);

		G4String decayTimeHe8FileName = "output/decayTimeHe8" + runCondition + ".txt";
		WriteDataToFile(decayTimeHe8FileName, decayTimeHe8);

		G4String decayTimeLi9FileName = "output/decayTimeLi9" + runCondition + ".txt";
		WriteDataToFile(decayTimeLi9FileName, decayTimeLi9);

		G4String numLi9FileName = "output/numLi9" + runCondition + ".txt";
		WriteDataToFile(numLi9FileName, numLi9);

		G4String numHe8FileName = "output/numHe8" + runCondition + ".txt";
		WriteDataToFile(numHe8FileName, numHe8);

		const PANDASimRun* fPANDASimRun = static_cast<const PANDASimRun*>(run);

		//list<G4double> capTimeH = fPANDASimRun->GetCaptureTimeH();
		//G4String capTimeHFileName = "output/capTimeH" + runCondition + ".txt";
		//WriteDataToFile(capTimeHFileName, capTimeH);

		//list<G4double> capTimeGd = fPANDASimRun->GetCaptureTimeGd();
		//G4String capTimeGdFileName = "output/capTimeGd" + runCondition + ".txt";
		//WriteDataToFile(capTimeGdFileName, capTimeGd);

		//G4String decayTimeMuFileName = "output/decayTimeMu" + runCondition + ".txt";
		//list<G4double> decayTimeMu = fPANDASimRun->GetDecayTimeMu();
		//WriteDataToFile(decayTimeMuFileName, decayTimeMu);

		//G4String edepFileName = "output/edep" + runCondition + ".txt";
		//list<G4double> energyDeposit = fPANDASimRun->GetEnergyDeposit();
		//WriteDataToFile(edepFileName, energyDeposit);

		list<vector<vector<G4double> > > moduleEnergyDeposit = fPANDASimRun->GetModuleEnergyDeposit();
		G4String moduleEdepFileName = "output/moduleEdepPrompt" + runCondition + ".txt";
		WriteDataToFile(moduleEdepFileName, moduleEnergyDeposit);

		list<vector<vector<G4double> > > moduleEnergyDepositDelayH = fPANDASimRun->GetModuleEnergyDepositDelayH();
		G4String moduleEdepDelayHFileName = "output/moduleEdepDelayH" + runCondition + ".txt";
		WriteDataToFile(moduleEdepDelayHFileName, moduleEnergyDepositDelayH);

		list<vector<vector<G4double> > > moduleEnergyDepositDelayGd = fPANDASimRun->GetModuleEnergyDepositDelayGd();
		G4String moduleEdepDelayGdFileName = "output/moduleEdepDelayGd" + runCondition + ".txt";
		WriteDataToFile(moduleEdepDelayGdFileName, moduleEnergyDepositDelayGd);

		list<vector<vector<G4double> > > moduleEnergyDepositDecayMu = fPANDASimRun->GetModuleEnergyDepositDecayMu();
		G4String moduleEdepDecayMuFileName = "output/moduleEdepDecayMu" + runCondition + ".txt";
		WriteDataToFile(moduleEdepDecayMuFileName, moduleEnergyDepositDecayMu);

		list<vector<vector<G4double> > > moduleCapTimeGd = fPANDASimRun->GetModuleCapTimeGd();
		G4String moduleCapTimeGdFileName = "output/moduleCapTimeGd" + runCondition + ".txt";
		WriteDataToFile(moduleCapTimeGdFileName, moduleCapTimeGd);

		list<vector<vector<G4double> > > moduleCapTimeH = fPANDASimRun->GetModuleCapTimeH();
		G4String moduleCapTimeHFileName = "output/moduleCapTimeH" + runCondition + ".txt";
		WriteDataToFile(moduleCapTimeHFileName, moduleCapTimeH);

		list<vector<vector<G4double> > > moduleDecayTimeMu = fPANDASimRun->GetModuleDecayTimeMu();
		G4String moduleDecayTimeMuFileName = "output/moduleDecayTimeMu" + runCondition + ".txt";
		WriteDataToFile(moduleDecayTimeMuFileName, moduleDecayTimeMu);

		list<vector<vector<G4double> > > moduleMuTrackLength = fPANDASimRun->GetModuleMuTrackLength();
		G4String moduleMuTrackLengthFileName = "output/moduleMuTrackLength" + runCondition + ".txt";
		WriteDataToFile(moduleMuTrackLengthFileName, moduleMuTrackLength);

		list<vector<vector<G4double> > > moduleMuEdep = fPANDASimRun->GetModuleMuEdep();
		G4String moduleMuEdepFileName = "output/moduleMuEdep" + runCondition + ".txt";
		WriteDataToFile(moduleMuEdepFileName, moduleMuEdep);

		if (UserDataInput::GetOpticalPhysicsStatus() == true)
		{
			auto moduleAbPh = fPANDASimRun->GetModuleAbPh();
			G4String moduleAbPhFileNameRight = "output/moduleAbPhRight" + runCondition + ".txt";
			G4String moduleAbPhFileNameLeft = "output/moduleAbPhLeft" + runCondition + ".txt";
			WriteDataToFile(moduleAbPhFileNameRight, moduleAbPhFileNameLeft, moduleAbPh);

			auto moduleDtPh = fPANDASimRun->GetModuleDtPh();
			G4String moduleDtPhFileNameRight = "output/moduleDtPhRight" + runCondition + ".txt";
			G4String moduleDtPhFileNameLeft = "output/moduleDtPhLeft" + runCondition + ".txt";
			WriteDataToFile(moduleDtPhFileNameRight, moduleDtPhFileNameLeft, moduleDtPh);

			auto moduleCalPh = fPANDASimRun->GetModuleCalPh();
			G4String moduleCalPhFileNameRight = "output/moduleCalPhPromptRight" + runCondition + ".txt";
			G4String moduleCalPhFileNameLeft = "output/moduleCalPhPromptLeft" + runCondition + ".txt";
			WriteDataToFile(moduleCalPhFileNameRight, moduleCalPhFileNameLeft, moduleCalPh);

			auto moduleCalPhDelayH = fPANDASimRun->GetModuleCalPhDelayH();
			G4String moduleCalPhDelayHFileNameRight = "output/moduleCalPhDelayHRight" + runCondition + ".txt";
			G4String moduleCalPhDelayHFileNameLeft = "output/moduleCalPhDelayHLeft" + runCondition + ".txt";
			WriteDataToFile(moduleCalPhDelayHFileNameRight, moduleCalPhDelayHFileNameLeft, moduleCalPhDelayH);

			auto moduleCalPhDelayGd = fPANDASimRun->GetModuleCalPhDelayGd();
			G4String moduleCalPhDelayGdFileNameRight = "output/moduleCalPhDelayGdRight" + runCondition + ".txt";
			G4String moduleCalPhDelayGdFileNameLeft = "output/moduleCalPhDelayGdLeft" + runCondition + ".txt";
			WriteDataToFile(moduleCalPhDelayGdFileNameRight, moduleCalPhDelayGdFileNameLeft, moduleCalPhDelayGd);

			auto moduleCalPhDecayMu = fPANDASimRun->GetModuleCalPhDecayMu();
			G4String moduleCalPhDecayMuFileNameRight = "output/moduleCalPhDecayMuRight" + runCondition + ".txt";
			G4String moduleCalPhDecayMuFileNameLeft = "output/moduleCalPhDecayMuLeft" + runCondition + ".txt";
			WriteDataToFile(moduleCalPhDecayMuFileNameRight, moduleCalPhDecayMuFileNameLeft, moduleCalPhDecayMu);
		}

		G4cout
			<< G4endl
			<< "--------------------End of Global Run-----------------------"
			<< G4endl;
	}
	else {
		G4cout
			<< G4endl
			<< "--------------------End of Local Run------------------------"
			<< G4endl;
	}
}

void PANDASimRunAction::WriteDataToFile(G4String fileName, list<G4double> data)
{
	if (data.empty()) return;
	ofstream outFile;
	outFile.open(fileName, ios_base::out);
	for (auto itr = data.begin(); itr != data.end(); ++itr)
	{
		if (*itr != 0)
		{
			outFile << *itr << G4endl;
		}
	}
	outFile.close();
}

void PANDASimRunAction::WriteDataToFile(G4String fileName, vector<vector<G4int> > data)
{
	if (data.empty()) return;
	vector <vector<G4int> > empty2DVec(arraySize, vector<G4int>(arraySize, 0));
	if (data == empty2DVec) return;
	ofstream outFile;
	outFile.open(fileName, ios_base::out);
	for (auto itrVector = data.begin(); itrVector != data.end(); ++itrVector)
	{
		for (auto itr = (*itrVector).begin(); itr != (*itrVector).end(); ++itr)
		{
			outFile << " " << *itr;
		}
		outFile << G4endl;
	}
	//outFile << G4endl;
	outFile.close();
}

void PANDASimRunAction::WriteDataToFile(G4String fileName, list<vector<vector<G4int> > > data)
{
	if (data.empty()) return;
	vector <vector<G4int> > empty2DVec(arraySize, vector<G4int>(arraySize, 0));
	ofstream outFile;
	outFile.open(fileName, ios_base::out);
	for (auto itrList = data.begin(); itrList != data.end(); ++itrList)
	{
		if (*itrList != empty2DVec)
		{
			for (auto itr2DVector = (*itrList).begin(); itr2DVector != (*itrList).end(); ++itr2DVector)
			{
				for (auto itrVector = (*itr2DVector).begin(); itrVector != (*itr2DVector).end(); ++itrVector)
				{
					outFile << " " << *itrVector;
				}
				outFile << G4endl;
			}
			//outFile << G4endl;
		}
	}
	outFile.close();
}

void PANDASimRunAction::WriteDataToFile(G4String fileName, list<vector<vector<G4double> > > data)
{
	if (data.empty()) return;
	vector <vector<G4double> > empty2DVec(arraySize, vector<G4double>(arraySize, 0));
	ofstream outFile;
	outFile.open(fileName, ios_base::out);
	for (auto itrList = data.begin(); itrList != data.end(); ++itrList)
	{
		if (*itrList != empty2DVec)
		{
			for (auto itr2DVector = (*itrList).begin(); itr2DVector != (*itrList).end(); ++itr2DVector)
			{
				for (auto itrVector = (*itr2DVector).begin(); itrVector != (*itr2DVector).end(); ++itrVector)
				{
					outFile << " " << *itrVector;
				}
				outFile << G4endl;
			}
			//outFile << G4endl;
		}
	}
	outFile.close();
}

void PANDASimRunAction::WriteDataToFile(G4String fileNameRight, G4String fileNameLeft, list<vector<vector<vector<G4int> > > > data)
{
	if (data.empty()) return;
	vector <vector<G4int> > empty2DVec(arraySize, vector<G4int>(2, 0));
	vector<vector<vector<G4int> > > empty3DVec(arraySize, empty2DVec);
	ofstream outFileRight, outFileLeft;
	outFileRight.open(fileNameRight, ios_base::out);
	outFileLeft.open(fileNameLeft, ios_base::out);
	for (auto itrList = data.begin(); itrList != data.end(); ++itrList)
	{
		if (*itrList != empty3DVec)
		{
			for (auto itr3DVector = (*itrList).begin(); itr3DVector != (*itrList).end(); ++itr3DVector)
			{
				for (auto itr2DVector = (*itr3DVector).begin(); itr2DVector != (*itr3DVector).end(); ++itr2DVector)
				{
					outFileRight << " " << *(*itr2DVector).begin();
					outFileLeft << " " << *((*itr2DVector).end() - 1);
				}
				outFileRight << G4endl;
				outFileLeft << G4endl;
			}
			//outFileRight << G4endl;
			//outFileLeft << G4endl;
		}
	}
	outFileRight.close();
	outFileLeft.close();
}

void PANDASimRunAction::WriteDataToFile(G4String fileNameRight, G4String fileNameLeft, list<vector<vector<vector<G4double> > > > data)
{
	if (data.empty()) return;
	vector <vector<G4double> > empty2DVec(arraySize, vector<G4double>(2, 0));
	vector<vector<vector<G4double> > > empty3DVec(arraySize, empty2DVec);
	ofstream outFileRight, outFileLeft;
	outFileRight.open(fileNameRight, ios_base::out);
	outFileLeft.open(fileNameLeft, ios_base::out);
	for (auto itrList = data.begin(); itrList != data.end(); ++itrList)
	{
		if (*itrList != empty3DVec)
		{
			for (auto itr3DVector = (*itrList).begin(); itr3DVector != (*itrList).end(); ++itr3DVector)
			{
				for (auto itr2DVector = (*itr3DVector).begin(); itr2DVector != (*itr3DVector).end(); ++itr2DVector)
				{
					outFileRight << " "  << *(*itr2DVector).begin();
					outFileLeft << " " << *((*itr2DVector).end() - 1);
				}
				outFileRight << G4endl;
				outFileLeft << G4endl;
			}
			//outFileRight << G4endl;
			//outFileLeft << G4endl;
		}
	}
	outFileRight.close();
	outFileLeft.close();
}

void PANDASimRunAction::PushBetaKEHe8(G4double bke)
{
	myAccu->PushBetaKEHe8(bke);
}

void PANDASimRunAction::PushBetaKELi9(G4double bke)
{
	myAccu->PushBetaKELi9(bke);
}

void PANDASimRunAction::PushDecayTimeHe8(G4double dt)
{
	myAccu->PushDecayTimeHe8(dt);
}

void PANDASimRunAction::PushDecayTimeLi9(G4double dt)
{
	myAccu->PushDecayTimeLi9(dt);
}

void PANDASimRunAction::AddNLi9(std::vector<std::vector<G4int>> nLi9)
{
	myAccu->AddNLi9(nLi9);
}

void PANDASimRunAction::AddNHe8(std::vector<std::vector<G4int>> nHe8)
{
	myAccu->AddNHe8(nHe8);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
