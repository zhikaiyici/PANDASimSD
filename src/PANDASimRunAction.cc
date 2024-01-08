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
#include "PANDASimPrimaryGeneratorMessenger.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"

#include "PANDASimAnalysis.hh"
#include "PANDASimEventAction.hh"
#include "UserDataInput.hh"

#include <fstream>
#include <iomanip>
#include <cmath>
#include <sstream>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#endif // _WIN32

#ifdef __linux__
#include <sys/stat.h> 
#include <sys/types.h>
#include <unistd.h>
#endif // __linux__

//using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimRunAction::PANDASimRunAction()
	: G4UserRunAction()
{
	//arraySize = UserDataInput::GetSizeOfArray();
	arraySize = (static_cast<const PANDASimDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction()))->GetArrySize();
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
	auto detectorConstruction = (static_cast<const PANDASimDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction()));

	G4LogicalVolumeStore* logicVolStroe = G4LogicalVolumeStore::GetInstance();
	auto containerLV = logicVolStroe->GetVolume("ContainerLV");
	G4Box* containerBox = dynamic_cast<G4Box*>(containerLV->GetSolid());
	G4double containerYHalfLength = containerBox->GetYHalfLength();

	auto moduleLV = logicVolStroe->GetVolume("ModuleLV");
	G4Box* moduleBox = dynamic_cast<G4Box*>(moduleLV->GetSolid());
	G4double moduleYHalfLength = moduleBox->GetYHalfLength();

	arraySize = (G4int)std::round(containerYHalfLength / moduleYHalfLength);
	myAccu->SetArraySize(arraySize);

	G4int nofEvents = run->GetNumberOfEventToBeProcessed();
	G4double doubNumOfEvents = nofEvents;
	if (IsMaster())
	{
		std::array<G4double, 3> dtctrXYZ = detectorConstruction->GetDetectorXYZ();
		G4double dtctrX = dtctrXYZ[0];// UserDataInput::GetDectorDimensionX() / cm;
		G4double dtctrY = dtctrXYZ[1];//UserDataInput::GetDectorDimensionY() / cm;
		G4double dtctrZ = dtctrXYZ[2];//UserDataInput::GetDectorDimensionZ() / cm;
		G4cout << G4endl << " Initialization completed. " << doubNumOfEvents << " event(s) will be simulated."
			   << G4endl
			   << " The dimension of detector module is " << dtctrX << " cm x " << dtctrY << " cm x " << dtctrZ << " cm."
			   << " The size of detector is " << arraySize << " x " << arraySize
			   << G4endl
			   << G4endl;
	}
	//G4int nofEvents = UserDataInput::GetNumberOfEvents();
	//G4int nofEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed();

	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Reset();

	//arraySize = UserDataInput::GetSizeOfArray();
	//G4double doubNumOfEvents = nofEvents;
	G4double gdFilmThickness = detectorConstruction->GetGdFilmThickness();// UserDataInput::GetGdFilmThickness() / um;
	std::ostringstream ossEventNumber; //ostrsGdFilmThickness, ossArraySize,
	ossEventNumber << std::setprecision(1) << doubNumOfEvents;
	//ossArraySize << arraySize;
	//ossNeutrinoPercentage << neutrinoPercentage;
	//ostrsGdFilmThickness << gdFilmThickness;

	G4String strEventNumber = ossEventNumber.str();;
	//G4String strEventNumber = std::to_string(doubNumOfEvents);
	G4String strArraySize = std::to_string(arraySize);
	G4String strGdFilmThickness = std::to_string(gdFilmThickness);

	//std::array<G4int, 2> neutrinoPosition = UserDataInput::GetPositionOfNeutrino();
	//G4String sourceType = UserDataInput::GetSoureType();
	//G4String sourcePosition = UserDataInput::GetSourePosition();

	G4String runCondition = "_" + strArraySize + "x" + strArraySize + "_" + strEventNumber + "_";

	auto generatorAction = static_cast<const PANDASimPrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

	if (generatorAction)
	{
		std::array<G4int, 2> neutrinoPosition = generatorAction->GetNeutrinoPosition();
		G4String strNeutrinoPosition;
		if (neutrinoPosition[0] < arraySize * arraySize)
		{
			G4String tmp;
			if (neutrinoPosition[1] < 5)
				tmp = std::to_string(neutrinoPosition[1]);
			else
				tmp = "Random";

			strNeutrinoPosition = std::to_string(neutrinoPosition[0]) + "_" + tmp;
		}
		else
			strNeutrinoPosition = "Random";

		auto sourceType = generatorAction->GetSourceType();
		auto sourcePosition = generatorAction->GetSourcePosition();

		if (sourceType != "NEUTRINO" && sourceType != "COSMICNEUTRON" && sourceType != "CRY" && sourceType != "He8" && sourceType != "Li9" && sourceType != "MUON")
		{
			if (sourceType == "GUN")
			{
				G4String particleName = generatorAction->GetParticleGun()->GetParticleDefinition()->GetParticleName();
				G4double particleEnergy = generatorAction->GetParticleGun()->GetParticleEnergy();
				std::ostringstream ossEnergy;
				ossEnergy << G4BestUnit(particleEnergy, "Energy");
				runCondition += particleName + "_" + ossEnergy.str() + "_" + sourcePosition;
			}
			else if (sourceType == "GPS")
			{
				runCondition += sourceType;
			}
			else
			{
				runCondition += sourceType + "_" + sourcePosition;
			}
		}
		else
		{
			if (sourceType != "NEUTRINO")
				runCondition += sourceType;
			else
				//runCondition += sourceType + "_" + strNeutrinoPercentage + "%_" + strNeutrinoPosition;
				runCondition += sourceType + "_" + strNeutrinoPosition;
		}

		myAccu->SetRunCondition(runCondition);

		G4String outDir = "output/" + runCondition;
		if (access(outDir, 6) == -1)
		{
#ifdef _WIN32
			mkdir(outDir);
#endif // _WIN32
#ifdef __linux__
			mkdir(outDir, S_IRWXU);
#endif
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

	// Print
	//  
	if (IsMaster())
	{
		std::list<G4double> betaKEHe8 = myAccu->GetBetaKEHe8();
		std::list<G4double> betaKELi9 = myAccu->GetBetaKELi9();
		std::list<G4double> decayTimeHe8 = myAccu->GetDecayTimeHe8();
		std::list<G4double> decayTimeLi9 = myAccu->GetDecayTimeLi9();
		std::list<G4double> neutronGenicTime = myAccu->GetNeutronGenicTime();

		std::vector<std::vector<G4int> > numHe8 = myAccu->GetNHe8();
		std::vector<std::vector<G4int> > numLi9 = myAccu->GetNLi9();
		std::vector<std::vector<G4int> > numNeutron = myAccu->GetNNeutron();

		auto runCondition = myAccu->GetRunCondition();

		G4String betaKEHe8FileName = "output/" + runCondition + "/betaKEHe8" + runCondition + ".txt";
		WriteDataToFile(betaKEHe8FileName, betaKEHe8);

		G4String betaKELi9FileName = "output/" + runCondition + "/betaKELi9" + runCondition + ".txt";
		WriteDataToFile(betaKELi9FileName, betaKELi9);

		G4String decayTimeHe8FileName = "output/" + runCondition + "/decayTimeHe8" + runCondition + ".txt";
		WriteDataToFile(decayTimeHe8FileName, decayTimeHe8);

		G4String decayTimeLi9FileName = "output/" + runCondition + "/decayTimeLi9" + runCondition + ".txt";
		WriteDataToFile(decayTimeLi9FileName, decayTimeLi9);

		G4String neutronGenicTimeFileName = "output/" + runCondition + "/neutronGenicTime" + runCondition + ".txt";
		WriteDataToFile(neutronGenicTimeFileName, neutronGenicTime);

		G4String numLi9FileName = "output/" + runCondition + "/numLi9" + runCondition + ".txt";
		WriteDataToFile(numLi9FileName, numLi9);

		G4String numHe8FileName = "output/" + runCondition + "/numHe8" + runCondition + ".txt";
		WriteDataToFile(numHe8FileName, numHe8);

		G4String numNeutronFileName = "output/" + runCondition + "/numNeutron" + runCondition + ".txt";
		WriteDataToFile(numNeutronFileName, numNeutron);

		const PANDASimRun* fPANDASimRun = static_cast<const PANDASimRun*>(run);

		//list<G4double> capTimeH = fPANDASimRun->GetCaptureTimeH();
		//G4String capTimeHFileName = "output/" + runCondition + "/capTimeH" + runCondition + ".txt";
		//WriteDataToFile(capTimeHFileName, capTimeH);

		//list<G4double> capTimeGd = fPANDASimRun->GetCaptureTimeGd();
		//G4String capTimeGdFileName = "output/" + runCondition + "/capTimeGd" + runCondition + ".txt";
		//WriteDataToFile(capTimeGdFileName, capTimeGd);

		//G4String decayTimeMuFileName = "output/" + runCondition + "/decayTimeMu" + runCondition + ".txt";
		//list<G4double> decayTimeMu = fPANDASimRun->GetDecayTimeMu();
		//WriteDataToFile(decayTimeMuFileName, decayTimeMu);

		//G4String edepFileName = "output/" + runCondition + "/edep" + runCondition + ".txt";
		//list<G4double> energyDeposit = fPANDASimRun->GetEnergyDeposit();
		//WriteDataToFile(edepFileName, energyDeposit);

		std::list<std::vector<std::vector<G4double> > > moduleEnergyDeposit = fPANDASimRun->GetModuleEnergyDeposit();
		G4String moduleEdepFileName = "output/" + runCondition + "/moduleEdepPrompt" + runCondition + ".txt";
		WriteDataToFile(moduleEdepFileName, moduleEnergyDeposit);

		std::list<std::vector<std::vector<G4double> > > moduleEnergyDepositDelayH = fPANDASimRun->GetModuleEnergyDepositDelayH();
		G4String moduleEdepDelayHFileName = "output/" + runCondition + "/moduleEdepDelayH" + runCondition + ".txt";
		WriteDataToFile(moduleEdepDelayHFileName, moduleEnergyDepositDelayH);

		std::list<std::vector<std::vector<G4double> > > moduleEnergyDepositDelayGd = fPANDASimRun->GetModuleEnergyDepositDelayGd();
		G4String moduleEdepDelayGdFileName = "output/" + runCondition + "/moduleEdepDelayGd" + runCondition + ".txt";
		WriteDataToFile(moduleEdepDelayGdFileName, moduleEnergyDepositDelayGd);

		std::list<std::vector<std::vector<G4double> > > moduleEnergyDepositDecayMu = fPANDASimRun->GetModuleEnergyDepositDecayMu();
		G4String moduleEdepDecayMuFileName = "output/" + runCondition + "/moduleEdepDecayMu" + runCondition + ".txt";
		WriteDataToFile(moduleEdepDecayMuFileName, moduleEnergyDepositDecayMu);

		std::list<std::vector<std::vector<G4double> > > moduleCapTimeGd = fPANDASimRun->GetModuleCapTimeGd();
		G4String moduleCapTimeGdFileName = "output/" + runCondition + "/moduleCapTimeGd" + runCondition + ".txt";
		WriteDataToFile(moduleCapTimeGdFileName, moduleCapTimeGd);

		std::list<std::vector<std::vector<G4double> > > moduleCapTimeH = fPANDASimRun->GetModuleCapTimeH();
		G4String moduleCapTimeHFileName = "output/" + runCondition + "/moduleCapTimeH" + runCondition + ".txt";
		WriteDataToFile(moduleCapTimeHFileName, moduleCapTimeH);

		std::list<std::vector<std::vector<G4double> > > moduleDecayTimeMu = fPANDASimRun->GetModuleDecayTimeMu();
		G4String moduleDecayTimeMuFileName = "output/" + runCondition + "/moduleDecayTimeMu" + runCondition + ".txt";
		WriteDataToFile(moduleDecayTimeMuFileName, moduleDecayTimeMu);

		std::list<std::vector<std::vector<G4double> > > moduleMuTrackLength = fPANDASimRun->GetModuleMuTrackLength();
		G4String moduleMuTrackLengthFileName = "output/" + runCondition + "/moduleMuTrackLength" + runCondition + ".txt";
		WriteDataToFile(moduleMuTrackLengthFileName, moduleMuTrackLength);

		std::list<std::vector<std::vector<G4double> > > moduleMuEdep = fPANDASimRun->GetModuleMuEdep();
		G4String moduleMuEdepFileName = "output/" + runCondition + "/moduleMuEdep" + runCondition + ".txt";
		WriteDataToFile(moduleMuEdepFileName, moduleMuEdep);

		//if (UserDataInput::GetOpticalPhysicsStatus() == true)
		//{
			auto moduleAbPh = fPANDASimRun->GetModuleAbPh();
			G4String moduleAbPhFileNameRight = "output/" + runCondition + "/moduleAbPhRight" + runCondition + ".txt";
			G4String moduleAbPhFileNameLeft = "output/" + runCondition + "/moduleAbPhLeft" + runCondition + ".txt";
			WriteDataToFile(moduleAbPhFileNameRight, moduleAbPhFileNameLeft, moduleAbPh);

			auto moduleDtPh = fPANDASimRun->GetModuleDtPh();
			G4String moduleDtPhFileNameRight = "output/" + runCondition + "/moduleDtPhRight" + runCondition + ".txt";
			G4String moduleDtPhFileNameLeft = "output/" + runCondition + "/moduleDtPhLeft" + runCondition + ".txt";
			WriteDataToFile(moduleDtPhFileNameRight, moduleDtPhFileNameLeft, moduleDtPh);

			auto moduleCalPh = fPANDASimRun->GetModuleCalPh();
			G4String moduleCalPhFileNameRight = "output/" + runCondition + "/moduleCalPhPromptRight" + runCondition + ".txt";
			G4String moduleCalPhFileNameLeft = "output/" + runCondition + "/moduleCalPhPromptLeft" + runCondition + ".txt";
			WriteDataToFile(moduleCalPhFileNameRight, moduleCalPhFileNameLeft, moduleCalPh);

			auto moduleCalPhDelayH = fPANDASimRun->GetModuleCalPhDelayH();
			G4String moduleCalPhDelayHFileNameRight = "output/" + runCondition + "/moduleCalPhDelayHRight" + runCondition + ".txt";
			G4String moduleCalPhDelayHFileNameLeft = "output/" + runCondition + "/moduleCalPhDelayHLeft" + runCondition + ".txt";
			WriteDataToFile(moduleCalPhDelayHFileNameRight, moduleCalPhDelayHFileNameLeft, moduleCalPhDelayH);

			auto moduleCalPhDelayGd = fPANDASimRun->GetModuleCalPhDelayGd();
			G4String moduleCalPhDelayGdFileNameRight = "output/" + runCondition + "/moduleCalPhDelayGdRight" + runCondition + ".txt";
			G4String moduleCalPhDelayGdFileNameLeft = "output/" + runCondition + "/moduleCalPhDelayGdLeft" + runCondition + ".txt";
			WriteDataToFile(moduleCalPhDelayGdFileNameRight, moduleCalPhDelayGdFileNameLeft, moduleCalPhDelayGd);

			auto moduleCalPhDecayMu = fPANDASimRun->GetModuleCalPhDecayMu();
			G4String moduleCalPhDecayMuFileNameRight = "output/" + runCondition + "/moduleCalPhDecayMuRight" + runCondition + ".txt";
			G4String moduleCalPhDecayMuFileNameLeft = "output/" + runCondition + "/moduleCalPhDecayMuLeft" + runCondition + ".txt";
			WriteDataToFile(moduleCalPhDecayMuFileNameRight, moduleCalPhDecayMuFileNameLeft, moduleCalPhDecayMu);
		//}

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

void PANDASimRunAction::WriteDataToFile(G4String fileName, std::list<G4double> data)
{
	if (data.empty()) return;
	std::ofstream outFile;
	outFile.open(fileName, std::ios_base::out);
	for (auto itr = data.begin(); itr != data.end(); ++itr)
	{
		if (*itr != 0)
		{
			outFile << *itr << G4endl;
		}
	}
	outFile.close();
}

void PANDASimRunAction::WriteDataToFile(G4String fileName, std::vector<std::vector<G4int> > data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4int> > empty2DVec(arraySize, std::vector<G4int>(arraySize, 0));
	if (data == empty2DVec) return;
	std::ofstream outFile;
	outFile.open(fileName, std::ios_base::out);
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

void PANDASimRunAction::WriteDataToFile(G4String fileName, std::list<std::vector<std::vector<G4int> > > data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4int> > empty2DVec(arraySize, std::vector<G4int>(arraySize, 0));
	std::ofstream outFile;
	outFile.open(fileName, std::ios_base::out);
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

void PANDASimRunAction::WriteDataToFile(G4String fileName, std::list<std::vector<std::vector<G4double> > > data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4double> > empty2DVec(arraySize, std::vector<G4double>(arraySize, 0));
	std::ofstream outFile;
	outFile.open(fileName, std::ios_base::out);
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

void PANDASimRunAction::WriteDataToFile(G4String fileNameRight, G4String fileNameLeft, std::list<std::vector<std::vector<std::vector<G4int> > > > data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4int> > empty2DVec(arraySize, std::vector<G4int>(2, 0));
	std::vector<std::vector<std::vector<G4int> > > empty3DVec(arraySize, empty2DVec);
	std::ofstream outFileRight, outFileLeft;
	outFileRight.open(fileNameRight, std::ios_base::out);
	outFileLeft.open(fileNameLeft, std::ios_base::out);
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

void PANDASimRunAction::WriteDataToFile(G4String fileNameRight, G4String fileNameLeft, std::list<std::vector<std::vector<std::vector<G4double> > > > data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4double> > empty2DVec(arraySize, std::vector<G4double>(2, 0));
	std::vector<std::vector<std::vector<G4double> > > empty3DVec(arraySize, empty2DVec);
	std::ofstream outFileRight, outFileLeft;
	outFileRight.open(fileNameRight, std::ios_base::out);
	outFileLeft.open(fileNameLeft, std::ios_base::out);
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

void PANDASimRunAction::PushNeutronGenicTime(G4double t)
{
	myAccu->PushNeutronGenicTime(t);
}

void PANDASimRunAction::AddNLi9(std::vector<std::vector<G4int>> nLi9)
{
	myAccu->AddNLi9(nLi9);
}

void PANDASimRunAction::AddNHe8(std::vector<std::vector<G4int>> nHe8)
{
	myAccu->AddNHe8(nHe8);
}

void PANDASimRunAction::AddNNeutron(std::vector<std::vector<G4int>> nNeutron)
{
	myAccu->AddNNeurtron(nNeutron);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
