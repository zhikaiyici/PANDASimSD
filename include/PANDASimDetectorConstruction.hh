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
#include "G4GenericMessenger.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"

#include "UserDataInput.hh"
#include "PANDASimDetectorConstructionMessenger.hh"

#include <vector>

class G4VPhysicalVolume;
class G4PVPlacement;
class G4LogicalVolume;
class G4VSolid;
class G4OpticalSurface;
class G4LogicalSurface;
class G4MaterialPropertiesTable;
class G4GenericMessenger;

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

	inline void SetGdFilmThickness(G4double t) { gdFilmThickness = t; }
	inline G4double GetGdFilmThickness() const { return gdFilmThickness; }

	inline void SetDetectorXYZ(G4double x, G4double y, G4double z) { dtctrX = x; dtctrY = y; dtctrZ = z; }
	inline std::array<G4double, 3> GetDetectorXYZ() const { return { dtctrX, dtctrY, dtctrZ }; }

	inline std::array<G4double, 3> GetContainerXYZ() const { return { containerX, containerY, containerZ }; }

	inline void SetModuleDistance(G4double d) { distanceBetweenModules = d; }
	inline G4double GetModuleDistance() const { return distanceBetweenModules; }

	inline void SetArrySize(G4int as) { arraySize = as; }
	inline G4int GetArrySize() const { return arraySize; }

private:
	G4LogicalVolume* fScoringVolume;
	G4LogicalVolume* fPhotoelectricScoringVolume;
	G4LogicalVolume* fGdFilmScoringVolume;

	// Option to switch on/off checking of volumes overlaps
	//
	G4bool checkOverlaps;
	
	G4int arraySize;

	G4double mylarFilmThickness;// = 40. * um;
	G4double alFilmThickness;// = 30. * um;
	G4double heightPMT;// = 4. * cm;

	G4double dtctrX;// = userData.GetDectorDimensionX();
	G4double dtctrY;// = userData.GetDectorDimensionY();
	G4double dtctrZ;// = userData.GetDectorDimensionZ();

	G4double gdFilmThickness;// = userData.GetGdFilmThickness();
	G4double distanceBetweenModules;// = userData.GetDistanceBetweenModules();

	G4double scintillatorModuleZ;// = dtctrX + 2. * (gdFilmThickness + alFilmThickness + mylarFilmThickness * 2.);
	G4double scintillatorModuleY;// = dtctrY + 2. * (gdFilmThickness + alFilmThickness + mylarFilmThickness * 2.);
	G4double scintillatorModuleX;// = dtctrZ;

	G4double moduleZ;// = scintillatorModuleZ + distanceBetweenModules;
	G4double moduleY;// = scintillatorModuleY + distanceBetweenModules;
	G4double moduleX;// = scintillatorModuleX + 2. * heightPMT;

	G4double containerZ;// = arraySize * moduleZ;
	G4double containerY;// = arraySize * moduleY;
	G4double containerX;// = moduleX;

	G4double worldX;// = containerX + 10. * m;
	G4double worldY;// = containerY + 10. * m;
	G4double worldZ;// = 1.1 * containerZ;

	G4double roofXY;// = 10. * m;
	G4double roofZ;// = 0.25 * m;
	G4double roomHeight;// = 4. * m;

	std::vector<G4VisAttributes*> fVisAttributes;
	std::vector<G4RotationMatrix*> fRotationMatrix;
	std::vector<G4VSolid*> fSolidVolume;
	std::vector<G4LogicalVolume*> fLogicalVolume;
	std::vector<G4VPhysicalVolume*> fPhysicalVolume;
	std::vector<G4OpticalSurface*> fOpticalSurface;
	std::vector<G4LogicalSurface*> fLogicalSurface;
	std::vector<G4MaterialPropertiesTable*> fMPT;
	std::vector<G4VSensitiveDetector*> fSD;

	//PANDASimDetectorConstructionMessenger* fMessenger;
	G4GenericMessenger* fMessenger;

	G4Material* air;

	G4LogicalVolume* logicWorld;
	G4LogicalVolume* logicPlasticScintillator;
	G4LogicalVolume* logicGdFilm;
	G4LogicalVolume* logicPhotocathode;

	G4Box* solidWorld;

	G4VPhysicalVolume* physWorld;

	void DefineMaterials();
	void DefineDetector(G4LogicalVolume*);
	void DefineFilmLogicAndPhysVolume(const G4String& filmName, 
		                              G4double filmBoxHalfSize[3],
		                              G4double antiBoxHalfSize[3],
		                              G4Material* filmMaterial,
		                              G4LogicalVolume* logicMotherVolume,
									  G4LogicalVolume*& logicFilmVolume,
		                              G4PVPlacement*& physFilmVolume);
	void DefineCommands();
	//void UpdateGeometry();

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
