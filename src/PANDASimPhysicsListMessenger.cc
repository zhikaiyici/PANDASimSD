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
/// \file electromagnetic/TestEm7/src/PhysicsList.cc
/// \brief Implementation of the PhysicsList class
//
// $Id$
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PANDASimPhysicsListMessenger.hh"

#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimPhysicsListMessenger::PANDASimPhysicsListMessenger() : G4UImessenger(), optical(false), muonicDecay(false)
{
	optiaclPhysicsCMD = new G4UIcmdWithABool("/physics/optical", this);
	optiaclPhysicsCMD->SetGuidance("Turn on/off optical process.");
	optiaclPhysicsCMD->SetParameterName("opticalProcess", true);
	optiaclPhysicsCMD->SetDefaultValue(true);

	muonicAtomDecayCMD = new G4UIcmdWithABool("/physics/muonicDecay", this);
	muonicAtomDecayCMD->SetGuidance("Turn on/off muonic atom decay process.");
	muonicAtomDecayCMD->SetParameterName("muonicDecay", true);
	muonicAtomDecayCMD->SetDefaultValue(true);
}

PANDASimPhysicsListMessenger::~PANDASimPhysicsListMessenger()
{
	delete optiaclPhysicsCMD;
	delete muonicAtomDecayCMD;
}

void PANDASimPhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == optiaclPhysicsCMD)
	{
		optical = optiaclPhysicsCMD->GetNewBoolValue(newValue);
	}
	if (command == muonicAtomDecayCMD)
	{
		muonicDecay = muonicAtomDecayCMD->GetNewBoolValue(newValue);
	}
}

