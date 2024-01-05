//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: PrimaryGeneratorMessenger.cc,v 1.8 2002/12/16 16:37:27 maire Exp $
// GEANT4 tag $Name: geant4-07-00-patch-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PANDASimPrimaryGeneratorMessenger.hh"

#include "PANDASimPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "PANDASimDetectorConstruction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimPrimaryGeneratorMessenger::PANDASimPrimaryGeneratorMessenger(PANDASimPrimaryGeneratorAction* gun)
	: G4UImessenger(), generaAction(gun)
{
	//G4LogicalVolumeStore* logicVolStroe = G4LogicalVolumeStore::GetInstance();
	//auto containerLV = logicVolStroe->GetVolume("ContainerLV");
	//G4Box* containerBox = dynamic_cast<G4Box*>(containerLV->GetSolid());
	//G4double containerYHalfLength = containerBox->GetYHalfLength();
	//
	//auto moduleLV = logicVolStroe->GetVolume("ModuleLV");
	//G4Box* moduleBox = dynamic_cast<G4Box*>(moduleLV->GetSolid());
	//G4double moduleYHalfLength = moduleBox->GetYHalfLength();
	//
	//G4int arraySize = (G4int)std::round(containerYHalfLength / moduleYHalfLength);
	G4int arraySize = (static_cast<const PANDASimDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction()))->GetArrySize();

	sourceDir = new G4UIdirectory("/source/");
	sourceDir->SetGuidance("Set source type.");

	sourceTypeCMD = new G4UIcmdWithAString("/source/type", this);
	sourceTypeCMD->SetGuidance("Available source type: ");
	sourceTypeCMD->SetGuidance("Cs137/Co60/Na22/Cs137g/Co60g/Am-Be-n with position CENTER/EDGE ");
	sourceTypeCMD->SetGuidance("or He8; Li9; NEUTRINO; MUON; COSMICNEUTRON; CRY (Linux only).");
	sourceTypeCMD->SetParameterName("sourceType", true);
	sourceTypeCMD->SetDefaultValue("NEUTRINO");
	sourceTypeCMD->SetCandidates("Cs137 Co60 Na22 Am-Be Cs137g Co60g Am-Be-n He8 Li9 NEUTRINO MUON COSMICNEUTRON CRY");
	sourceTypeCMD->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);

	sourceSpectraCMD = new G4UIcmdWithAString("/source/spectra", this);
	sourceSpectraCMD->SetGuidance("Specify spectra file name.");
	sourceSpectraCMD->SetParameterName("spectraName", true);
	sourceSpectraCMD->SetDefaultValue("IBDPositron.spec IBDNeutron.spec");

	sourcePositionCMD = new G4UIcmdWithAString("/source/position", this);
	sourcePositionCMD->SetGuidance("Source position only for the following souce type: ");
	sourcePositionCMD->SetGuidance("Cs137/Co60/Na22/Am-Be/Cs137g/Co60g/Am-Be-n with position CENTER/EDGE ");
	sourcePositionCMD->SetParameterName("sourcePosition", true);
	sourcePositionCMD->SetDefaultValue("CENTER");
	sourcePositionCMD->SetCandidates("CENTER EDGE");
	sourcePositionCMD->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);

	G4UIparameter* parameter;

	neutrinoPositionCMD = new G4UIcommand("/source/neutrinoPosition", this);
	neutrinoPositionCMD->SetGuidance("Two integer: [0 - size*size, 0 - 5]. ");
	neutrinoPositionCMD->SetGuidance("First integer: Position in the detector array, size*size means random.");
	neutrinoPositionCMD->SetGuidance("Second integer: Position in the detector module, 5 means random.");
	neutrinoPositionCMD->SetGuidance("Second integer is valid only when arrayPosition < size*size");
	parameter = new G4UIparameter("arrayPosition", 'i', true);
	parameter->SetDefaultValue(arraySize * arraySize/*16*/);
	parameter->SetParameterRange("arrayPosition >= 0");
	neutrinoPositionCMD->SetParameter(parameter);
	parameter = new G4UIparameter("detectorPosition", 'i', true);
	parameter->SetDefaultValue(5);
	parameter->SetParameterRange("detectorPosition >= 0");
	neutrinoPositionCMD->SetParameter(parameter);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimPrimaryGeneratorMessenger::~PANDASimPrimaryGeneratorMessenger()
{
	delete sourceDir;
	delete sourceTypeCMD;
	delete sourceSpectraCMD;
	delete sourcePositionCMD;
	delete neutrinoPositionCMD;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == sourceTypeCMD)
	{
		generaAction->SetSourceType(newValue);
	}
	if (command == sourcePositionCMD)
	{
		generaAction->SetSourcePosition(newValue);
	}
	if (command == neutrinoPositionCMD)
	{
		G4int ap, dp;
		std::istringstream is(newValue);
		is >> ap >> dp;
		generaAction->SetNeutrinoPosition({ ap, dp });
	}
	if (command == sourceSpectraCMD)
	{
		std::istringstream is(newValue);
		G4String name = "";
		std::vector<std::vector<G4double>> energys(0);
		std::vector<std::vector<G4double>> cdfSpectrums(0);
		while (is >> name)
		{
			std::vector<G4double> energy(0);
			std::vector<G4double> cdfSpectrum(0);
			UserDataInput::ReadSpectra(name, energy, cdfSpectrum);
			energys.push_back(energy);
			cdfSpectrums.push_back(cdfSpectrum);
		}
		generaAction->SetNeutronEnergy(energys[0]);
		generaAction->SetNeutronCDF(cdfSpectrums[0]);
		if (energys.size() > 1)
		{
			generaAction->SetPositronEnergy(energys[1]);
			generaAction->SetPositronCDF(cdfSpectrums[1]);
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......