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
// $Id: exampleB1.cc 86065 2014-11-07 08:51:15Z gcosmo $
//
/// \file exampleB1.cc
/// \brief Main program of the B1 example

#include "PANDASimDetectorConstruction.hh"
#include "PANDASimPhysicsList.hh"
#include "PANDASimPrimaryGeneratorAction.hh"
#include "PANDASimActionInitialization.hh"
#include "PANDASimPhysicsListMessenger.hh"

//#ifdef G4MULTITHREADED
//#include "G4MTRunManager.hh"
//#else
//#include "G4RunManager.hh"
//#endif
#include "G4RunManagerFactory.hh"
#include "G4EventManager.hh"
#include "G4SteppingManager.hh"
#include "G4SteppingVerbose.hh"

#include "G4UImanager.hh"
#include "QGSP_BIC_AllHP.hh"
#include "QGSP_BIC_HP.hh"
#include "QGSP_BERT_HP.hh"
#include "G4OpticalPhysics.hh"
#include "G4MuonicAtomDecayPhysics.hh"
#include "QBBC.hh"
//#include "Shielding.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"
#include "G4ParticleHPManager.hh"
#include "G4HadronicParameters.hh"

#include "UserDataInput.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char** argv)
{
	//UserDataInput::ReadInputData();

	// Choose the Random engine
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	auto seed = time(NULL);
	G4Random::setTheSeed(seed);

	//use G4SteppingVerboseWithUnits
	G4int precision = 4;
	G4SteppingVerbose::UseBestUnit(precision);

	// 
    // Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	PANDASimPhysicsListMessenger* physMessenger = new PANDASimPhysicsListMessenger(); 
	//UImanager->ApplyCommand("/physics/optical true");
	if (argc > 2)
	{
		// set optical process status
		G4String command = "/control/execute ";
		G4String fileName = argv[2];
		UImanager->ApplyCommand(command + fileName);
	}

	// Construct the default run manager
	//
//#ifdef G4MULTITHREADED
//	G4MTRunManager* runManager = new G4MTRunManager;
//	G4int threadNumber = UserDataInput::GetNumberOfThreads();
//	if (uiStatus != false)
//		threadNumber = 1;
//	else
//		threadNumber = UserDataInput::GetNumberOfThreads();
//	runManager->SetNumberOfThreads(threadNumber);
//#else
//	G4RunManager* runManager = new G4RunManager;
//#endif
	G4RunManager* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);

	// Set mandatory initialization classes
	//
	// Detector construction
	runManager->SetUserInitialization(new PANDASimDetectorConstruction());

	//// Physics list
	//runManager->SetUserInitialization(new PANDASimPhysicsList());
	//G4cout << "PANDASimPhysicsList" << G4endl;

	// Physics list
	G4int phyVer = 1;
	G4VModularPhysicsList* physicsList = new QGSP_BIC_AllHP(phyVer);
	//G4VModularPhysicsList* physicsList = new QBBC(phyVer);
	//G4VModularPhysicsList* physicsList1 = new PANDASimPhysicsList(phyVer);
	//G4VModularPhysicsList* physicsList = new QGSP_BIC_HP(phyVer);
	//G4VModularPhysicsList* physicsList = new QGSP_BERT_HP(phyVer);
	if (physMessenger->GetMuonicDecay())
	{
		physicsList->RegisterPhysics(new G4MuonicAtomDecayPhysics(phyVer));
		if (phyVer > 0)
			G4cout << G4endl <<  "<<< Muonic Atom Decay Physics is registered." << G4endl << G4endl;
	}
	physicsList->SetVerboseLevel(phyVer);
	G4bool opticalStatus = physMessenger->GetOptical();//UserDataInput::GetOpticalPhysicsStatus()
	if (opticalStatus)
	{
		G4OpticalParameters::Instance()->SetScintFiniteRiseTime(true);
		G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics(phyVer);
		physicsList->RegisterPhysics(opticalPhysics);
		opticalPhysics->SetVerboseLevel(phyVer);
		if (phyVer > 0)
			G4cout << G4endl <<  "<<< Optical Physics is registered." << G4endl << G4endl;
	}
	//G4double cutsValue = 10. * um;
	//physicsList->SetDefaultCutValue(cutsValue);
	//physicsList->SetCutValue(0, "proton");
	runManager->SetUserInitialization(physicsList);

	//runManager->SetUserAction(new PANDASimPrimaryGeneratorAction(""));

	// User action initialization
	runManager->SetUserInitialization(new PANDASimActionInitialization());

	//The default value of the time threshold for radioactive decays of ions has been changed, from 10^27 ns to 1 year, sine 11.2
	//G4HadronicParameters::Instance()->SetTimeThresholdForRadioactiveDecay(1.0e+60 * CLHEP::year);

	// Replaced HP environmental variables with C++ calls
	G4ParticleHPManager::GetInstance()->SetSkipMissingIsotopes(true);
	G4ParticleHPManager::GetInstance()->SetDoNotAdjustFinalState(true);
	//G4ParticleHPManager::GetInstance()->SetUseOnlyPhotoEvaporation(true);
	//G4ParticleHPManager::GetInstance()->SetNeglectDoppler(false);
	//G4ParticleHPManager::GetInstance()->SetProduceFissionFragments(false);
	//G4ParticleHPManager::GetInstance()->SetUseWendtFissionModel(false);
	//G4ParticleHPManager::GetInstance()->SetUseNRESP71Model(false);

	if (argc > 1)
	{
		// batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UImanager->ApplyCommand(command + fileName);
	}
	//else if (true)
	//{
	//	runManager->Initialize();
	//	UImanager->ApplyCommand("/source/type Cs137g");
	//	G4String command = "/control/execute ";
	//	UImanager->ApplyCommand(command + "run.mac");

	//}
	else
	{
		runManager->SetNumberOfThreads(1);
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
		UImanager->ApplyCommand("/detector/addLabRoom true");
		//UImanager->ApplyCommand("/run/initialize");
		//UImanager->ApplyCommand("/source/type Cs137g");
		//UImanager->ApplyCommand("/control/execute vis.mac");
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();

		delete ui;
		delete visManager;
	}
	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	// owned and deleted by the run manager, so they should not be deleted 
	// in the main() program !
	delete physMessenger;
	delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
