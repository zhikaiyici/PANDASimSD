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
// $Id: exampleB1.cc 86065 2014-11-07 08:51:15Z gcosmo $
//
/// \file exampleB1.cc
/// \brief Main program of the B1 example

#include "PANDASimDetectorConstruction.hh"
//#include "PANDASimPhysicsList.hh"
#include "PANDASimActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4EventManager.hh"
#include "G4SteppingManager.hh"

#include "G4UImanager.hh"
#include "QGSP_BIC_AllHP.hh"
#include "QGSP_BIC_HP.hh"
#include "QGSP_BERT_HP.hh"
#include "G4OpticalPhysics.hh"
#include "QBBC.hh"
//#include "Shielding.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"
#include "G4ParticleHPManager.hh"

#include "UserDataInput.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char** argv)
{
	UserDataInput::ReadInputData();

	// Choose the Random engine
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	auto seed = time(NULL);
	G4Random::setTheSeed(seed);

	G4bool uiStatus = UserDataInput::GetStatusOfUI();

	// Construct the default run manager
	//
#ifdef G4MULTITHREADED
	G4MTRunManager* runManager = new G4MTRunManager;
	G4int threadNumber = UserDataInput::GetNumberOfThreads();
	if (uiStatus != false)
		threadNumber = 1;
	else
		threadNumber = UserDataInput::GetNumberOfThreads();
	runManager->SetNumberOfThreads(threadNumber);
#else
	G4RunManager* runManager = new G4RunManager;
#endif

	// Set mandatory initialization classes
	//
	// Detector construction
	runManager->SetUserInitialization(new PANDASimDetectorConstruction());

	//// Physics list
	//runManager->SetUserInitialization(new PANDASimPhysicsList());
	//G4cout << "PANDASimPhysicsList" << G4endl;

	// Physics list
	G4VModularPhysicsList* physicsList = new QGSP_BIC_AllHP;
	//G4VModularPhysicsList* physicsList = new QBBC;
	//G4VModularPhysicsList* physicsList = new QGSP_BIC_HP;
	//G4VModularPhysicsList* physicsList = new QGSP_BERT_HP;
	physicsList->SetVerboseLevel(0);
	if (UserDataInput::GetOpticalPhysicsStatus() == true)
	{
		G4OpticalParameters::Instance()->SetScintFiniteRiseTime(true);
		G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
		physicsList->RegisterPhysics(opticalPhysics);
		opticalPhysics->SetVerboseLevel(0);
	}
	//G4double cutsValue = 10. * um;
	//physicsList->SetDefaultCutValue(cutsValue);
	//physicsList->SetCutValue(0, "proton");
	runManager->SetUserInitialization(physicsList);

	// User action initialization
	runManager->SetUserInitialization(new PANDASimActionInitialization());

	// Replaced HP environmental variables with C++ calls
	G4ParticleHPManager::GetInstance()->SetSkipMissingIsotopes(true);
	G4ParticleHPManager::GetInstance()->SetDoNotAdjustFinalState(true);
	//G4ParticleHPManager::GetInstance()->SetUseOnlyPhotoEvaporation(true);
	//G4ParticleHPManager::GetInstance()->SetNeglectDoppler(false);
	//G4ParticleHPManager::GetInstance()->SetProduceFissionFragments(false);
	//G4ParticleHPManager::GetInstance()->SetUseWendtFissionModel(false);
	//G4ParticleHPManager::GetInstance()->SetUseNRESP71Model(false);
	// 
	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	if (uiStatus != false)
	{
		// Initialize visualization
		//
		G4VisManager* visManager = new G4VisExecutive;
		// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
		// G4VisManager* visManager = new G4VisExecutive("Quiet");
		visManager->Initialize();

		// Detect interactive mode (if no arguments) and define UI session
		//
		G4UIExecutive* ui = new G4UIExecutive(argc, argv);

		UImanager->ApplyCommand("/run/verbose 2");
		UImanager->ApplyCommand("/event/verbose 0");
		UImanager->ApplyCommand("/tracking/verbose 0");
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();

		delete ui;

		// Job termination
		// Free the store: user actions, physics_list and detector_description are
		// owned and deleted by the run manager, so they should not be deleted 
		// in the main() program !
		delete visManager;
		delete runManager;
	}
	else
	{
		G4int NumberOfEvents = UserDataInput::GetNumberOfEvents();
		if (argc > 1)
		{
			// batch mode
			G4String command = "/control/execute ";
			G4String fileName = argv[1];
			UImanager->ApplyCommand(command + fileName);
			//runManager->BeamOn(NumberOfEvents);
		}
		else
		{
			runManager->SetVerboseLevel(2);
			UImanager->ApplyCommand("/event/verbose 0");
			UImanager->ApplyCommand("/tracking/verbose 0");

			// initialize G4 kernel
			runManager->Initialize();
			
			//auto neutrinoPosition = UserDataInput::GetPositionOfNeutrino();
			//if (neutrinoPosition[1] < 5)
			//{
			//	for (G4int j = neutrinoPosition[1]; j < 5; j++)
			//	{
			//		UserDataInput::SetPositionOfNeutrino({ neutrinoPosition[0],j });
			//		runManager->BeamOn(NumberOfEvents);
			//	}
			//}
			//else
				runManager->BeamOn(NumberOfEvents);
		}
		delete runManager;
		G4cout << G4endl << "Press Enter to exit." << G4endl;
		getchar();
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
