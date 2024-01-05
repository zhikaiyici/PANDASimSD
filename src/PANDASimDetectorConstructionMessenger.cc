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
// $Id: PANDASimDetectorConstruction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file PANDASimDetectorConstruction.cc
/// \brief Implementation of the PANDASimDetectorConstruction class

#include "PANDASimDetectorConstructionMessenger.hh"

PANDASimDetectorConstructionMessenger::PANDASimDetectorConstructionMessenger(PANDASimDetectorConstruction* pndCnstrctn)
	: G4UImessenger(), pandaDetectorConstruction(pndCnstrctn)
{
}

PANDASimDetectorConstructionMessenger::~PANDASimDetectorConstructionMessenger()
{
}

void PANDASimDetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	//// Define detector command directory using generic messenger class
	//fMessenger = new G4GenericMessenger(this, "/detector/",	"Detector control");

	//auto& gdFilmCMD = fMessenger->DeclarePropertyWithUnit("gdFilmThickness", "um", gdFilmThickness, "Set Gd film thickness.");
	//gdFilmCMD.SetParameterName("gdFilmThickness", true);
	//gdFilmCMD.SetDefaultUnit("um");
	//gdFilmCMD.SetDefaultValue("30.");
	//gdFilmCMD.SetRange("gdFilmThickness >= 0");

	//auto& armAngleCmd = fMessenger->DeclareMethodWithUnit("armAngle", "deg",
	//		&DetectorConstruction::SetArmAngle,
	//		"Set rotation angle of the second arm.");
	//armAngleCmd.SetParameterName("GdFilm", true);
	//armAngleCmd.SetRange("GdFilm >= 0.");
	//armAngleCmd.SetDefaultValue("30.");
}
