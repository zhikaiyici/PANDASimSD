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
// $Id: PANDASimDetectorConstruction.hh 69565 2013-05-08 12:35:31Z gcosmo $
//
/// \file PANDASimDetectorConstruction.hh
/// \brief Definition of the PANDASimDetectorConstruction class

#ifndef PANDASimDetectorConstruction_h
#define PANDASimDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "G4Material.hh"

#include "UserDataInput.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

/// Detector construction class to define materials and geometry.

class PANDASimDetectorConstruction : public G4VUserDetectorConstruction
{
public:
	PANDASimDetectorConstruction();
	virtual ~PANDASimDetectorConstruction();

	virtual G4VPhysicalVolume* Construct();
	virtual void ConstructSDandField();

	inline G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }
	inline G4LogicalVolume* GetPhotoelectricScoringVolume() const { return fPhotoelectricScoringVolume; }
	inline G4LogicalVolume* GetGdFilmScoringVolume() const { return fGdFilmScoringVolume; }

protected:
	G4LogicalVolume* fScoringVolume;
	G4LogicalVolume* fPhotoelectricScoringVolume;
	G4LogicalVolume* fGdFilmScoringVolume;

private:
	// Option to switch on/off checking of volumes overlaps
	//
	G4bool checkOverlaps;
	G4int arraySize;
	//G4LogicalVolume* logicPlasticScintillator;

	void DefineMaterials();
	G4VPhysicalVolume* DefineDetector();
	void DefineFilmLogicAndPhysVolume(G4double filmBoxHalfSize[3],
									  G4Material* filmMaterial,
									  const G4String& filmName, 
									  G4double antiBoxHalfSize[3],
									  G4LogicalVolume* logicMotherVolume,
									  G4LogicalVolume*& logicFilmVolume,
									  G4VPhysicalVolume*& physFilmVolume);

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
