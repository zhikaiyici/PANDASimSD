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

#include "PANDASimDetectorConstruction.hh"
#include "PANDASimScinitillatorSD.hh"
#include "PANDASimGdFilmSD.hh"
#include "PANDASimPhotocathodeSD.hh"

#include "G4SDManager.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"
#include "G4BooleanSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Trd.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4VisAttributes.hh"

#include "CalculateReferencePoints.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimDetectorConstruction::PANDASimDetectorConstruction()
	: G4VUserDetectorConstruction(),
	fScoringVolume(nullptr), fPhotoelectricScoringVolume(nullptr),fGdFilmScoringVolume(nullptr),
	checkOverlaps(true)
{
	arraySize = UserDataInput::GetSizeOfArray();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimDetectorConstruction::~PANDASimDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* PANDASimDetectorConstruction::Construct()
{
	DefineMaterials();
	return DefineDetector();
}

void PANDASimDetectorConstruction::DefineMaterials()
{
	// Al,Air material defined using NIST Manager
	auto nistManager = G4NistManager::Instance();
	G4Material* aluminium = nistManager->FindOrBuildMaterial("G4_Al");
	G4Material* air = nistManager->FindOrBuildMaterial("G4_AIR");
	G4Material* mylarFilm = nistManager->FindOrBuildMaterial("G4_MYLAR");
	G4Material* glass = nistManager->FindOrBuildMaterial("G4_Pyrex_Glass");
	G4Material* stainlessSteel = nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
	G4Material* berylliumForAmBe = nistManager->FindOrBuildMaterial("G4_Be");

	G4Element* elH = nistManager->FindOrBuildElement(1);
	G4Element* elB = nistManager->FindOrBuildElement(5);
	G4Element* elC = nistManager->FindOrBuildElement(6);
	G4Element* elO = nistManager->FindOrBuildElement(8);
	G4Element* elNa = nistManager->FindOrBuildElement(11);
	G4Element* elAl = nistManager->FindOrBuildElement(13);
	G4Element* elSi = nistManager->FindOrBuildElement(14);
	G4Element* elK = nistManager->FindOrBuildElement(19);
	G4Element* elSb = nistManager->FindOrBuildElement(51);
	G4Element* elCs = nistManager->FindOrBuildElement(55);
	G4Element* elGd = nistManager->FindOrBuildElement(64);

	// Plastic Scintillator material
	G4Material* plasticScintillator = new G4Material("PlasticScintillator", 1.023 * g / cm3, 2);
	plasticScintillator->AddElement(elC, 517);
	plasticScintillator->AddElement(elH, 469);

	// Gd2O3 material
	G4Material* gadoliniumOxide = new G4Material("GadoliniumOxide", 7.407 * g / cm3, 2);
	gadoliniumOxide->AddElement(elGd, 2);
	gadoliniumOxide->AddElement(elO, 3);

	// bialkali photon cathode. unknown
	G4Material* bialkaliPhotocathode = new G4Material("Bialkali", 3.149 * g / cm3, 3);
	bialkaliPhotocathode->AddElement(elK, 2);
	bialkaliPhotocathode->AddElement(elSb, 1);
	bialkaliPhotocathode->AddElement(elCs, 1);

	/* glass material
	G4Material* glass = new G4Material("Glass", 1.032 * g / cm3, 2);
	glass->AddElement(elC, 91.533 * perCent);
	glass->AddElement(elH, 8.467 * perCent);*/

	/*// PET聚酯薄膜
	G4Material* PET = new G4Material("PET", 0.95 * g / cm3, 3);
	PET->AddElement(elC, 5);
	PET->AddElement(elH, 4);
	PET->AddElement(elO, 2);*/

	// Vacuum
	G4double density = 1e-3 * kGasThreshold;         //from PhysicalConstants.h
	G4double temperature = STP_Temperature;         //from PhysicalConstants.h
	G4double pressure = STP_Pressure * density / (1.29e-3 * g / cm3);
	G4Material* vacuum = new G4Material("Vacuum", density, 1, kStateGas, temperature, pressure);
	vacuum->AddMaterial(air, 1.);

	//
	// ------------ Generate & Add Material Properties Table ------------
	//
	G4double photonEnergy[] =
	{ 2.4800 * eV, 2.4879 * eV, 2.5036 * eV, 2.5195 * eV, 2.5357 * eV, 2.5504 * eV,
	  2.5619 * eV, 2.5769 * eV, 2.5904 * eV, 2.6023 * eV, 2.6126 * eV, 2.6230 * eV,
	  2.6335 * eV, 2.6405 * eV, 2.6511 * eV, 2.6618 * eV, 2.6744 * eV, 2.6907 * eV,
	  2.7018 * eV, 2.7185 * eV, 2.7297 * eV, 2.7429 * eV, 2.7544 * eV, 2.7640 * eV,
	  2.7756 * eV, 2.7834 * eV, 2.7952 * eV, 2.8071 * eV, 2.8171 * eV, 2.8271 * eV, 
	  2.8375 * eV, 2.8475 * eV, 2.8578 * eV, 2.8702 * eV, 2.8848 * eV, 2.8975 * eV,
	  2.9176 * eV, 2.9383 * eV, 2.9471 * eV, 2.9522 * eV, 2.9581 * eV, 2.9625 * eV,
	  2.9692 * eV, 2.9736 * eV, 2.9804 * eV, 2.9848 * eV, 2.9903 * eV, 2.9961 * eV, 
	  3.0030 * eV, 3.0121 * eV, 3.0199 * eV, 3.0276 * eV, 3.0375 * eV, 3.0469 * eV,
	  3.0572 * eV, 3.0692 * eV, 3.0897 * eV, 3.1115 * eV };
	const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double);

	G4double scintillatorRefractiveIndex[nEntries] = {};
	fill(scintillatorRefractiveIndex, scintillatorRefractiveIndex + nEntries, 1.58);
	G4double scintillatorAbsorption[nEntries] = {};
	fill(scintillatorAbsorption, scintillatorAbsorption + nEntries, 380. * cm);

	G4double scintilFast[] =
	{ 0.0595, 0.0615, 0.0724, 0.0844, 0.1053, 0.1218, 0.1382, 0.1547, 0.1821, 0.2095, 0.2369,
	  0.2644, 0.2863, 0.3092, 0.3411, 0.3795, 0.4014, 0.4179, 0.4253, 0.4618, 0.4892, 0.5221,
	  0.5495, 0.5769, 0.6098, 0.6357, 0.6756, 0.7085, 0.7469, 0.7908, 0.8252, 0.8557, 0.8904,
	  0.9244, 0.9568, 0.9772, 1.0000, 0.9608, 0.9114, 0.8566, 0.8017, 0.7469, 0.6921, 0.6372,
	  0.5824, 0.5276, 0.4727, 0.4179, 0.3631, 0.3082, 0.2589, 0.2095, 0.1657, 0.1163, 0.0734,
	  0.0455, 0.0176, 0.0011};

	assert(sizeof(scintilFast) == sizeof(photonEnergy));

	//G4double scintilSlow[] =
	//{ 0.01, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00,
	//  7.00, 8.00, 9.00, 8.00, 7.00, 6.00, 4.00,
	//  3.00, 2.00, 1.00, 0.01, 1.00, 2.00, 3.00,
	//  4.00, 5.00, 6.00, 7.00, 8.00, 9.00, 8.00,
	//  7.00, 6.00, 5.00, 4.00 };

	G4MaterialPropertiesTable* scintillatorMPT = new G4MaterialPropertiesTable();

	scintillatorMPT->AddProperty("RINDEX", photonEnergy, scintillatorRefractiveIndex, nEntries, false, true);// ->SetSpline(true);
	scintillatorMPT->AddProperty("ABSLENGTH", photonEnergy, scintillatorAbsorption, nEntries, false, true);// ->SetSpline(true);
	//scintillatorMPT->AddProperty("FASTCOMPONENT", photonEnergy, scintilFast, nEntries, false, true);// ->SetSpline(true);
	scintillatorMPT->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scintilFast, nEntries, false, true);// ->SetSpline(true);


	scintillatorMPT->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV);
	scintillatorMPT->AddConstProperty("RESOLUTIONSCALE", 0./*1.0*/);
	//scintillatorMPT->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.9 * ns);
	scintillatorMPT->AddConstProperty("SCINTILLATIONRISETIME1", 0.9 * ns);
	scintillatorMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns);
	//scintillatorMPT->AddConstProperty("FASTTIMECONSTANT", 2.1 * ns);
	//scintillatorMPT->AddConstProperty("YIELDRATIO", MAXFLOAT);
	scintillatorMPT->AddConstProperty("SCINTILLATIONYIELD1", 1.0/*0.8*/);

	G4cout << "Plastic scintillator G4MaterialPropertiesTable" << G4endl;
	scintillatorMPT->DumpTable();

	plasticScintillator->SetMaterialPropertiesTable(scintillatorMPT);

	// Air
	//
	//G4double airRefractiveIndex[] =
	//{ 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
	//  1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
	//  1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
	//  1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
	//  1.00, 1.00, 1.00, 1.00 };

	G4double airRefractiveIndex[nEntries] = {};
	fill(airRefractiveIndex, airRefractiveIndex + nEntries, 1.0);

	G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
	airMPT->AddProperty("RINDEX", photonEnergy, airRefractiveIndex, nEntries);

	G4cout << "Air/Vacuum G4MaterialPropertiesTable" << G4endl;
	airMPT->DumpTable();

	air->SetMaterialPropertiesTable(airMPT);
	vacuum->SetMaterialPropertiesTable(airMPT);

	// glass
	G4double glassRefractiveIndex[nEntries] = {};
	fill(glassRefractiveIndex, glassRefractiveIndex + nEntries, 1.53);
	G4double glassAbsorption[nEntries] = {};
	fill(glassAbsorption, glassAbsorption + nEntries, 420. * cm);

	G4MaterialPropertiesTable* glassMPT = new G4MaterialPropertiesTable();
	glassMPT->AddProperty("RINDEX", photonEnergy, glassRefractiveIndex, nEntries);
	glassMPT->AddProperty("ABSLENGTH", photonEnergy, glassAbsorption, nEntries);
	glass->SetMaterialPropertiesTable(glassMPT);

	G4cout << "Glass G4MaterialPropertiesTable" << G4endl;
	glassMPT->DumpTable();

	// Print materials
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

G4VPhysicalVolume* PANDASimDetectorConstruction::DefineDetector()
{
	UserDataInput userData;
	// Geometry parameters
	G4double dtctrX = userData.GetDectorDimensionX();
	G4double dtctrY = userData.GetDectorDimensionY();
	G4double dtctrZ = userData.GetDectorDimensionZ();
	G4double gdFilmThickness = userData.GetGdFilmThickness();
	G4double distanceBetweenModules = userData.GetDistanceBetweenModules();
	G4double mylarFilmThickness = 40. * um;
	G4double alFilmThickness = 30. * um;
	G4double outerRadiusPMT = 5.1 / 2. * cm;
	G4double innerRadiusPMT = 4.6 / 2. * cm;
	G4double glassThicknessPMT = outerRadiusPMT - innerRadiusPMT;
	G4double heightPMT = 4. * cm;
	G4double radiusPhotocathode = innerRadiusPMT;
	G4double heightPhotocathode = 26. * nm;

	G4double scintillatorModuleZ = dtctrX + 2. * (gdFilmThickness + alFilmThickness + mylarFilmThickness * 2.);
	G4double scintillatorModuleY = dtctrY + 2. * (gdFilmThickness + alFilmThickness + mylarFilmThickness * 2.);
	G4double scintillatorModuleX = dtctrZ;
	G4double moduleZ = scintillatorModuleZ + distanceBetweenModules;
	G4double moduleY = scintillatorModuleY + distanceBetweenModules;
	G4double moduleX = scintillatorModuleX + 2. * heightPMT;

	G4double containerZ = arraySize * moduleZ;
	G4double containerY = arraySize * moduleY;
	G4double containerX = moduleX;

	G4double worldX = 1.1 * containerX + 1. * mm;
	G4double worldY = 1.1 * containerY + 1. * mm;
	G4double worldZ = 1.1 * containerZ + 1. * mm;

	// Get materials
	G4Material* plasticScintillator = G4Material::GetMaterial("PlasticScintillator");
	G4Material* air = G4Material::GetMaterial("G4_AIR");
	G4Material* gadoliniumOxide = G4Material::GetMaterial("GadoliniumOxide");
	G4Material* aluminium = G4Material::GetMaterial("G4_Al");
	G4Material* mylarFilm = G4Material::GetMaterial("G4_MYLAR");
	G4Material* glass = G4Material::GetMaterial("G4_Pyrex_Glass");
	G4Material* vacuum = G4Material::GetMaterial("Vacuum");
	G4Material* bialkaliPhotocathode = G4Material::GetMaterial("Bialkali");
	G4Material* stainlessSteel = G4Material::GetMaterial("G4_STAINLESS-STEEL");
	G4Material* berylliumForAmBe = G4Material::GetMaterial("G4_Be");

	if (!plasticScintillator || !air || !gadoliniumOxide || !mylarFilm || !aluminium 
		|| !glass || !vacuum || !bialkaliPhotocathode || !stainlessSteel || !berylliumForAmBe)
	{
		G4ExceptionDescription msg;
		msg << "Cannot retrieve materials already defined.";
		G4Exception("PANDASimDetectorConstruction::DefineDetector()", "Get materials", FatalException, msg);
	}

	// World
	//
	G4Box* solidWorld =	new G4Box("WorldSV", 0.5 * worldX, 0.5 * worldY, 0.5 * worldZ);
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, air, "WorldLV");
	G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "WorldPV", 0, false, 0, checkOverlaps);
	
	// detector array
	//
	G4Box* solidContainer = new G4Box("ContaineSV", containerX / 2., containerY / 2., containerZ / 2.);
	G4LogicalVolume* logicContainer = new G4LogicalVolume(solidContainer, air, "ContainerLV");
	G4RotationMatrix* rotC = new G4RotationMatrix();
	rotC->rotateX(0/*pi / 2.*/);
	new G4PVPlacement(rotC, G4ThreeVector(), logicContainer, "ContainerPV", logicWorld, false, 0, checkOverlaps);
	
	G4Box* solidModuleRow =	new G4Box("ModuleRowSV", moduleX / 2., moduleY / 2., arraySize * moduleZ / 2.);
	G4LogicalVolume* logicModuleRow = new G4LogicalVolume(solidModuleRow, air, "ModuleRowLV");
	new G4PVReplica("ModuleRowPV", logicModuleRow, logicContainer, kYAxis, arraySize, moduleY);
	
	G4Box* solidModule = new G4Box("ModuleSV", moduleX / 2., moduleY / 2., moduleZ / 2.);
	G4LogicalVolume* logicModule = new G4LogicalVolume(solidModule, air, "ModuleLV");
	G4VPhysicalVolume* physModule =	new G4PVReplica("ModulePV", logicModule, logicModuleRow, kZAxis, arraySize, moduleZ);

	G4double scintillatorHalfSize[3] = { dtctrZ / 2., dtctrY / 2., dtctrX / 2. };
	G4double alFilmHalfSize[3] = 
		{ scintillatorHalfSize[0], scintillatorHalfSize[1] + alFilmThickness, scintillatorHalfSize[2] + alFilmThickness };
	G4double alMylarFilmHalfSize[3] = 
		{ alFilmHalfSize[0], alFilmHalfSize[1] + mylarFilmThickness, alFilmHalfSize[2] + mylarFilmThickness };
	G4double gdFilmHalfSize[3] = 
		{ alMylarFilmHalfSize[0], alMylarFilmHalfSize[1] + gdFilmThickness, alMylarFilmHalfSize[2] + gdFilmThickness };
	G4double gdMylarFilmHalfSize[3] =
		{ gdFilmHalfSize[0], gdFilmHalfSize[1] + mylarFilmThickness, gdFilmHalfSize[2] + mylarFilmThickness };

	G4LogicalVolume* logicAlFilm;
	G4VPhysicalVolume* physAlFilm;
	DefineFilmLogicAndPhysVolume(alFilmHalfSize,   // size
								 aluminium,    // material
								 "AlFilm",     // name
								 scintillatorHalfSize,  // antiBoxSize
								 logicModule,  // mother logical volume
								 logicAlFilm,  // logical
								 physAlFilm);  // physical

	G4LogicalVolume* logicAlMylarFilm;
	G4VPhysicalVolume* physAlMylarFilm;
	DefineFilmLogicAndPhysVolume(alMylarFilmHalfSize,   // size
								 mylarFilm,    // material
								 "AlMylarFilm",     // name
								 alFilmHalfSize,  // antiBoxSize
								 logicModule,  // mother logical volume
								 logicAlMylarFilm,  // logical
								 physAlMylarFilm);  // physical

	G4LogicalVolume* logicGdFilm;
	G4VPhysicalVolume* physGdFilm;
	DefineFilmLogicAndPhysVolume(gdFilmHalfSize,
								 gadoliniumOxide,
								 "GdFilm",
								 alMylarFilmHalfSize,
								 logicModule,
								 logicGdFilm,
								 physGdFilm);
	
	fGdFilmScoringVolume = logicGdFilm;

	G4LogicalVolume* logicGdMylarFilm;
	G4VPhysicalVolume* physGdMylarFilm;
	DefineFilmLogicAndPhysVolume(gdMylarFilmHalfSize,
								 mylarFilm,
								 "GdMylarFilm",
								 gdFilmHalfSize,
								 logicModule,
								 logicGdMylarFilm,
								 physGdMylarFilm);

	G4Box* solidPlasticScintillator = new G4Box("PlasticScintillatorSV", dtctrZ / 2., dtctrY / 2., dtctrX / 2.);
	G4LogicalVolume* logicPlasticScintillator = 
		new G4LogicalVolume(solidPlasticScintillator, plasticScintillator, "PlasticScintillatorLV");
	G4VPhysicalVolume* physPlasticScinitillator = 
		new G4PVPlacement(0, G4ThreeVector(), logicPlasticScintillator, "PlasticScintillatorPV", logicModule, false, 0, checkOverlaps);

	fScoringVolume = logicPlasticScintillator;

	G4Tubs* solidPMT = new G4Tubs("PMTSV", 0., outerRadiusPMT, heightPMT / 2., 0., twopi);
	//G4BooleanSolid* solidTubePMT = 
	//	new G4SubtractionSolid("TubePMT", solidPMT, solidChamberPMT, 0, G4ThreeVector(0, 0, -wallThicknessPMT));
	//G4Box* solidPMTBox = new G4Box("PMTBox", dtctrX / 2., dtctrY / 2., heightPMT / 2.);
	G4LogicalVolume* logicPMT = new G4LogicalVolume(solidPMT, glass, "PMTLV");

	G4Tubs* solidChamberPMT = new G4Tubs("ChamberPMTSV", 0., innerRadiusPMT, heightPMT / 2. - glassThicknessPMT, 0., twopi);
	G4LogicalVolume* logicChamberPMT = new G4LogicalVolume(solidChamberPMT, vacuum, "ChamberPMTLV");
	new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicChamberPMT, "ChamberPMTPV", logicPMT, false, 0, checkOverlaps);

	G4Tubs* solidPhotocathode = new G4Tubs("PhotocathodeSV", 0., radiusPhotocathode, heightPhotocathode / 2., 0., twopi);
	//G4Box* solidPhotocathodeBox = new G4Box("PhotocathodeBox", dtctrX / 2., dtctrY / 2., heightPhotocathode / 2.);
	G4LogicalVolume* logicPhotocathode = new G4LogicalVolume(solidPhotocathode, bialkaliPhotocathode, "PhotocathodeLV");
	new G4PVPlacement(0, G4ThreeVector(0, 0, -heightPhotocathode / 2. + solidChamberPMT->GetZHalfLength()),
		logicPhotocathode, "PhotocathodePV", logicChamberPMT, false, 0, checkOverlaps);

	fPhotoelectricScoringVolume = logicPhotocathode;

	G4RotationMatrix* rotPMTLeft = new G4RotationMatrix();
	rotPMTLeft->rotateY(-pi / 2.);
	G4VPhysicalVolume* physPMTLeft =
		new G4PVPlacement(rotPMTLeft, G4ThreeVector(-(scintillatorModuleX + heightPMT) / 2., 0., 0.),
			logicPMT, "PMTLeftPV", logicModule, false, 1, checkOverlaps);
	G4RotationMatrix* rotPMTRight = new G4RotationMatrix();
	rotPMTRight->rotateY(pi / 2.);
	G4VPhysicalVolume* physPMTRight =
		new G4PVPlacement(rotPMTRight, G4ThreeVector((scintillatorModuleX + heightPMT) / 2., 0., 0.),
						  logicPMT, "PMTRightPV", logicModule, false, 0, checkOverlaps);

	G4double hatHalfThickness = distanceBetweenModules / 2.;
	G4Box* solidHatBox = new G4Box("HatBoxSV", hatHalfThickness, moduleY / 2., moduleZ / 2.);
	G4Box* solidAntiHatBox = 
		new G4Box("AntiHatBoxSV", hatHalfThickness, scintillatorModuleY / 2., scintillatorModuleZ / 2.);
	G4Tubs* solidAntiHatCylinder = new G4Tubs("AntiHatCylinderSV", 0, outerRadiusPMT, 2. * hatHalfThickness, 0, twopi);
	G4BooleanSolid* solidHatTemp = 
		new G4SubtractionSolid("HatTempSV", solidHatBox, solidAntiHatBox, 0, G4ThreeVector(hatHalfThickness, 0, 0));
	G4BooleanSolid* solidHat =
		new G4SubtractionSolid("HatSV", solidHatTemp, solidAntiHatCylinder, rotPMTRight, G4ThreeVector());
	G4LogicalVolume* logicHat = new G4LogicalVolume(solidHat, aluminium, /*stainlessSteel,*/ "HatLV");
	G4RotationMatrix* rotHat = new G4RotationMatrix;
	rotHat->rotateZ(pi);
	G4VPhysicalVolume* physHatRight =
		new G4PVPlacement(rotHat, G4ThreeVector(scintillatorModuleX / 2., 0, 0), 
						  logicHat, "HatRightPV", logicModule, false, 0, checkOverlaps);
	G4VPhysicalVolume* physHatLeft =
		new G4PVPlacement(0, G4ThreeVector(-scintillatorModuleX / 2., 0, 0),
						  logicHat, "HatLeftPV", logicModule, false, 1, checkOverlaps);

	//G4_Be orb for Am-Be neutron source
	if (UserDataInput::GetSoureType() == "Am-Be")
	{
		G4Orb* solidBeOrb = new G4Orb("BeOrbSV", 0.5 * mm);
		G4LogicalVolume* logicBeOrb = new G4LogicalVolume(solidBeOrb, berylliumForAmBe, "BeOrbLV");
		auto sourcePosition = UserDataInput::GetSourePosition();
		G4ThreeVector positionVector;
		if (sourcePosition == "EDGE")
		{
			positionVector = G4ThreeVector(scintillatorHalfSize[0] - distanceBetweenModules, 0., 0.5 * mm + containerZ / 2.);
		}
		else
		{
			positionVector = G4ThreeVector(0., 0., 0.5 * mm + containerZ / 2.);
		}
		new G4PVPlacement(0, positionVector, logicBeOrb, "BeOrbPV", logicWorld, false, 0, checkOverlaps);
	}

	//
	//----------------Define optical surfaces----------------
	//
	// sci-reflector
	G4OpticalSurface* opScinRelfleSurface = new G4OpticalSurface("ScinMetalOpticalSurface");
	opScinRelfleSurface->SetType(dielectric_metal);
	opScinRelfleSurface->SetFinish(polished);
	opScinRelfleSurface->SetModel(glisur);

	G4LogicalBorderSurface* scinRefleSurface =
		new G4LogicalBorderSurface("ScinRefleSurface", physPlasticScinitillator, physAlFilm, opScinRelfleSurface);
	G4LogicalBorderSurface* scinHatRightSurface =
		new G4LogicalBorderSurface("ScinHatRightSurface", physPlasticScinitillator, physHatRight, opScinRelfleSurface);
	G4LogicalBorderSurface* scinHatLeftSurface =
		new G4LogicalBorderSurface("ScinHatLeftSurface", physPlasticScinitillator, physHatLeft, opScinRelfleSurface);
	
	// sci-PMT glss
	G4OpticalSurface* opScinPMTSurface = new G4OpticalSurface("ScinPMTOpticalSurface");
	opScinPMTSurface->SetType(dielectric_dielectric);
	opScinPMTSurface->SetFinish(polished);
	opScinPMTSurface->SetModel(unified);

	//G4double photonEnergy[] = { 2 * eV, 3.5 * eV };
	//const G4int num = sizeof(photonEnergy) / sizeof(G4double);
	//G4double reflectivity[num] = { 1.0, 1.0 };
	//assert(sizeof(photonEnergy) == sizeof(reflectivity));
	//G4double transimission[num] = { 0.0, 0.0 };
	//assert(sizeof(photonEnergy) == sizeof(transimission));
	//G4double efficiency[num] = { 0.8, 1.0 };
	//G4MaterialPropertiesTable* opPMTSurfaceMPT = new G4MaterialPropertiesTable();
	//opPMTSurfaceMPT->AddProperty("REFLECTIVITY", photonEnergy, reflectivity, num);
	//opPMTSurfaceMPT->AddProperty("TRANSMISSION", photonEnergy, transimission, num);
	//G4cout << "PMT Surface G4MaterialPropertiesTable" << G4endl;
	//opPMTSurfaceMPT->DumpTable();
	//opPMTSurface->SetMaterialPropertiesTable(opPMTSurfaceMPT);

	G4LogicalBorderSurface* PMTRightSurface =
		new G4LogicalBorderSurface("ScinPMTRightSurface", physPlasticScinitillator, physPMTRight, opScinPMTSurface);
	G4LogicalBorderSurface* PMTLeftSurface =
		new G4LogicalBorderSurface("ScinPMTLeftSurface", physPlasticScinitillator, physPMTLeft, opScinPMTSurface);

	// Photocathode surface properties
	G4double photonEnergy[] = 
	{ 1.8338 * eV, 1.8419 * eV, 1.8527 * eV, 1.8721 * eV, 1.8890 * eV, 1.9172 * eV, 1.9530 * eV, 1.9800 * eV,
	  2.0022 * eV, 2.0413 * eV, 2.0845 * eV, 2.1479 * eV, 2.2163 * eV, 2.2922 * eV, 2.4194 * eV, 2.5563 * eV,
	  2.7037 * eV, 2.8891 * eV, 3.0268 * eV, 3.1703 * eV, 3.3728 * eV, 3.6556 * eV, 3.9353 * eV, 4.0806 * eV,
	  4.2007 * eV, 4.2506 * eV };
	const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double);

	G4double realRefraIndex[nEntries] = {};
	fill(realRefraIndex, realRefraIndex + nEntries, 2.9);
	G4double imgRefraIndex[nEntries] = {};
	fill(imgRefraIndex, imgRefraIndex + nEntries, 1.6);

	G4double reflectivity[nEntries] = {};
	fill(reflectivity, reflectivity + nEntries, 1.0);

	G4double quatumnEfficiency[] =
	{ 0.0005, 0.0006, 0.0009, 0.0013, 0.0021, 0.0034, 0.0068, 0.0093, 0.0129, 0.0184, 0.0289, 0.0436,
	  0.0624, 0.0903, 0.1354, 0.1785, 0.2165, 0.2461, 0.2530, 0.2460, 0.2268, 0.1802, 0.1222, 0.0847,
	  0.0510, 0.0387 };
	assert(sizeof(quatumnEfficiency) == sizeof(photonEnergy));

	G4MaterialPropertiesTable* photocathodeMPT = new G4MaterialPropertiesTable();
	photocathodeMPT->AddProperty("EFFICIENCY", photonEnergy, quatumnEfficiency, nEntries, false, true);// ->SetSpline(true);
	photocathodeMPT->AddProperty("REALRINDEX", photonEnergy, realRefraIndex, nEntries, false, true);// ->SetSpline(true);
	photocathodeMPT->AddProperty("IMAGINARYRINDEX", photonEnergy, imgRefraIndex, nEntries, false, true);// ->SetSpline(true);

	G4cout << "Photocathode G4MaterialPropertiesTable" << G4endl;
	photocathodeMPT->DumpTable();

	G4OpticalSurface* opPhotocathodeSurf =
		new G4OpticalSurface("PhotocathodeSurface", unified, polished, dielectric_metal);
	opPhotocathodeSurf->SetMaterialPropertiesTable(photocathodeMPT);
	new G4LogicalSkinSurface("PhotocathodeSurface", logicPhotocathode, opPhotocathodeSurf);



	// 
	// Visualization attributes
	//
	G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
	visAttributes->SetVisibility(false);
	logicWorld->SetVisAttributes(visAttributes);
	logicContainer->SetVisAttributes(visAttributes);
	logicModuleRow->SetVisAttributes(visAttributes);
	logicModule->SetVisAttributes(visAttributes);
	logicGdMylarFilm->SetVisAttributes(visAttributes);
	logicAlMylarFilm->SetVisAttributes(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(0., 1.0, 0.));
	visAttributes->SetVisibility(false);
	logicGdFilm->SetVisAttributes(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(1.0, 0., 0.));
	visAttributes->SetVisibility(false);
	logicAlFilm->SetVisAttributes(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(3. / 255., 130. / 255., 233. / 255., 1.));
	visAttributes->SetVisibility(true);
	//visAttributes->SetForceSolid(true);
	//visAttributes->SetForceWireframe(true);
	//visAttributes->SetForceAuxEdgeVisible(true);
	logicPlasticScintillator->SetVisAttributes(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.5));
	visAttributes->SetVisibility(true);
	visAttributes->SetForceSolid(true);
	logicPMT->SetVisAttributes(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 1.));
	visAttributes->SetVisibility(true);
	visAttributes->SetForceSolid(true);
	logicChamberPMT->SetVisAttributes(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(204. / 255, 205. / 255, 207. / 255));
	visAttributes->SetVisibility(true);
	visAttributes->SetForceSolid(true);
	//visAttributes->SetForceWireframe(true);
	visAttributes->SetForceAuxEdgeVisible(true);
	logicHat->SetVisAttributes(visAttributes);
	
	//
	// Always return the physical World
	//
	return physWorld;
}

void PANDASimDetectorConstruction::ConstructSDandField()
{
	G4SDManager* sdManager = G4SDManager::GetSDMpointer();

	// Sensitive detectors
	PANDASimScinitillatorSD* scinitillatorSD
		= new PANDASimScinitillatorSD("ScinitillatorSD", "ScinitillatorHitsCollection", arraySize * arraySize);
	sdManager->AddNewDetector(scinitillatorSD);
	SetSensitiveDetector("PlasticScintillatorLV", scinitillatorSD);

	auto gdFilmSD
		= new PANDASimGdFilmSD("GdFilmSD", "GdFilmHitsCollection", arraySize * arraySize);
	sdManager->AddNewDetector(gdFilmSD);
	SetSensitiveDetector("GdFilmLV", gdFilmSD);

	auto photocathodeSD
		= new PANDASimPhotocathodeSD("PhotocathodeSD", "PhotocathodeHitsCollection", arraySize * arraySize * 2);
	sdManager->AddNewDetector(photocathodeSD);
	SetSensitiveDetector("PhotocathodeLV", photocathodeSD);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimDetectorConstruction::DefineFilmLogicAndPhysVolume
	(G4double filmBoxHalfSize[3], G4Material* filmMaterial, const G4String& filmName,
	 G4double antiBoxHalfSize[3], G4LogicalVolume* logicMotherVolume,
	 G4LogicalVolume*& logicFilmVolume, G4VPhysicalVolume*& physFilmVolume)
{
	G4Box* solidFilmBox =
		new G4Box(filmName + "BoxSV", filmBoxHalfSize[0], filmBoxHalfSize[1], filmBoxHalfSize[2]);
	G4Box* solidAnti =
		new G4Box("AntiBoxSV", 1.1 * antiBoxHalfSize[0], antiBoxHalfSize[1], antiBoxHalfSize[2]);
	G4BooleanSolid* solidFilm = new G4SubtractionSolid(filmName, solidFilmBox, solidAnti);
	logicFilmVolume =
		new G4LogicalVolume(solidFilm, filmMaterial, filmName + "LV");
	physFilmVolume = 
		new G4PVPlacement(0, G4ThreeVector(), logicFilmVolume, filmName + "PV", logicMotherVolume, false, 0, checkOverlaps);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
