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

#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"

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
	fScoringVolume(nullptr), fPhotoelectricScoringVolume(nullptr), fGdFilmScoringVolume(nullptr),
	checkOverlaps(true), addLabRoom(false)
{
	arraySize = 4;// UserDataInput::GetSizeOfArray();
	dtctrX = 10. * cm;// UserDataInput::GetDectorDimensionX();
	dtctrY = 10. * cm;// UserDataInput::GetDectorDimensionY();
	dtctrZ = 100. * cm;// UserDataInput::GetDectorDimensionZ();
	gdFilmThickness = 30. * um;// UserDataInput::GetGdFilmThickness();
	distanceBetweenModules = 2. * cm;// UserDataInput::GetDistanceBetweenModules();

	roofXY = 10. * m;
	roofZ = 0.25 * m;
	roomHeight = 4. * m;

	DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimDetectorConstruction::~PANDASimDetectorConstruction()
{
	delete fMessenger;

	//for (auto visAttributes : fVisAttributes)
	//{
	//	delete visAttributes;
	//}
	//for (auto rotationMatrix : fRotationMatrix)
	//{
	//	delete rotationMatrix;
	//}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* PANDASimDetectorConstruction::Construct()
{
	DefineMaterials();

	air = G4Material::GetMaterial("G4_AIR");

	if (!air)
	{
		G4ExceptionDescription msg;
		msg << "Cannot retrieve materials already defined.";
		G4Exception("PANDASimDetectorConstruction::Construct()", "Air", FatalException, msg);
	}

	// Geometry parameters
	heightPMT = 4. * cm;
	mylarFilmThickness = 40. * um;
	alFilmThickness = 30. * um;

	scintillatorModuleZ = dtctrX + 2. * (gdFilmThickness + alFilmThickness + mylarFilmThickness * 2.);
	scintillatorModuleY = dtctrY + 2. * (gdFilmThickness + alFilmThickness + mylarFilmThickness * 2.);
	scintillatorModuleX = dtctrZ;
	moduleZ = scintillatorModuleZ + distanceBetweenModules;
	moduleY = scintillatorModuleY + distanceBetweenModules;
	moduleX = scintillatorModuleX + 2. * heightPMT + 2 * distanceBetweenModules;

	containerZ = arraySize * moduleZ;
	containerY = arraySize * moduleY;
	containerX = moduleX;

	if (addLabRoom)
	{
		auto concrete = G4Material::GetMaterial("G4_CONCRETE");

		if (!concrete)
		{
			G4ExceptionDescription msg;
			msg << "Cannot retrieve materials already defined.";
			G4Exception("PANDASimDetectorConstruction::Construct()", "Concrete", FatalException, msg);
		}

		roofXY = 10. * m;
		roofZ = 0.25 * m;
		roomHeight = 4. * m;

		G4double wallThickness = 1. * m;
		G4double roomZ = 2. * (roomHeight + roofZ);
		G4double roomXY = roofXY + 2. * wallThickness;

		worldX = 1.1 * (containerX + roomXY + 100. * m);
		worldY = 1.1 * (containerY + roomXY + 100. * m);
		worldZ = 1.1 * (4. * (roofZ + roomHeight));

		// World
		//
		solidWorld = new G4Box("WorldSV", 0.5 * worldX, 0.5 * worldY, 0.5 * worldZ);
		logicWorld = new G4LogicalVolume(solidWorld, air, "WorldLV");
		physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "WorldPV", 0, false, 0, checkOverlaps);

		auto solidRoof = new G4Box("RoofSV", 0.5 * roofXY, 0.5 * roofXY, 0.5 * roofZ);
		auto logicRoof = new G4LogicalVolume(solidRoof, concrete, "RoofLV");
		auto physRoof1 = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.5 * roofZ + roomHeight - containerZ * 0.5), logicRoof, "RoofPV1", logicWorld, false, 0, checkOverlaps);
		auto physRoof2 = new G4PVPlacement(0, G4ThreeVector(0., 0., 1.5 * roofZ + 2. * roomHeight - containerZ * 0.5), logicRoof, "RoofPV2", logicWorld, false, 0, checkOverlaps);

		auto solidSolidRoom = new G4Box("SolidRoomSV", 0.5 * roomXY, 0.5 * roomXY, 0.5 * roomZ);
		auto solidAirRoom = new G4Box("AirRoomSV", 0.5 * roofXY, 0.5 * roofXY, roomZ);
		auto solidRoom = new G4SubtractionSolid("RoomSV", solidSolidRoom, solidAirRoom);
		auto logicRoom = new G4LogicalVolume(solidRoom, concrete, "RoomLV");
		auto physRoom = new G4PVPlacement(0, G4ThreeVector(0., 0., roofZ + roomHeight - 0.5 * containerZ), logicRoom, "RoomPV", logicWorld, false, 0, checkOverlaps);
	}
	else
	{
		worldX = 1.1 * (containerX + 10. * m);
		worldY = 1.1 * (containerY + 10. * m);
		worldZ = 1.1 * containerZ;

		// World
		//
		solidWorld = new G4Box("WorldSV", 0.5 * worldX, 0.5 * worldY, 0.5 * worldZ);
		logicWorld = new G4LogicalVolume(solidWorld, air, "WorldLV");
		physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "WorldPV", 0, false, 0, checkOverlaps);
	}

	DefineDetector(logicWorld);

	//
	// Always return the physical World
	//
	return physWorld;
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
	if (addLabRoom)
		G4Material* concrete = nistManager->FindOrBuildMaterial("G4_CONCRETE");

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
	std::fill(scintillatorRefractiveIndex, scintillatorRefractiveIndex + nEntries, 1.58);
	G4double scintillatorAbsorption[nEntries] = {};
	std::fill(scintillatorAbsorption, scintillatorAbsorption + nEntries, 380. * cm);

	G4double scintilFast[] =
	{ 0.0595, 0.0615, 0.0724, 0.0844, 0.1053, 0.1218, 0.1382, 0.1547, 0.1821, 0.2095, 0.2369,
	  0.2644, 0.2863, 0.3092, 0.3411, 0.3795, 0.4014, 0.4179, 0.4253, 0.4618, 0.4892, 0.5221,
	  0.5495, 0.5769, 0.6098, 0.6357, 0.6756, 0.7085, 0.7469, 0.7908, 0.8252, 0.8557, 0.8904,
	  0.9244, 0.9568, 0.9772, 1.0000, 0.9608, 0.9114, 0.8566, 0.8017, 0.7469, 0.6921, 0.6372,
	  0.5824, 0.5276, 0.4727, 0.4179, 0.3631, 0.3082, 0.2589, 0.2095, 0.1657, 0.1163, 0.0734,
	  0.0455, 0.0176, 0.0011 };

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
	std::fill(airRefractiveIndex, airRefractiveIndex + nEntries, 1.0);

	G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
	airMPT->AddProperty("RINDEX", photonEnergy, airRefractiveIndex, nEntries);

	G4cout << "Air/Vacuum G4MaterialPropertiesTable" << G4endl;
	airMPT->DumpTable();

	air->SetMaterialPropertiesTable(airMPT);
	vacuum->SetMaterialPropertiesTable(airMPT);

	// glass
	G4double glassRefractiveIndex[nEntries] = {};
	std::fill(glassRefractiveIndex, glassRefractiveIndex + nEntries, 1.53);
	G4double glassAbsorption[nEntries] = {};
	std::fill(glassAbsorption, glassAbsorption + nEntries, 420. * cm);

	G4MaterialPropertiesTable* glassMPT = new G4MaterialPropertiesTable();
	glassMPT->AddProperty("RINDEX", photonEnergy, glassRefractiveIndex, nEntries);
	glassMPT->AddProperty("ABSLENGTH", photonEnergy, glassAbsorption, nEntries);
	glass->SetMaterialPropertiesTable(glassMPT);

	G4cout << "Glass G4MaterialPropertiesTable" << G4endl;
	glassMPT->DumpTable();

	// Print materials
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}


void PANDASimDetectorConstruction::DefineDetector(G4LogicalVolume* world)
{

	// Geometry parameters
	G4double outerRadiusPMT = 5.1 / 2. * cm;
	G4double innerRadiusPMT = 4.6 / 2. * cm;
	G4double glassThicknessPMT = outerRadiusPMT - innerRadiusPMT;

	G4double radiusPhotocathode = innerRadiusPMT;
	G4double heightPhotocathode = 26. * nm;

	// Get materials
	G4Material* plasticScintillator = G4Material::GetMaterial("PlasticScintillator");
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

	// detector array
	//
	G4Box* solidContainer = new G4Box("ContaineSV", containerX / 2., containerY / 2., containerZ / 2.);
	G4LogicalVolume* logicContainer = new G4LogicalVolume(solidContainer, air, "ContainerLV");
	G4RotationMatrix* rotC = new G4RotationMatrix();
	rotC->rotateX(0/*pi / 2.*/);
	G4PVPlacement* physContainer = new G4PVPlacement(rotC, G4ThreeVector(), logicContainer, "ContainerPV", world, false, 0, checkOverlaps);
	//fRotationMatrix.push_back(rotC);
	//fSolidVolume.push_back(solidContainer);
	//fLogicalVolume.push_back(logicContainer);
	//fPhysicalVolume.push_back(physContainer);

	G4Box* solidModuleRow = new G4Box("ModuleRowSV", moduleX / 2., moduleY / 2., arraySize * moduleZ / 2.);
	G4LogicalVolume* logicModuleRow = new G4LogicalVolume(solidModuleRow, air, "ModuleRowLV");
	G4PVReplica* physModuleRow = new G4PVReplica("ModuleRowPV", logicModuleRow, logicContainer, kYAxis, arraySize, moduleY);
	//fSolidVolume.push_back(solidModuleRow);
	//fLogicalVolume.push_back(logicModuleRow);
	//fPhysicalVolume.push_back(physModuleRow);

	G4Box* solidModule = new G4Box("ModuleSV", moduleX / 2., moduleY / 2., moduleZ / 2.);
	G4LogicalVolume* logicModule = new G4LogicalVolume(solidModule, air, "ModuleLV");
	G4PVReplica* physModule = new G4PVReplica("ModulePV", logicModule, logicModuleRow, kZAxis, arraySize, moduleZ);
	//fSolidVolume.push_back(solidModule);
	//fLogicalVolume.push_back(logicModule);
	//fPhysicalVolume.push_back(physModule);

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
	G4PVPlacement* physAlFilm;
	DefineFilmLogicAndPhysVolume("AlFilm",     // name
		alFilmHalfSize,   // size
		scintillatorHalfSize,  // antiBoxSize
		aluminium,    // material
		logicModule,  // mother logical volume
		logicAlFilm,  // logical
		physAlFilm);  // physical
	//fLogicalVolume.push_back(logicAlFilm);
	//fPhysicalVolume.push_back(physAlFilm);

	G4LogicalVolume* logicAlMylarFilm;
	G4PVPlacement* physAlMylarFilm;
	DefineFilmLogicAndPhysVolume("AlMylarFilm",     // name
		alMylarFilmHalfSize,   // size
		alFilmHalfSize,  // antiBoxSize
		mylarFilm,    // material
		logicModule,  // mother logical volume
		logicAlMylarFilm,  // logical
		physAlMylarFilm);  // physical
	//fLogicalVolume.push_back(logicAlMylarFilm);
	//fPhysicalVolume.push_back(physAlMylarFilm);

	//G4LogicalVolume* 
		logicGdFilm;
	G4PVPlacement* physGdFilm;
	DefineFilmLogicAndPhysVolume("GdFilm", gdFilmHalfSize, alMylarFilmHalfSize, gadoliniumOxide, logicModule, logicGdFilm, physGdFilm);
	//fLogicalVolume.push_back(logicGdFilm);
	//fPhysicalVolume.push_back(physGdFilm);

	fGdFilmScoringVolume = logicGdFilm;

	G4LogicalVolume* logicGdMylarFilm;
	G4PVPlacement* physGdMylarFilm;
	DefineFilmLogicAndPhysVolume("GdMylarFilm",	gdMylarFilmHalfSize, gdFilmHalfSize, mylarFilm,	logicModule, logicGdMylarFilm, physGdMylarFilm);
	//fLogicalVolume.push_back(logicGdMylarFilm);
	//fPhysicalVolume.push_back(physGdMylarFilm);

	G4Box* solidPlasticScintillator = new G4Box("PlasticScintillatorSV", dtctrZ / 2., dtctrY / 2., dtctrX / 2.);
	//G4LogicalVolume* 
		logicPlasticScintillator = new G4LogicalVolume(solidPlasticScintillator, plasticScintillator, "PlasticScintillatorLV");
	G4PVPlacement* physPlasticScinitillator = 
		new G4PVPlacement(0, G4ThreeVector(), logicPlasticScintillator, "PlasticScintillatorPV", logicModule, false, 0, checkOverlaps);
	//fSolidVolume.push_back(solidPlasticScintillator);
	//fLogicalVolume.push_back(logicPlasticScintillator);
	//fPhysicalVolume.push_back(physPlasticScinitillator);

	fScoringVolume = logicPlasticScintillator;

	G4Tubs* solidPMT = new G4Tubs("PMTSV", 0., outerRadiusPMT, heightPMT / 2., 0., twopi);
	//G4BooleanSolid* solidTubePMT = 
	//	new G4SubtractionSolid("TubePMT", solidPMT, solidChamberPMT, 0, G4ThreeVector(0, 0, -wallThicknessPMT));
	//G4Box* solidPMTBox = new G4Box("PMTBox", dtctrX / 2., dtctrY / 2., heightPMT / 2.);
	G4LogicalVolume* logicPMT = new G4LogicalVolume(solidPMT, glass, "PMTLV");
	//fSolidVolume.push_back(solidPMT);
	//fLogicalVolume.push_back(logicPMT);

	G4Tubs* solidChamberPMT = new G4Tubs("ChamberPMTSV", 0., innerRadiusPMT, heightPMT / 2. - glassThicknessPMT, 0., twopi);
	G4LogicalVolume* logicChamberPMT = new G4LogicalVolume(solidChamberPMT, vacuum, "ChamberPMTLV");
	G4PVPlacement* physChamberPMT = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicChamberPMT, "ChamberPMTPV", logicPMT, false, 0, checkOverlaps);
	//fSolidVolume.push_back(solidChamberPMT);
	//fLogicalVolume.push_back(logicChamberPMT);
	//fPhysicalVolume.push_back(physChamberPMT);

	G4Tubs* solidPhotocathode = new G4Tubs("PhotocathodeSV", 0., radiusPhotocathode, heightPhotocathode / 2., 0., twopi);
	//G4Box* solidPhotocathodeBox = new G4Box("PhotocathodeBox", dtctrX / 2., dtctrY / 2., heightPhotocathode / 2.);
	//G4LogicalVolume* 
		logicPhotocathode = new G4LogicalVolume(solidPhotocathode, bialkaliPhotocathode, "PhotocathodeLV");
	G4PVPlacement* physPhotocathode =
		new G4PVPlacement(0, G4ThreeVector(0, 0, -heightPhotocathode / 2. + solidChamberPMT->GetZHalfLength()),
			logicPhotocathode, "PhotocathodePV", logicChamberPMT, false, 0, checkOverlaps);
	//fSolidVolume.push_back(solidPhotocathode);
	//fLogicalVolume.push_back(logicPhotocathode);
	//fPhysicalVolume.push_back(physPhotocathode);

	fPhotoelectricScoringVolume = logicPhotocathode;

	G4RotationMatrix* rotPMTLeft = new G4RotationMatrix();
	rotPMTLeft->rotateY(-pi / 2.);
	G4PVPlacement* physPMTLeft =
		new G4PVPlacement(rotPMTLeft, G4ThreeVector(-(scintillatorModuleX + heightPMT) / 2., 0., 0.), logicPMT, "PMTLeftPV", logicModule, false, 1, checkOverlaps);
	//fRotationMatrix.push_back(rotPMTLeft);
	//fPhysicalVolume.push_back(physPMTLeft);

	G4RotationMatrix* rotPMTRight = new G4RotationMatrix();
	rotPMTRight->rotateY(pi / 2.);
	G4PVPlacement* physPMTRight =
		new G4PVPlacement(rotPMTRight, G4ThreeVector((scintillatorModuleX + heightPMT) / 2., 0., 0.), logicPMT, "PMTRightPV", logicModule, false, 0, checkOverlaps);
	//fRotationMatrix.push_back(rotPMTRight);
	//fPhysicalVolume.push_back(physPMTRight);

	G4double hatHalfThickness = 1.5 * cm;// distanceBetweenModules / 2.;
	G4Box* solidHatBox = new G4Box("HatBoxSV", hatHalfThickness, moduleY / 2., moduleZ / 2.);
	G4Box* solidAntiHatBox = new G4Box("AntiHatBoxSV", hatHalfThickness, scintillatorModuleY / 2., scintillatorModuleZ / 2.);
	G4Tubs* solidAntiHatCylinder = new G4Tubs("AntiHatCylinderSV", 0, outerRadiusPMT, 2. * hatHalfThickness, 0, twopi);
	G4BooleanSolid* solidHatTemp = new G4SubtractionSolid("HatTempSV", solidHatBox, solidAntiHatBox, 0, G4ThreeVector(hatHalfThickness, 0, 0));
	G4BooleanSolid* solidHat = new G4SubtractionSolid("HatSV", solidHatTemp, solidAntiHatCylinder, rotPMTRight, G4ThreeVector());
	G4LogicalVolume* logicHat = new G4LogicalVolume(solidHat, aluminium, /*stainlessSteel,*/ "HatLV");
	G4RotationMatrix* rotHat = new G4RotationMatrix;
	rotHat->rotateZ(pi);
	G4VPhysicalVolume* physHatRight =
		new G4PVPlacement(rotHat, G4ThreeVector(scintillatorModuleX / 2., 0, 0), logicHat, "HatRightPV", logicModule, false, 0, checkOverlaps);
	G4VPhysicalVolume* physHatLeft =
		new G4PVPlacement(0, G4ThreeVector(-scintillatorModuleX / 2., 0, 0), logicHat, "HatLeftPV", logicModule, false, 1, checkOverlaps);
	//fRotationMatrix.push_back(rotHat);
	//fSolidVolume.push_back(solidHatBox);
	//fSolidVolume.push_back(solidAntiHatBox);
	//fSolidVolume.push_back(solidAntiHatCylinder);
	//fSolidVolume.push_back(solidHatTemp);
	//fSolidVolume.push_back(solidHat);
	//fLogicalVolume.push_back(logicHat);
	//fPhysicalVolume.push_back(physHatRight);
	//fPhysicalVolume.push_back(physHatLeft);

	////G4_Be orb for Am-Be neutron source
	//G4String sourceType =
	//	(static_cast<const PANDASimPrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction()))->GetSourceType();
	//if (/*UserDataInput::GetSoureType()*/ sourceType == "Am-Be")
	//{
	//	G4Orb* solidBeOrb = new G4Orb("BeOrbSV", 0.5 * mm);
	//	G4LogicalVolume* logicBeOrb = new G4LogicalVolume(solidBeOrb, berylliumForAmBe, "BeOrbLV");
	//	auto sourcePosition = (static_cast<const PANDASimPrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction()))->GetSourcePosition();
	//	G4ThreeVector positionVector;
	//	if (sourcePosition == "EDGE")
	//	{
	//		positionVector = G4ThreeVector(scintillatorHalfSize[0] - distanceBetweenModules, 0., 0.5 * mm + containerZ / 2.);
	//	}
	//	else
	//	{
	//		positionVector = G4ThreeVector(0., 0., 0.5 * mm + containerZ / 2.);
	//	}
	//	G4VPhysicalVolume* physBeOrb = new G4PVPlacement(0, positionVector, logicBeOrb, "BeOrbPV", world, false, 0, checkOverlaps);
	//	//fSolidVolume.push_back(solidBeOrb);
	//	//fLogicalVolume.push_back(logicBeOrb);
	//	//fPhysicalVolume.push_back(physBeOrb);
	//}

	//
	//----------------Define optical surfaces----------------
	//
	// sci-reflector
	G4OpticalSurface* opScinRelfleSurface = new G4OpticalSurface("ScinMetalOpticalSurface");
	opScinRelfleSurface->SetType(dielectric_metal);
	opScinRelfleSurface->SetFinish(polished);
	opScinRelfleSurface->SetModel(glisur);
	//fOpticalSurface.push_back(opScinRelfleSurface);

	G4LogicalBorderSurface* scinRefleSurface = new G4LogicalBorderSurface("ScinRefleSurface", physPlasticScinitillator, physAlFilm, opScinRelfleSurface);
	G4LogicalBorderSurface* scinHatRightSurface = new G4LogicalBorderSurface("ScinHatRightSurface", physPlasticScinitillator, physHatRight, opScinRelfleSurface);
	G4LogicalBorderSurface* scinHatLeftSurface = new G4LogicalBorderSurface("ScinHatLeftSurface", physPlasticScinitillator, physHatLeft, opScinRelfleSurface);
	//fLogicalSurface.push_back(scinRefleSurface);
	//fLogicalSurface.push_back(scinHatRightSurface);
	//fLogicalSurface.push_back(scinHatLeftSurface);

	// sci-PMT glss
	G4OpticalSurface* opScinPMTSurface = new G4OpticalSurface("ScinPMTOpticalSurface");
	opScinPMTSurface->SetType(dielectric_dielectric);
	opScinPMTSurface->SetFinish(polished);
	opScinPMTSurface->SetModel(unified);
	//fOpticalSurface.push_back(opScinPMTSurface);

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

	G4LogicalBorderSurface* PMTRightSurface = new G4LogicalBorderSurface("ScinPMTRightSurface", physPlasticScinitillator, physPMTRight, opScinPMTSurface);
	G4LogicalBorderSurface* PMTLeftSurface = new G4LogicalBorderSurface("ScinPMTLeftSurface", physPlasticScinitillator, physPMTLeft, opScinPMTSurface);
	//fLogicalSurface.push_back(PMTRightSurface);
	//fLogicalSurface.push_back(PMTLeftSurface);

	// Photocathode surface properties
	G4double photonEnergy[] =
	{ 1.8338 * eV, 1.8419 * eV, 1.8527 * eV, 1.8721 * eV, 1.8890 * eV, 1.9172 * eV, 1.9530 * eV, 1.9800 * eV,
	  2.0022 * eV, 2.0413 * eV, 2.0845 * eV, 2.1479 * eV, 2.2163 * eV, 2.2922 * eV, 2.4194 * eV, 2.5563 * eV,
	  2.7037 * eV, 2.8891 * eV, 3.0268 * eV, 3.1703 * eV, 3.3728 * eV, 3.6556 * eV, 3.9353 * eV, 4.0806 * eV,
	  4.2007 * eV, 4.2506 * eV };
	const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double);

	G4double realRefraIndex[nEntries] = {};
	std::fill(realRefraIndex, realRefraIndex + nEntries, 2.9);
	G4double imgRefraIndex[nEntries] = {};
	std::fill(imgRefraIndex, imgRefraIndex + nEntries, 1.6);

	G4double reflectivity[nEntries] = {};
	std::fill(reflectivity, reflectivity + nEntries, 1.0);

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
	//fMPT.push_back(photocathodeMPT);

	G4OpticalSurface* opPhotocathodeSurf = new G4OpticalSurface("PhotocathodeSurface", unified, polished, dielectric_metal);
	opPhotocathodeSurf->SetMaterialPropertiesTable(photocathodeMPT);
	//fOpticalSurface.push_back(opPhotocathodeSurf);

	G4LogicalSkinSurface* photocathodeSurface = new G4LogicalSkinSurface("PhotocathodeSurface", logicPhotocathode, opPhotocathodeSurf);
	//fLogicalSurface.push_back(photocathodeSurface);

	// 
	// Visualization attributes
	//
	G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
	visAttributes->SetVisibility(false);
	world->SetVisAttributes(visAttributes);
	logicContainer->SetVisAttributes(visAttributes);
	logicModuleRow->SetVisAttributes(visAttributes);
	logicModule->SetVisAttributes(visAttributes);
	logicGdMylarFilm->SetVisAttributes(visAttributes);
	logicAlMylarFilm->SetVisAttributes(visAttributes);
	//fVisAttributes.push_back(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(0., 1.0, 0.));
	visAttributes->SetVisibility(false);
	logicGdFilm->SetVisAttributes(visAttributes);
	//fVisAttributes.push_back(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(1.0, 0., 0.));
	visAttributes->SetVisibility(false);
	logicAlFilm->SetVisAttributes(visAttributes);
	//fVisAttributes.push_back(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(3. / 255., 130. / 255., 233. / 255., 1.));
	visAttributes->SetVisibility(true);
	//visAttributes->SetForceSolid(true);
	//visAttributes->SetForceWireframe(true);
	//visAttributes->SetForceAuxEdgeVisible(true);
	logicPlasticScintillator->SetVisAttributes(visAttributes);
	fVisAttributes.push_back(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.5));
	visAttributes->SetVisibility(true);
	visAttributes->SetForceSolid(true);
	logicPMT->SetVisAttributes(visAttributes);
	//fVisAttributes.push_back(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 1.));
	visAttributes->SetVisibility(true);
	visAttributes->SetForceSolid(true);
	logicChamberPMT->SetVisAttributes(visAttributes);
	//fVisAttributes.push_back(visAttributes);

	visAttributes = new G4VisAttributes(G4Colour(204. / 255, 205. / 255, 207. / 255));
	visAttributes->SetVisibility(true);
	visAttributes->SetForceSolid(true);
	//visAttributes->SetForceWireframe(true);
	visAttributes->SetForceAuxEdgeVisible(true);
	logicHat->SetVisAttributes(visAttributes);
	//fVisAttributes.push_back(visAttributes);

}

void PANDASimDetectorConstruction::ConstructSDandField()
{
	G4SDManager* sdManager = G4SDManager::GetSDMpointer();

	// Sensitive detectors
	PANDASimScinitillatorSD* scinitillatorSD = new PANDASimScinitillatorSD("ScinitillatorSD", "ScinitillatorHitsCollection", arraySize * arraySize);
	sdManager->AddNewDetector(scinitillatorSD);
	//SetSensitiveDetector("PlasticScintillatorLV", scinitillatorSD);
	logicPlasticScintillator->SetSensitiveDetector(scinitillatorSD);
	//fSD.push_back(scinitillatorSD);

	auto gdFilmSD = new PANDASimGdFilmSD("GdFilmSD", "GdFilmHitsCollection", arraySize * arraySize);
	sdManager->AddNewDetector(gdFilmSD);
	//SetSensitiveDetector("GdFilmLV", gdFilmSD);
	logicGdFilm->SetSensitiveDetector(gdFilmSD);
	//fSD.push_back(gdFilmSD);

	auto photocathodeSD	= new PANDASimPhotocathodeSD("PhotocathodeSD", "PhotocathodeHitsCollection", arraySize * arraySize * 2);
	sdManager->AddNewDetector(photocathodeSD);
	//SetSensitiveDetector("PhotocathodeLV", photocathodeSD);
	logicPhotocathode->SetSensitiveDetector(photocathodeSD);
	//fSD.push_back(photocathodeSD);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimDetectorConstruction::DefineFilmLogicAndPhysVolume(const G4String& filmName, G4double filmBoxHalfSize[3], G4double antiBoxHalfSize[3],
	G4Material* filmMaterial, G4LogicalVolume* logicMotherVolume, G4LogicalVolume*& logicFilmVolume, G4PVPlacement*& physFilmVolume)
{
	G4Box* solidFilmBox = new G4Box(filmName + "BoxSV", filmBoxHalfSize[0], filmBoxHalfSize[1], filmBoxHalfSize[2]);
	G4Box* solidAnti = new G4Box("AntiBoxSV", 1.1 * antiBoxHalfSize[0], antiBoxHalfSize[1], antiBoxHalfSize[2]);
	G4BooleanSolid* solidFilm = new G4SubtractionSolid(filmName + "SV", solidFilmBox, solidAnti);
	logicFilmVolume = new G4LogicalVolume(solidFilm, filmMaterial, filmName + "LV");
	physFilmVolume = new G4PVPlacement(0, G4ThreeVector(), logicFilmVolume, filmName + "PV", logicMotherVolume, false, 0, checkOverlaps);
	//fSolidVolume.push_back(solidFilmBox);
	//fSolidVolume.push_back(solidAnti);
	//fSolidVolume.push_back(solidFilm);
	//fLogicalVolume.push_back(logicFilmVolume);
	//fPhysicalVolume.push_back(physFilmVolume);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void PANDASimDetectorConstruction::UpdateGeometry()
//{
//	for (auto itr:fSolidVolume)
//	{
//		G4SolidStore::GetInstance()->DeRegister(itr);
//		delete itr;
//	}
//	fSolidVolume.clear();
//	for (auto itr : fLogicalVolume)
//	{
//		G4LogicalVolumeStore::GetInstance()->DeRegister(itr);
//		delete itr;
//	}
//	fLogicalVolume.clear();
//	for (auto itr : fPhysicalVolume)
//	{
//		G4PhysicalVolumeStore::GetInstance()->DeRegister(itr);
//		delete itr;
//	}
//	fPhysicalVolume.clear();
//	for (auto itr : fVisAttributes)
//	{
//		delete itr;
//	}
//	fVisAttributes.clear();
//	for (auto itr : fRotationMatrix)
//	{
//		delete itr;
//	}
//	fRotationMatrix.clear();
//	for (auto itr : fOpticalSurface)
//	{
//		delete itr;
//	}
//	fOpticalSurface.clear();
//	for (auto itr : fLogicalSurface)
//	{
//		delete itr;
//	}
//	fLogicalSurface.clear();
//	for (auto itr : fMPT)
//	{
//		delete itr;
//	}
//	fMPT.clear();
//	for (auto itr : fSD)
//	{
//		delete itr;
//	}
//	fSD.clear();
//
//	// Geometry parameters
//	heightPMT = 4. * cm;
//	mylarFilmThickness = 40. * um;
//	alFilmThickness = 30. * um;
//
//	scintillatorModuleZ = dtctrX + 2. * (gdFilmThickness + alFilmThickness + mylarFilmThickness * 2.);
//	scintillatorModuleY = dtctrY + 2. * (gdFilmThickness + alFilmThickness + mylarFilmThickness * 2.);
//	scintillatorModuleX = dtctrZ;
//	moduleZ = scintillatorModuleZ + distanceBetweenModules;
//	moduleY = scintillatorModuleY + distanceBetweenModules;
//	moduleX = scintillatorModuleX + 2. * heightPMT + 2 * distanceBetweenModules;
//
//	containerZ = arraySize * moduleZ;
//	containerY = arraySize * moduleY;
//	containerX = moduleX;
//
//	worldX = containerX + 10. * m;
//	worldY = containerY + 10. * m;
//	worldZ = 1.1 * containerZ;
//
//	solidWorld->SetXHalfLength(worldX * 0.5);
//	solidWorld->SetYHalfLength(worldY * 0.5);
//	solidWorld->SetZHalfLength(worldZ * 0.5);
//
//	DefineDetector(logicWorld);
//
//	//for (auto itr : fSolidVolume)
//	//{
//	//	G4String name = itr->GetName();
//	//	name = name.substr(0, name.size() - 2) + "LV";
//	//	if (name == logicPlasticScintillator->GetName())
//	//	{
//	//		logicPlasticScintillator->SetSolid(itr);
//	//	}
//	//	if (name == logicGdFilm->GetName())
//	//	{
//	//		logicGdFilm->SetSolid(itr);
//	//	}
//	//	if (name == logicPhotocathode->GetName())
//	//	{
//	//		logicPhotocathode->SetSolid(itr);
//	//	}
//	//}
//
//	ConstructSDandField();
//
//	G4RunManager::GetRunManager()->GeometryHasBeenModified();
//
//	//delete solidWorld;
//	//delete logicWorld;
//	//delete physWorld;
//	//physWorld = Construct();
//	//G4RunManager::GetRunManager()->DefineWorldVolume(physWorld);
//
//	//G4RunManager::GetRunManager()->ReinitializeGeometry();
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimDetectorConstruction::DefineCommands()
{
	// Define detector command directory using generic messenger class
	fMessenger = new G4GenericMessenger(this, "/detector/", "Detector control");

	auto& arraySizeCMD = fMessenger->DeclareProperty("arraySize", arraySize, "Set detector array size.");
	arraySizeCMD.SetParameterName("arraySize", true);
	arraySizeCMD.SetDefaultValue("4");
	arraySizeCMD.SetRange("arraySize > 0");

	auto& detectorXCMD = fMessenger->DeclarePropertyWithUnit("detectorX", "cm", dtctrX, "Set detector X dimensions.");
	detectorXCMD.SetParameterName("detectorX", true);
	detectorXCMD.SetDefaultValue("10.");
	detectorXCMD.SetRange("detectorX > 0");

	auto& detectorYCMD = fMessenger->DeclarePropertyWithUnit("detectorY", "cm", dtctrY, "Set detector Y dimensions.");
	detectorYCMD.SetParameterName("detectorY", true);
	detectorYCMD.SetDefaultValue("10.");
	detectorYCMD.SetRange("detectorY > 0");

	auto& detectorZCMD = fMessenger->DeclarePropertyWithUnit("detectorZ", "cm", dtctrZ, "Set detector Z dimensions.");
	detectorZCMD.SetParameterName("detectorZ", true);
	detectorZCMD.SetDefaultValue("100.");
	detectorZCMD.SetRange("detectorZ > 0");

	auto& distanceCMD = fMessenger->DeclarePropertyWithUnit("moduleDistance", "cm", distanceBetweenModules, "Set distance between modules.");
	distanceCMD.SetParameterName("moduleDistance", true);
	distanceCMD.SetDefaultValue("2.");
	distanceCMD.SetRange("moduleDistance >= 0");

	auto& gdFilmCMD = fMessenger->DeclarePropertyWithUnit("gdFilmThickness", "um", gdFilmThickness, "Set Gd film thickness.");
	gdFilmCMD.SetParameterName("gdFilmThickness", true);
	//gdFilmCMD.SetDefaultUnit("um"); //thread-unsafe
	gdFilmCMD.SetDefaultValue("30.");
	gdFilmCMD.SetRange("gdFilmThickness >= 0");

	auto& addLabRoomCMD = fMessenger->DeclareProperty("addLabRoom", addLabRoom, "Add lab room.");
	addLabRoomCMD.SetParameterName("addLabRoom", true);
	addLabRoomCMD.SetDefaultValue("true");

	//auto& updateCMD = fMessenger->DeclareMethod("update", &PANDASimDetectorConstruction::UpdateGeometry, "Update geometry.");

	//auto& armAngleCmd = fMessenger->DeclareMethodWithUnit("armAngle", "deg",
	//		&DetectorConstruction::SetArmAngle,
	//		"Set rotation angle of the second arm.");
	//armAngleCmd.SetParameterName("GdFilm", true);
	//armAngleCmd.SetRange("GdFilm >= 0.");
	//armAngleCmd.SetDefaultValue("30.");
}

