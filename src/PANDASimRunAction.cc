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
	myAccu = new PANDASimAccumulable(arraySize);

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
			   << " The dimension of detector module is " << dtctrX / cm << " cm x " << dtctrY /cm << " cm x " << dtctrZ / cm << " cm."
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
		auto runCondition = myAccu->GetRunCondition();
		G4int runID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();

		G4String outDir = "output/" + std::to_string(runID) + runCondition;
		if (access(outDir, 6) == -1)
		{
#ifdef _WIN32
			mkdir(outDir);
#endif // _WIN32
#ifdef __linux__
			mkdir(outDir, S_IRWXU);
#endif
		}

		std::list<G4double> betaKEHe8 = myAccu->GetBetaKEHe8();
		G4String betaKEHe8FileName = outDir + "/betaKEHe8" + runCondition + ".data";
		WriteDataToFile(betaKEHe8FileName, betaKEHe8);
		betaKEHe8.clear();

		std::list<G4double> betaKELi9 = myAccu->GetBetaKELi9();
		G4String betaKELi9FileName = outDir + "/betaKELi9" + runCondition + ".data";
		WriteDataToFile(betaKELi9FileName, betaKELi9);
		betaKELi9.clear();

		std::list<G4double> decayTimeHe8 = myAccu->GetDecayTimeHe8();
		G4String decayTimeHe8FileName = outDir + "/decayTimeHe8" + runCondition + ".data";
		WriteDataToFile(decayTimeHe8FileName, decayTimeHe8);
		decayTimeHe8.clear();

		std::list<G4double> decayTimeLi9 = myAccu->GetDecayTimeLi9();
		G4String decayTimeLi9FileName = outDir + "/decayTimeLi9" + runCondition + ".data";
		WriteDataToFile(decayTimeLi9FileName, decayTimeLi9);
		decayTimeLi9.clear();

		std::list<G4double> neutronGT = myAccu->GetNeutronGT();
		G4String neutronGTFileName = outDir + "/neutronGT" + runCondition + ".data";
		WriteDataToFile(neutronGTFileName, neutronGT);
		neutronGT.clear();

		std::list<G4double> neutronKE = myAccu->GetNeutronKE();
		G4String neutronKEFileName = outDir + "/neutronKE" + runCondition + ".data";
		WriteDataToFile(neutronKEFileName, neutronKE);
		neutronKE.clear();

		std::list<G4double> capTimeH = myAccu->GetCapTimeH();
		G4String capTimeHFileName = outDir + "/capTimeH" + runCondition + ".data";
		WriteDataToFile(capTimeHFileName, capTimeH);
		capTimeH.clear();

		std::list<G4double> capTimeGd = myAccu->GetCapTimeGd();
		G4String capTimeGdFileName = outDir + "/capTimeGd" + runCondition + ".data";
		WriteDataToFile(capTimeGdFileName, capTimeGd);
		capTimeGd.clear();

		std::list<std::vector<std::vector<G4double>>> neutronGenicTime = myAccu->GetNeutronGenicTime();
		G4String neutronGenicTimeFileName = outDir + "/moduleNeutronGenicTime" + runCondition + ".data";
		WriteDataToFile(neutronGenicTimeFileName, neutronGenicTime);
		neutronGenicTime.clear();

		std::list<std::vector<std::vector<G4double>>> neutronKineticEnergy = myAccu->GetNeutronKineticEnergy();
		G4String neutronKineticEnergyFileName = outDir + "/moduleNeutronKineticEnergy" + runCondition + ".data";
		WriteDataToFile(neutronKineticEnergyFileName, neutronKineticEnergy);
		neutronKineticEnergy.clear();

		std::list<std::vector<std::vector<G4double>>> edepDecay = myAccu->GetEdepDecay();
		G4String edepDecayFileName = outDir + "/moduleEdepDecay" + runCondition + ".data";
		WriteDataToFile(edepDecayFileName, edepDecay);
		edepDecay.clear();

		std::list<std::vector<std::vector<std::vector<G4double>>>> moduleCalPhDecay = myAccu->GetModuleCalPhDecay();
		G4String moduleCalPhDecayFileNameRight = outDir + "/moduleCalPhDecayRight" + runCondition + ".data";
		G4String moduleCalPhDecayFileNameLeft = outDir + "/moduleCalPhDecayLeft" + runCondition + ".data";
		WriteDataToFile(moduleCalPhDecayFileNameRight, moduleCalPhDecayFileNameLeft, moduleCalPhDecay);
		moduleCalPhDecay.clear();

		std::vector<std::vector<G4int>> numLi9 = myAccu->GetNLi9();
		G4String numLi9FileName = outDir + "/moduleNumLi9" + runCondition + ".txt";
		WriteDataToFile(numLi9FileName, numLi9);

		std::vector<std::vector<G4int>> numHe8 = myAccu->GetNHe8();
		G4String numHe8FileName = outDir + "/moduleNumHe8" + runCondition + ".txt";
		WriteDataToFile(numHe8FileName, numHe8);

		std::vector<std::vector<G4int>> numNeutron = myAccu->GetNNeutron();
		G4String numNeutronFileName = outDir + "/modulenumNeutron" + runCondition + ".txt";
		WriteDataToFile(numNeutronFileName, numNeutron);

		const PANDASimRun* fPANDASimRun = static_cast<const PANDASimRun*>(run);

		//list<G4double> capTimeH = fPANDASimRun->GetCaptureTimeH();
		//G4String capTimeHFileName = outDir + "/capTimeH" + runCondition + ".data";
		//WriteDataToFile(capTimeHFileName, capTimeH);

		//list<G4double> capTimeGd = fPANDASimRun->GetCaptureTimeGd();
		//G4String capTimeGdFileName = outDir + "/capTimeGd" + runCondition + ".data";
		//WriteDataToFile(capTimeGdFileName, capTimeGd);

		//G4String decayTimeMuFileName = outDir + "/decayTimeMu" + runCondition + ".data";
		//list<G4double> decayTimeMu = fPANDASimRun->GetDecayTimeMu();
		//WriteDataToFile(decayTimeMuFileName, decayTimeMu);

		//G4String edepFileName = outDir + "/edep" + runCondition + ".data";
		//list<G4double> energyDeposit = fPANDASimRun->GetEnergyDeposit();
		//WriteDataToFile(edepFileName, energyDeposit);

		std::list<std::vector<std::vector<G4double>>> moduleEnergyDeposit = fPANDASimRun->GetModuleEnergyDeposit();
		G4String moduleEdepFileName = outDir + "/moduleEdepPrompt" + runCondition + ".data";
		WriteDataToFile(moduleEdepFileName, moduleEnergyDeposit);
		moduleEnergyDeposit.clear();

		std::list<std::vector<std::vector<G4double>>> moduleEnergyDepositDelayH = fPANDASimRun->GetModuleEnergyDepositDelayH();
		G4String moduleEdepDelayHFileName = outDir + "/moduleEdepDelayH" + runCondition + ".data";
		WriteDataToFile(moduleEdepDelayHFileName, moduleEnergyDepositDelayH);
		moduleEnergyDepositDelayH.clear();

		std::list<std::vector<std::vector<G4double>>> moduleEnergyDepositDelayGd = fPANDASimRun->GetModuleEnergyDepositDelayGd();
		G4String moduleEdepDelayGdFileName = outDir + "/moduleEdepDelayGd" + runCondition + ".data";
		WriteDataToFile(moduleEdepDelayGdFileName, moduleEnergyDepositDelayGd);
		moduleEnergyDepositDelayGd.clear();

		std::list<std::vector<std::vector<G4double>>> moduleEnergyDepositDecayMu = fPANDASimRun->GetModuleEnergyDepositDecayMu();
		G4String moduleEdepDecayMuFileName = outDir + "/moduleEdepDecayMu" + runCondition + ".data";
		WriteDataToFile(moduleEdepDecayMuFileName, moduleEnergyDepositDecayMu);
		moduleEnergyDepositDecayMu.clear();

		std::list<std::vector<std::vector<G4double>>> moduleCapTimeGd = fPANDASimRun->GetModuleCapTimeGd();
		G4String moduleCapTimeGdFileName = outDir + "/moduleCapTimeGd" + runCondition + ".data";
		WriteDataToFile(moduleCapTimeGdFileName, moduleCapTimeGd);
		moduleCapTimeGd.clear();

		std::list<std::vector<std::vector<G4double>>> moduleCapTimeH = fPANDASimRun->GetModuleCapTimeH();
		G4String moduleCapTimeHFileName = outDir + "/moduleCapTimeH" + runCondition + ".data";
		WriteDataToFile(moduleCapTimeHFileName, moduleCapTimeH);
		moduleCapTimeH.clear();

		std::list<std::vector<std::vector<G4double>>> moduleDecayTimeMu = fPANDASimRun->GetModuleDecayTimeMu();
		G4String moduleDecayTimeMuFileName = outDir + "/moduleDecayTimeMu" + runCondition + ".data";
		WriteDataToFile(moduleDecayTimeMuFileName, moduleDecayTimeMu);
		moduleDecayTimeMu.clear();

		std::list<std::vector<std::vector<G4double>>> moduleMuTrackLength = fPANDASimRun->GetModuleMuTrackLength();
		G4String moduleMuTrackLengthFileName = outDir + "/moduleMuTrackLength" + runCondition + ".data";
		WriteDataToFile(moduleMuTrackLengthFileName, moduleMuTrackLength);
		moduleMuTrackLength.clear();

		std::list<std::vector<std::vector<G4double>>> moduleMuEdep = fPANDASimRun->GetModuleMuEdep();
		G4String moduleMuEdepFileName = outDir + "/moduleMuEdep" + runCondition + ".data";
		WriteDataToFile(moduleMuEdepFileName, moduleMuEdep);
		moduleMuEdep.clear();

		//if (UserDataInput::GetOpticalPhysicsStatus() == true)
		//{
			//auto moduleAbPh = fPANDASimRun->GetModuleAbPh();
			//G4String moduleAbPhFileNameRight = outDir + "/moduleAbPhRight" + runCondition + ".data";
			//G4String moduleAbPhFileNameLeft = outDir + "/moduleAbPhLeft" + runCondition + ".data";
			//WriteDataToFile(moduleAbPhFileNameRight, moduleAbPhFileNameLeft, moduleAbPh);

			//auto moduleDtPh = fPANDASimRun->GetModuleDtPh();
			//G4String moduleDtPhFileNameRight = outDir + "/moduleDtPhRight" + runCondition + ".data";
			//G4String moduleDtPhFileNameLeft = outDir + "/moduleDtPhLeft" + runCondition + ".data";
			//WriteDataToFile(moduleDtPhFileNameRight, moduleDtPhFileNameLeft, moduleDtPh);

			auto moduleCalPh = fPANDASimRun->GetModuleCalPh();
			G4String moduleCalPhFileNameRight = outDir + "/moduleCalPhPromptRight" + runCondition + ".data";
			G4String moduleCalPhFileNameLeft = outDir + "/moduleCalPhPromptLeft" + runCondition + ".data";
			WriteDataToFile(moduleCalPhFileNameRight, moduleCalPhFileNameLeft, moduleCalPh);
			moduleCalPh.clear();

			auto moduleCalPhDelayH = fPANDASimRun->GetModuleCalPhDelayH();
			G4String moduleCalPhDelayHFileNameRight = outDir + "/moduleCalPhDelayHRight" + runCondition + ".data";
			G4String moduleCalPhDelayHFileNameLeft = outDir + "/moduleCalPhDelayHLeft" + runCondition + ".data";
			WriteDataToFile(moduleCalPhDelayHFileNameRight, moduleCalPhDelayHFileNameLeft, moduleCalPhDelayH);
			moduleCalPhDelayH.clear();

			auto moduleCalPhDelayGd = fPANDASimRun->GetModuleCalPhDelayGd();
			G4String moduleCalPhDelayGdFileNameRight = outDir + "/moduleCalPhDelayGdRight" + runCondition + ".data";
			G4String moduleCalPhDelayGdFileNameLeft = outDir + "/moduleCalPhDelayGdLeft" + runCondition + ".data";
			WriteDataToFile(moduleCalPhDelayGdFileNameRight, moduleCalPhDelayGdFileNameLeft, moduleCalPhDelayGd);
			moduleCalPhDelayGd.clear();

			auto moduleCalPhDecayMu = fPANDASimRun->GetModuleCalPhDecayMu();
			G4String moduleCalPhDecayMuFileNameRight = outDir + "/moduleCalPhDecayMuRight" + runCondition + ".data";
			G4String moduleCalPhDecayMuFileNameLeft = outDir + "/moduleCalPhDecayMuLeft" + runCondition + ".data";
			WriteDataToFile(moduleCalPhDecayMuFileNameRight, moduleCalPhDecayMuFileNameLeft, moduleCalPhDecayMu);
			moduleCalPhDecayMu.clear();
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

void PANDASimRunAction::WriteDataToFile(const G4String& fileName, const std::list<G4double>& data)
{
	if (data.empty()) return;
	std::ofstream outFile;
	//outFile.open(fileName, std::ios_base::out);
	outFile.open(fileName, std::ios_base::out | std::ios::binary);
	for (auto itr = data.begin(); itr != data.end(); ++itr)
	{
		if (*itr != 0)
		{
			//outFile << *itr << G4endl;
			outFile.write((char*)&(*itr), sizeof(*itr));
		}
	}
	outFile.close();
}

void PANDASimRunAction::WriteDataToFile(const G4String& fileName, const std::vector<std::vector<G4int>>& data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4int> > empty2DVec(arraySize, std::vector<G4int>(arraySize, 0));
	if (data == empty2DVec) return;
	std::ofstream outFile;
	outFile.open(fileName, std::ios_base::out);
	//outFile.open(fileName, std::ios_base::out | std::ios::binary);
	for (auto itrVector = data.begin(); itrVector != data.end(); ++itrVector)
	{
		//outFile.write((char*)itrVector->data(), sizeof((*itrVector)[0]) * itrVector->size());
		for (auto itr = itrVector->begin(); itr != itrVector->end(); ++itr)
		{
			outFile << " " << *itr;
		}
		outFile << G4endl;
	}
	outFile.close();
}

void PANDASimRunAction::WriteDataToFile(const G4String& fileName, std::list<std::vector<std::vector<G4int>>>& data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4int> > empty2DVec(arraySize, std::vector<G4int>(arraySize, 0));
	std::ofstream outFile;
	//outFile.open(fileName, std::ios_base::out);
	outFile.open(fileName, std::ios_base::out | std::ios::binary);
	for (auto itrList = data.begin(); itrList != data.end(); ++itrList)
	{
		if (*itrList != empty2DVec)
		{
			for (auto itr2DVector = itrList->begin(); itr2DVector != itrList->end(); ++itr2DVector)
			{
				outFile.write((char*)itr2DVector->data(), sizeof((*itr2DVector)[0]) * itr2DVector->size());
				//for (auto itrVector = itr2DVector->begin(); itrVector != itr2DVector->end(); ++itrVector)
				//{
				//	outFile << " " << *itrVector;
				//}
				//outFile << G4endl;
			}
		}
	}
	outFile.close();
}

void PANDASimRunAction::WriteDataToFile(const G4String& fileName, std::list<std::vector<std::vector<G4double>>> data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4double> > empty2DVec(arraySize, std::vector<G4double>(arraySize, 0));
	std::ofstream outFile;
	//outFile.open(fileName, std::ios_base::out);
	outFile.open(fileName, std::ios_base::out | std::ios::binary);
	for (auto itrList = data.begin(); itrList != data.end(); ++itrList)
	{
		if (*itrList != empty2DVec)
		{
			for (auto itr2DVector = itrList->begin(); itr2DVector != itrList->end(); ++itr2DVector)
			{
				outFile.write((char*)itr2DVector->data(), sizeof((*itr2DVector)[0]) * itr2DVector->size());
				//for (auto itrVector = itr2DVector->begin(); itrVector != itr2DVector->end(); ++itrVector)
				//{
				//	outFile << " " << *itrVector;
				//}
				//outFile << G4endl;
			}
		}
	}
	outFile.close();
}

void PANDASimRunAction::WriteDataToFile(const G4String& fileNameRight, const G4String& fileNameLeft, const std::list<std::vector<std::vector<std::vector<G4int>>>>& data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4int> > empty2DVec(arraySize, std::vector<G4int>(2, 0));
	std::vector<std::vector<std::vector<G4int> > > empty3DVec(arraySize, empty2DVec);
	std::ofstream outFileRight, outFileLeft;
	//outFileRight.open(fileNameRight, std::ios_base::out);
	//outFileLeft.open(fileNameLeft, std::ios_base::out);
	outFileRight.open(fileNameRight, std::ios_base::out | std::ios::binary);
	outFileLeft.open(fileNameLeft, std::ios_base::out | std::ios::binary);
	for (auto itrList = data.begin(); itrList != data.end(); ++itrList)
	{
		if (*itrList != empty3DVec)
		{
			for (auto itr3DVector = itrList->begin(); itr3DVector != itrList->end(); ++itr3DVector)
			{
				for (auto itr2DVector = itr3DVector->begin(); itr2DVector != itr3DVector->end(); ++itr2DVector)
				{
					outFileRight.write((char*)itr2DVector->data(), sizeof((*itr2DVector)[0]) * itr2DVector->size());
					outFileLeft.write((char*)itr2DVector->data(), sizeof((*itr2DVector)[0]) * itr2DVector->size());
					//outFileRight << " " << *itr2DVector->begin();
					//outFileLeft << " " << *(itr2DVector->end() - 1);
				}
				//outFileRight << G4endl;
				//outFileLeft << G4endl;
			}
		}
	}
	outFileRight.close();
	outFileLeft.close();
}

void PANDASimRunAction::WriteDataToFile(const G4String& fileNameRight, const G4String& fileNameLeft, const std::list<std::vector<std::vector<std::vector<G4double>>>>& data)
{
	if (data.empty()) return;
	std::vector <std::vector<G4double> > empty2DVec(arraySize, std::vector<G4double>(2, 0));
	std::vector<std::vector<std::vector<G4double> > > empty3DVec(arraySize, empty2DVec);
	std::ofstream outFileRight, outFileLeft;
	//outFileRight.open(fileNameRight, std::ios_base::out);
	//outFileLeft.open(fileNameLeft, std::ios_base::out);
	outFileRight.open(fileNameRight, std::ios_base::out | std::ios::binary);
	outFileLeft.open(fileNameLeft, std::ios_base::out | std::ios::binary);
	for (auto itrList = data.begin(); itrList != data.end(); ++itrList)
	{
		if (*itrList != empty3DVec)
		{
			for (auto itr3DVector = itrList->begin(); itr3DVector != itrList->end(); ++itr3DVector)
			{
				for (auto itr2DVector = itr3DVector->begin(); itr2DVector != itr3DVector->end(); ++itr2DVector)
				{
					outFileRight.write((char*)itr2DVector->data(), sizeof((*itr2DVector)[0]) * itr2DVector->size());
					outFileLeft.write((char*)itr2DVector->data(), sizeof((*itr2DVector)[0]) * itr2DVector->size());
					//outFileRight << " "  << *itr2DVector->begin();
					//outFileLeft << " " << *(itr2DVector->end() - 1);
				}
				//outFileRight << G4endl;
				//outFileLeft << G4endl;
			}
		}
	}
	outFileRight.close();
	outFileLeft.close();
}

void PANDASimRunAction::PushBetaKEHe8(const G4double& bke)
{
	myAccu->PushBetaKEHe8(bke);
}

void PANDASimRunAction::PushBetaKELi9(const G4double& bke)
{
	myAccu->PushBetaKELi9(bke);
}

void PANDASimRunAction::PushDecayTimeHe8(const G4double& dt)
{
	myAccu->PushDecayTimeHe8(dt);
}

void PANDASimRunAction::PushDecayTimeLi9(const G4double& dt)
{
	myAccu->PushDecayTimeLi9(dt);
}

void PANDASimRunAction::PushNeutronGenicTime(const std::vector<std::vector<G4double>>& t)
{
	myAccu->PushNeutronGenicTime(t);
}

void PANDASimRunAction::PushNeutronGenicTime(const G4double& t)
{
	myAccu->PushNeutronGenicTime(t);
}

void PANDASimRunAction::PushNeutronKE(const std::vector<std::vector<G4double>>& ke)
{
	myAccu->PushNeutronKE(ke);
}

void PANDASimRunAction::PushNeutronKE(const G4double& ke)
{
	myAccu->PushNeutronKE(ke);
}

void PANDASimRunAction::PushCapTimeH(const G4double& ct)
{
	myAccu->PushCapTimeH(ct);
}

void PANDASimRunAction::PushCapTimeGd(const G4double& ct)
{
	myAccu->PushCapTimeGd(ct);
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

void PANDASimRunAction::PushEdepDecay(const std::vector<std::vector<G4double>>& de)
{
	myAccu->PushEdepDecay(de);
}

void PANDASimRunAction::PushModuleCalPhDecay(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec)
{
	myAccu->PushModuleCalPhDecay(nCalPhVec);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
