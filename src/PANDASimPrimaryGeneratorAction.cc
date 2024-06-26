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
// $Id: PANDASimPrimaryGeneratorAction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file PANDASimPrimaryGeneratorAction.cc
/// \brief Implementation of the PANDASimPrimaryGeneratorAction class

#include "PANDASimPrimaryGeneratorAction.hh"
#include "Spline.hh"

#include "CalculateReferencePoints.hh"

#include "G4Neutron.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4Geantino.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Run.hh"

#include "PANDASimDetectorConstruction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//using namespace std;

PANDASimPrimaryGeneratorAction::PANDASimPrimaryGeneratorAction(const char* inputfile)
	: G4VUserPrimaryGeneratorAction()/*,
	fParticle(nullptr), fPositron(nullptr), fNeutron(nullptr),
	 fMuonN(nullptr), fMuonP(nullptr), fProton(nullptr), fGamma(nullptr),
	 fParticleGun(nullptr), fParticleGunP(nullptr),
	 arraySize(0), neutrinoPosition(0),
	 scinitillatorXHalfLength(0.), scinitillatorYHalfLength(0.), scinitillatorZHalfLength(0.),
	 containerXHalfLength(0.), containerYHalfLength(0.), containerZHalfLength(0.),
	 distanceBetweenModules(0.) , sourceType("NEUTRINO"), sourcePosition("CENTER"), referencePoints(0)*/
{
	G4int n_particle = 1;
	fParticleGun = new G4ParticleGun(n_particle);
	fGPS = new G4GeneralParticleSource();
	// fParticleGunP = new G4ParticleGun(n_particle);

	particleTable = G4ParticleTable::GetParticleTable();
	ionTable = G4IonTable::GetIonTable();
	auto aa = particleTable->GetGenericIon();
	G4String particleName;
	fPositron = particleTable->FindParticle(particleName = "e+");
	fNeutron = particleTable->FindParticle(particleName = "neutron");
	// fNeutron = G4Neutron::Definition();
	fMuonN = particleTable->FindParticle(particleName = "mu-");
	fMuonP = particleTable->FindParticle(particleName = "mu+");
	fGamma = particleTable->FindParticle(particleName = "gamma");
	fProton = particleTable->FindParticle(particleName = "proton");
	fParticle = G4Geantino::Geantino();

	runID = 0;

	logicVolStroe = G4LogicalVolumeStore::GetInstance();

	G4LogicalVolume* logicWorld = logicVolStroe->GetVolume("WorldLV");
	G4Box* worldBox = dynamic_cast<G4Box*>(logicWorld->GetSolid());
	worldXHalfLength = worldBox->GetXHalfLength();
	worldYHalfLength = worldBox->GetYHalfLength();
	worldZHalfLength = worldBox->GetZHalfLength();

	//G4LogicalVolume* logicPlasticScintillator = logicVolStroe->GetVolume("PlasticScintillatorLV");
	//G4Box* scinitillatorBox = dynamic_cast<G4Box*>(logicPlasticScintillator->GetSolid());
	//scinitillatorXHalfLength = scinitillatorBox->GetXHalfLength();
	//scinitillatorYHalfLength = scinitillatorBox->GetYHalfLength();
	//scinitillatorZHalfLength = scinitillatorBox->GetZHalfLength();

	//auto containerLV = logicVolStroe->GetVolume("ContainerLV");
	//G4Box* containerBox = dynamic_cast<G4Box*>(containerLV->GetSolid());
	//containerXHalfLength = containerBox->GetXHalfLength();
	//containerYHalfLength = containerBox->GetYHalfLength();
	//containerZHalfLength = containerBox->GetZHalfLength();

	//auto moduleLV = logicVolStroe->GetVolume("ModuleLV");
	//G4Box* moduleBox = dynamic_cast<G4Box*>(moduleLV->GetSolid());
	//G4double moduleYHalfLength = moduleBox->GetYHalfLength();

	//arraySize = (G4int)std::round(containerYHalfLength / moduleYHalfLength);
	//distanceBetweenModules = (moduleYHalfLength - scinitillatorYHalfLength) * 2;

	auto detetctorConstruction = static_cast<const PANDASimDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

	arraySize = detetctorConstruction->GetArrySize();
	distanceBetweenModules = detetctorConstruction->GetModuleDistance();

	auto containerXYZ = detetctorConstruction->GetContainerXYZ();
	containerXHalfLength = 0.5 * containerXYZ[0];
	containerYHalfLength = 0.5 * containerXYZ[1];
	containerZHalfLength = 0.5 * containerXYZ[2];

	auto detectorXYZ = detetctorConstruction->GetDetectorXYZ();
	scinitillatorXHalfLength = 0.5 * detectorXYZ[2];
	scinitillatorYHalfLength = 0.5 * detectorXYZ[1];
	scinitillatorZHalfLength = 0.5 * detectorXYZ[0];

	CalculateReferencePoints refP = CalculateReferencePoints();
	referencePoints = refP.GetRefrencePoints();

	sourceType = "NEUTRINO";// UserDataInput::GetSoureType();
	sourcePosition = "CENTER";// UserDataInput::GetSourePosition();
	neutrinoPosition = {16, 5};// UserDataInput::GetPositionOfNeutrino();

	//arraySize = UserDataInput::GetSizeOfArray();
	//distanceBetweenModules = UserDataInput::GetDistanceBetweenModules();

	//neutrinoPercentage = UserDataInput::GetNeutrinoPercentage();

	neutronEnergy = new std::vector<G4double>; // UserDataInput::GetNeutronEnergy();
	neutronEnergy->push_back(0.0253 * eV);
	neutronCDFSpectrum = new std::vector<G4double>; // UserDataInput::GetNeutronCDFSpectrum();
	neutronCDFSpectrum->push_back(1);
	positronEnergy = new std::vector<G4double>; // UserDataInput::GetPositronEnergy();
	positronEnergy->push_back(5. * MeV);
	positronCDFSpectrum = new std::vector<G4double>; // UserDataInput::GetPositronCDFSpectrum();
	positronCDFSpectrum->push_back(1);

	splineForNeutron = nullptr; // new SplineSpace::Spline(&neutronCDFSpectrum[0], &neutronEnergy[0], neutronEnergy.size());
	splineForPositron = nullptr; // new SplineSpace::Spline(&positronCDFSpectrum[0], &positronEnergy[0], positronEnergy.size());

#ifdef __linux__
	// char* CRYHome = getenv("CRYHOME");
	// std::stringstream CRYData;
	// CRYData << CRYHome << "/data";
	// CRYDataPath = CRYData.str();

	//char* CRYData = getenv("CRYDATAPATH");
	//char* CRYData = G4GetEnv("CRYDATAPATH", &CRYDataPath);
	//CRYDataPath = CRYData;

	CRYDataPath = "/mnt/d/Linux/cry/cry_v1.7/data";
	CRYDataPath = G4GetEnv("CRYDATAPATH", CRYDataPath);

	// Read the cry input file
	std::ifstream inputFile;
	inputFile.open(inputfile, std::ios::in);
	char buffer[1000];

	if (inputFile.fail())
	{
		if (*inputfile != 0) //....only complain if a filename was given
			G4cout << "PrimaryGeneratorAction: Failed to open CRY input file= " << inputfile << G4endl;
		InputState = -1;
	}
	else
	{
		std::string setupString("");
		while (!inputFile.getline(buffer, 1000).eof())
		{
			setupString.append(buffer);
			setupString.append(" ");
		}

		// CRYSetup *setup = new CRYSetup(setupString, CRYDataPath);
		//
		// gen = new CRYGenerator(setup);
		//
		// // set random number generator
		// RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
		// setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
		// InputState = 0;
		UpdateCRY(&setupString);
	}
	// create a vector to store the CRY particle properties
	vect = new std::vector<CRYParticle*>;

	// Create the table containing all particle names
	// particleTable = G4ParticleTable::GetParticleTable();

	// Create the messenger file
	gunMessenger = new PrimaryGeneratorMessenger(this);
#endif

	sourceMessenger = new PANDASimPrimaryGeneratorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimPrimaryGeneratorAction::~PANDASimPrimaryGeneratorAction()
{
	delete fParticleGun;
	delete fGPS;
	// delete fParticleGunP;

	delete neutronEnergy;
	delete neutronCDFSpectrum;
	delete positronEnergy;
	delete positronCDFSpectrum;

	if (splineForNeutron)
		delete splineForNeutron;
	if (splineForPositron)
		delete splineForPositron;

#ifdef __linux__
	delete gunMessenger;
	vect->clear();
	delete vect;
	if (gen != nullptr)
		//gen = nullptr;
		delete gen;
#endif
	delete sourceMessenger;
}

#ifdef __linux__
//----------------------------------------------------------------------------//
void PANDASimPrimaryGeneratorAction::InputCRY()
{
	InputState = 1;
}

//----------------------------------------------------------------------------//
void PANDASimPrimaryGeneratorAction::UpdateCRY(std::string* MessInput)
{
	// CRYSetup setup0 = CRYSetup(*MessInput, CRYDataPath);
	// CRYSetup* setup = &setup0;
	CRYSetup *setup = new CRYSetup(*MessInput, CRYDataPath);
	// CRYSetup *setup = new CRYSetup(*MessInput, "/home/lab/programs/cry/cry_v1.7/data");

	gen = new CRYGenerator(setup);

	// set random number generator
	RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
	setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
	InputState = 0;
	delete setup;
}

//----------------------------------------------------------------------------//
void PANDASimPrimaryGeneratorAction::CRYFromFile(G4String newValue)
{
	// Read the cry input file
	std::ifstream inputFile;
	inputFile.open(newValue, std::ios::in);
	char buffer[1000];

	if (inputFile.fail())
	{
		G4cout << "Failed to open input file " << newValue << G4endl;
		G4cout << "Make sure to define the cry library on the command line" << G4endl;
		InputState = -1;
	}
	else
	{
		std::string setupString("");
		while (!inputFile.getline(buffer, 1000).eof())
		{
			setupString.append(buffer);
			setupString.append(" ");
		}

		// CRYSetup *setup = new CRYSetup(setupString, CRYDataPath);
		//
		// gen = new CRYGenerator(setup);
		//
		// // set random number generator
		// RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
		// setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
		// InputState = 0;
		UpdateCRY(&setupString);
	}
}
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	// this function is called at the begining of ecah event
	//

	G4ThreeVector positionVector, directionVector = G4ThreeVector();
	G4double primaryParticleEnergy = 0. * MeV;

	G4int thisID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();

	//if (anEvent->GetEventID() == 0)
	//	runID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
	//else
	//	thisID = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
	if (thisID != runID)
	{
		logicVolStroe = G4LogicalVolumeStore::GetInstance();

		G4LogicalVolume* logicPlasticScintillator = logicVolStroe->GetVolume("PlasticScintillatorLV");
		G4Box* scinitillatorBox = dynamic_cast<G4Box*>(logicPlasticScintillator->GetSolid());
		scinitillatorXHalfLength = scinitillatorBox->GetXHalfLength();
		scinitillatorYHalfLength = scinitillatorBox->GetYHalfLength();
		scinitillatorZHalfLength = scinitillatorBox->GetZHalfLength();

		auto containerLV = logicVolStroe->GetVolume("ContainerLV");
		G4Box* containerBox = dynamic_cast<G4Box*>(containerLV->GetSolid());
		containerXHalfLength = containerBox->GetXHalfLength();
		containerYHalfLength = containerBox->GetYHalfLength();
		containerZHalfLength = containerBox->GetZHalfLength();

		G4LogicalVolume* logicWorld = logicVolStroe->GetVolume("WorldLV");
		G4Box* worldBox = dynamic_cast<G4Box*>(logicWorld->GetSolid());
		worldXHalfLength = worldBox->GetXHalfLength();
		worldYHalfLength = worldBox->GetYHalfLength();
		worldZHalfLength = worldBox->GetZHalfLength();

		auto moduleLV = logicVolStroe->GetVolume("ModuleLV");
		G4Box* moduleBox = dynamic_cast<G4Box*>(moduleLV->GetSolid());
		G4double moduleYHalfLength = moduleBox->GetYHalfLength();

		arraySize = (G4int)std::round(containerYHalfLength / moduleYHalfLength);
		distanceBetweenModules = (moduleYHalfLength - scinitillatorYHalfLength) * 2;

		CalculateReferencePoints refP = CalculateReferencePoints();
		referencePoints = refP.GetRefrencePoints();
	}

	if (sourceType != "NEUTRINO" && sourceType != "COSMICNEUTRON" && sourceType != "CRY" && sourceType != "He8" && sourceType != "Li9"
		&& sourceType != "MUON" && sourceType != "GPS" && sourceType != "GUN")
	{
		if (sourceType == "Am-Be-n" || sourceType == "Cs137g" || sourceType == "Co60g")
		{
			if (sourceType == "Am-Be-n")
			{
				fParticle = fNeutron;
				primaryParticleEnergy = EnergySampling(neutronEnergy, neutronCDFSpectrum);
			}
			else if (sourceType == "Cs137g")
			{
				fParticle = fGamma;
				primaryParticleEnergy = 0.66166 * MeV;
			}
			else if (sourceType == "Co60g")
			{
				fParticle = fGamma;
				if (G4UniformRand() > 0.5)
					primaryParticleEnergy = 1.1732 * MeV;
				else
					primaryParticleEnergy = 1.3325 * MeV;
			}
			G4double costheta = 2. * G4UniformRand() - 1.;
			G4double sintheta = sqrt(1 - pow(costheta, 2));
			G4double phi = twopi * G4UniformRand();
			directionVector = G4ThreeVector(sintheta * cos(phi), sintheta * sin(phi), costheta);

			fParticleGun->SetParticleDefinition(fParticle);
			fParticleGun->SetParticleEnergy(primaryParticleEnergy);
			fParticleGun->SetParticleMomentumDirection(directionVector);
		}


		if (fParticleGun->GetParticleDefinition() == G4Geantino::Geantino() || thisID != runID)
		{
			//G4IonTable* ionTable = G4IonTable::GetIonTable();
			if (sourceType == "Co60")
			{
				fParticle = ionTable->GetIon(27, 60);
				//fParticle = ionTable->GetIon(2, 8);
				//fParticle = ionTable->GetIon(3, 9);
				primaryParticleEnergy = 0. * MeV;
				directionVector = G4ThreeVector();
			}
			else if (sourceType == "Cs137")
			{
				fParticle = ionTable->GetIon(55, 137);
				primaryParticleEnergy = 0. * MeV;
				directionVector = G4ThreeVector();
			}
			else if (sourceType == "Na22")
			{
				fParticle = ionTable->GetIon(11, 22);
				primaryParticleEnergy = 0. * MeV;
				directionVector = G4ThreeVector();
			}
			else if (sourceType == "Am-Be")
			{
				fParticle = ionTable->GetIon(95, 241);
				primaryParticleEnergy = 0. * MeV;
				directionVector = G4ThreeVector();
			}
			fParticleGun->SetParticleDefinition(fParticle);
			fParticleGun->SetParticleEnergy(primaryParticleEnergy);
			fParticleGun->SetParticleMomentumDirection(directionVector);
			fParticleGun->SetParticleCharge(0. * eplus);
		}

		if (sourcePosition == "EDGE")
		{
			positionVector = G4ThreeVector(scinitillatorXHalfLength - distanceBetweenModules, 0., containerZHalfLength + 0.5 * mm);
		}
		else if(sourcePosition == "CENTER")
		{
			positionVector = G4ThreeVector(0., 0., containerZHalfLength + 0.5 * mm);
		}
		fParticleGun->SetParticlePosition(positionVector);
		fParticleGun->GeneratePrimaryVertex(anEvent);
	}
#ifdef __linux__
	else if (sourceType == "CRY")
	{
		if (InputState != 0)
		{
			//G4String* str = new G4String("CRY library was not successfully initialized");
			G4String str = G4String("CRY library was not successfully initialized");
			// G4Exception(*str);
			//G4Exception("PrimaryGeneratorAction", "1", RunMustBeAborted, *str);
			G4Exception("PrimaryGeneratorAction", "1", RunMustBeAborted, str);
		}
		G4String particleName;
		vect->clear();
		gen->genEvent(vect);

		////....debug output
		//G4cout << "\nEvent=" << anEvent->GetEventID() << " "
		//	   << "CRY generated nparticles=" << vect->size()
		//	   << G4endl;

		for (unsigned j = 0; j < vect->size(); j++)
		{
			//particleName = CRYUtils::partName((*vect)[j]->id());
			//if (particleName == "muon")
			//{
			//	std::ofstream outFile;
			//	outFile.open("fileName.txt", std::ios_base::app);
			//	outFile << (*vect)[j]->ke() << G4endl;
			//	outFile.close();
			//}
			////....debug output
			//G4cout << "  " << particleName << " "
			//	   << "charge=" << (*vect)[j]->charge() << " "
			//	   << std::setprecision(4)
			//	   << "energy (MeV)=" << (*vect)[j]->ke() * MeV << " "
			//	   << "pos (m)"
			//	   << G4ThreeVector((*vect)[j]->x(), (*vect)[j]->y(), (*vect)[j]->z())
			//	   << " "
			//	   << "direction cosines "
			//	   << G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w())
			//	   << " " << G4endl;

			fParticleGun->SetParticleDefinition(particleTable->FindParticle((*vect)[j]->PDGid()));
			fParticleGun->SetParticleEnergy((*vect)[j]->ke() * MeV);
			fParticleGun->SetParticlePosition(G4ThreeVector((*vect)[j]->x() * m, (*vect)[j]->y() * m, (*vect)[j]->z() * m + worldZHalfLength / 1.05));
			fParticleGun->SetParticleMomentumDirection(G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w()));
			fParticleGun->SetParticleTime((*vect)[j]->t());
			fParticleGun->GeneratePrimaryVertex(anEvent);
			delete (*vect)[j];
		}
	}
#endif
	else if (sourceType == "COSMICNEUTRON")
	{
		fParticleGun->SetParticleDefinition(fNeutron);
		primaryParticleEnergy = EnergySampling(neutronEnergy, neutronCDFSpectrum);
		fParticleGun->SetParticleEnergy(primaryParticleEnergy);
		SamplingForMuon(positionVector, directionVector);
		fParticleGun->SetParticlePosition(positionVector);
		fParticleGun->SetParticleMomentumDirection(directionVector);
		fParticleGun->GeneratePrimaryVertex(anEvent);
	}
	else if (sourceType == "He8" || sourceType == "Li9")
	{
		if (fParticleGun->GetParticleDefinition() == G4Geantino::Geantino() || thisID != runID)
		{
			//G4IonTable* ionTable = G4IonTable::GetIonTable();
			if (sourceType == "He8")
				fParticle = ionTable->GetIon(2, 8);
			else
				fParticle = ionTable->GetIon(3, 9);
		}

		fParticleGun->SetParticleDefinition(fParticle);
		PositionSampling(positionVector);
		fParticleGun->SetParticlePosition(positionVector);
		fParticleGun->SetParticleMomentumDirection(directionVector);
		fParticleGun->SetParticleCharge(0. * eplus);
		fParticleGun->SetParticleEnergy(0. * MeV);
		fParticleGun->GeneratePrimaryVertex(anEvent);
	}
	else if (sourceType == "MUON")
	{
		G4double randmu = G4UniformRand();
		if (randmu < 5. / 11) // mu+/mu- = 1.2
		{
			fParticle = fMuonN;
		}
		else
		{
			fParticle = fMuonP;
		}
		//primaryParticleEnergy = (0. * G4UniformRand() + 100.) * GeV; // 4. * GeV;
		primaryParticleEnergy = 4. * GeV;
		fParticleGun->SetParticleDefinition(fParticle);
		fParticleGun->SetParticleEnergy(primaryParticleEnergy);
		SamplingForMuon(positionVector, directionVector);
		fParticleGun->SetParticlePosition(positionVector);
		fParticleGun->SetParticleMomentumDirection(directionVector);
		fParticleGun->GeneratePrimaryVertex(anEvent);
	}
	else if (sourceType == "GUN")
	{
		if (sourcePosition == "EDGE")
		{
			positionVector = G4ThreeVector(scinitillatorXHalfLength - distanceBetweenModules, 0., containerZHalfLength + 0.5 * mm);
			fParticleGun->SetParticlePosition(positionVector);
		}
		else if (sourcePosition == "CENTER")
		{
			positionVector = G4ThreeVector(0., 0., containerZHalfLength + 0.5 * mm);
			fParticleGun->SetParticlePosition(positionVector);
		}
		else if (sourcePosition == "INSIDE")
		{
			SamplingForIBD(positionVector, directionVector);
			fParticleGun->SetParticleMomentumDirection(directionVector);
			fParticleGun->SetParticlePosition(positionVector);
		}
		fParticleGun->GeneratePrimaryVertex(anEvent);
	}
	else if (sourceType == "GPS")
	{
		fGPS->GeneratePrimaryVertex(anEvent);
	}
	else
	{
		// neutrinoPosition = UserDataInput::GetPositionOfNeutrino();

		fParticleGun->SetParticleDefinition(fNeutron);
		primaryParticleEnergy = EnergySampling(neutronEnergy, neutronCDFSpectrum);
		fParticleGun->SetParticleEnergy(primaryParticleEnergy);
		SamplingForIBD(positionVector, directionVector);
		fParticleGun->SetParticlePosition(positionVector);
		fParticleGun->SetParticleMomentumDirection(directionVector);
		fParticleGun->GeneratePrimaryVertex(anEvent);

		fParticleGun->SetParticleDefinition(fPositron);
		primaryParticleEnergy = EnergySampling(positronEnergy, positronCDFSpectrum);
		fParticleGun->SetParticleEnergy(primaryParticleEnergy);
		fParticleGun->SetParticlePosition(positionVector);
		SamplingForIBD(positionVector, directionVector);
		fParticleGun->SetParticleMomentumDirection(directionVector);
		fParticleGun->GeneratePrimaryVertex(anEvent);
	}
	runID = thisID;
}

G4double PANDASimPrimaryGeneratorAction::EnergySampling(std::vector<G4double>* energy, std::vector<G4double>* cdfSpectrum)
{
	G4double random = G4UniformRand();
	G4int i = 0;

	if (energy->size() < 3)
	{
		while (random > cdfSpectrum->at(i))
		{
			++i;
		}
		return energy->at(i) * MeV;
	}
	else
	{
		G4double primaryParticleEnergy = energy->at(0);
		while (random < cdfSpectrum->at(0) || random > *(cdfSpectrum->end() - 1))
		{
			random = G4UniformRand();
			i++;
			if (i > 1e6)
			{
				G4ExceptionDescription msg;
				msg << "Cannot sample primaryParticleEnergy. 1e6 times samplings have been tried." << G4endl;
				msg << "Break the while loop now and return energy[0].";
				G4Exception("PANDASimPrimaryGeneratorAction::EnergySampling()",
					"while (random < cdfSpectrum[0] || random > *(cdfSpectrum.end() - 1))", JustWarning, msg);
				return primaryParticleEnergy * MeV;
			}
		}
		try
		{
			SplineSpace::SplineInterface* sp;
			if (energy == neutronEnergy)
				sp = splineForNeutron;
			else //if (energy == positronEnergy)
				sp = splineForPositron;
			sp->SinglePointInterp(random, primaryParticleEnergy);
			//G4cout << "random: " << random << " energy: " << primaryParticleEnergy << G4endl;
		}
		catch (SplineSpace::SplineFailure sf)
		{
			G4cout << sf.GetMessage() << G4endl;
			//primaryParticleEnergy = 15.75 * keV;
		}
		return primaryParticleEnergy * MeV;
	}
}

void PANDASimPrimaryGeneratorAction::SamplingForIBD(G4ThreeVector& positionVector, G4ThreeVector& directionVector)
{
	G4double sourcePositionX = 0.;
	G4double sourcePositionY = 0.;
	G4double sourcePositionZ = 0.; // 2. * scinitillatorZHalfLength * G4UniformRand() - scinitillatorZHalfLength;

	G4double theta = pi * G4UniformRand();
	G4double phi = twopi * G4UniformRand();

	G4int randi;
	if (neutrinoPosition[0] < arraySize * arraySize)
	{
		randi = neutrinoPosition[0];

		if (neutrinoPosition[1] < 5)
			sourcePositionX = (G4UniformRand() + neutrinoPosition[1]) * scinitillatorXHalfLength / 5.; // 10. * cm;
		else
			sourcePositionX = 2. * scinitillatorXHalfLength * G4UniformRand() - scinitillatorXHalfLength;
	}
	else
	{
		randi = (G4int)(arraySize * arraySize * G4UniformRand());
		sourcePositionX = 2. * scinitillatorXHalfLength * G4UniformRand() - scinitillatorXHalfLength;
	}
	G4double minZ = referencePoints[randi][0] - scinitillatorZHalfLength;
	G4double maxZ = referencePoints[randi][0] + scinitillatorZHalfLength;
	G4double minY = referencePoints[randi][1] - scinitillatorYHalfLength;
	G4double maxY = referencePoints[randi][1] + scinitillatorYHalfLength;

	// G4cout << minX << ", " << maxZ << "; " << maxX << ", " << maxZ << G4endl;
	// G4cout << minX << ", " << minZ << "; " << maxX << ", " << minZ << G4endl;
	// getchar();

	sourcePositionY = minY + (maxY - minY) * G4UniformRand();
	sourcePositionZ = minZ + (maxZ - minZ) * G4UniformRand();

	G4double costheta = 2. * G4UniformRand() - 1.;
	G4double sintheta = sqrt(1 - pow(costheta, 2));

	directionVector = G4ThreeVector(sintheta * cos(phi), sintheta * sin(phi), costheta);
	positionVector = G4ThreeVector(sourcePositionX, sourcePositionY, sourcePositionZ);
}

void PANDASimPrimaryGeneratorAction::SamplingForMuon(G4ThreeVector& positionVector, G4ThreeVector& directionVector)
{
	G4double sourcePositionX = 0.;
	G4double sourcePositionY = 0.;
	G4double sourcePositionZ = 0.; // 2. * scinitillatorZHalfLength * G4UniformRand() - scinitillatorZHalfLength;

	G4double theta = pi * G4UniformRand();
	G4double phi = twopi * G4UniformRand();

	//sourcePositionX = 2. * containerXHalfLength * G4UniformRand() - containerXHalfLength;
	//sourcePositionY = 2. * containerYHalfLength * G4UniformRand() - containerYHalfLength;
	//sourcePositionZ = containerZHalfLength;
	sourcePositionX = 2. * worldXHalfLength / 1.05 * G4UniformRand() - worldXHalfLength / 1.05;
	sourcePositionY = 2. * worldYHalfLength / 1.05 * G4UniformRand() - worldYHalfLength / 1.05;
	sourcePositionZ = worldZHalfLength / 1.05;

	// for mu- / mu+, f(theta) = 4/pi * cos^2(theta) (pi/2, pi),替换抽样，抽样效率1/2
	G4double random = G4UniformRand();
	G4double candidateTheta = 0.5 * pi * (G4UniformRand() + 1);
	G4double cos2theta = pow(cos(candidateTheta), 2);
	G4int i = 0;
	while (random > cos2theta)
	{
		candidateTheta = 0.5 * pi * (G4UniformRand() + 1);
		cos2theta = pow(cos(candidateTheta), 2);
		i++;
		if (i > 1e6)
		{
			G4ExceptionDescription msg;
			msg << "Cannot sample theta. 1e6 times samplings have been tried." << G4endl;
			msg << "Break the while loop now.";
			G4Exception("PANDASimPrimaryGeneratorAction::SamplingForMuon()", "while (random > cos2theta)", JustWarning, msg);
			break;
		}
	}
	theta = candidateTheta;

	directionVector = G4ThreeVector(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
	positionVector = G4ThreeVector(sourcePositionX, sourcePositionY, sourcePositionZ);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PANDASimPrimaryGeneratorAction::PositionSampling(G4ThreeVector& positionVector)
{
	G4double sourcePositionX = 0.;
	G4double sourcePositionY = 0.;
	G4double sourcePositionZ = 0.;

	G4int randi = (G4int)(arraySize * arraySize * G4UniformRand());

	sourcePositionX = 2. * scinitillatorXHalfLength * G4UniformRand() - scinitillatorXHalfLength;

	G4double minZ = referencePoints[randi][0] - scinitillatorZHalfLength;
	G4double maxZ = referencePoints[randi][0] + scinitillatorZHalfLength;
	G4double minY = referencePoints[randi][1] - scinitillatorYHalfLength;
	G4double maxY = referencePoints[randi][1] + scinitillatorYHalfLength;

	sourcePositionY = minY + (maxY - minY) * G4UniformRand();
	sourcePositionZ = minZ + (maxZ - minZ) * G4UniformRand();

	positionVector = G4ThreeVector(sourcePositionX, sourcePositionY, sourcePositionZ);
}