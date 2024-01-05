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
// $Id: PANDASimPrimaryGeneratorAction.hh 90623 2015-06-05 09:24:30Z gcosmo $
//
/// \file PANDASimPrimaryGeneratorAction.hh
/// \brief Definition of the PANDASimPrimaryGeneratorAction class

#ifndef PANDASimPrimaryGeneratorAction_h
#define PANDASimPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "PANDASimPrimaryGeneratorMessenger.hh"

#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4PhysicalConstants.hh"

#include "UserDataInput.hh"
#include "G4GeneralParticleSource.hh"
#include "G4IonTable.hh"

#ifdef __linux__
#include "RNGWrapper.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "CRYSetup.h"
#include "CRYGenerator.h"
#include "CRYParticle.h"
#include "CRYUtils.h"
#endif

//using namespace std;

class G4ParticleGun;
class G4Event;
class G4Box;
class G4LogicalVolumeStore;


/// The primary generator action class with particle gun.
///
/// The default kinematic is a 6 MeV gamma, randomly distribued 
/// in front of the phantom across 80% of the (X,Y) phantom size.

class PANDASimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
	PANDASimPrimaryGeneratorAction(const char *inputfile);
	virtual ~PANDASimPrimaryGeneratorAction();

	// method from the base class
	virtual void GeneratePrimaries(G4Event*);

	// method to access particle gun
	// const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

	inline void SetSourceType(G4String s) { sourceType = s; }
	inline const G4String GetSourceType() const { return sourceType; }

	inline void SetSourcePosition(G4String s) { sourcePosition = s; }
	inline const G4String GetSourcePosition() const { return sourcePosition; }

	inline void SetNeutrinoPosition(std::array<G4int, 2> np) { neutrinoPosition = np; }
	inline const std::array<G4int, 2> GetNeutrinoPosition() const { return neutrinoPosition; }

	inline void SetNeutronEnergy(std::vector<G4double> energy) { neutronEnergy = energy; }
	inline void SetNeutronCDF(std::vector<G4double> cdf) { neutronCDFSpectrum = cdf; }
	inline void SetPositronEnergy(std::vector<G4double> energy) { positronEnergy = energy; }
	inline void SetPositronCDF(std::vector<G4double> cdf) { positronCDFSpectrum = cdf; }

	void InputCRY();
    void UpdateCRY(std::string* MessInput);
    void CRYFromFile(G4String newValue);

private:
	G4ParticleGun* fParticleGun; // pointer a to G4 gun class
	//G4GeneralParticleSource* fParticleGun; // pointer a to G4 gun class
	//G4ParticleGun* fParticleGunP; // pointer a to G4 gun class

	G4ParticleTable* particleTable;
	G4IonTable* ionTable;

	G4ParticleDefinition* fParticle;
	G4ParticleDefinition* fPositron;
	G4ParticleDefinition* fNeutron;
	G4ParticleDefinition* fProton;
	G4ParticleDefinition* fMuonN;
	G4ParticleDefinition* fMuonP;
	G4ParticleDefinition* fGamma;

	G4double scinitillatorXHalfLength;
	G4double scinitillatorYHalfLength;
	G4double scinitillatorZHalfLength;
	G4double containerXHalfLength;
	G4double containerYHalfLength;
	G4double containerZHalfLength;

	G4double distanceBetweenModules;
	G4int arraySize;

	G4String sourceType;
	G4String sourcePosition;
	std::array<G4int, 2> neutrinoPosition;

	//G4double neutrinoPercentage;// = UserDataInput::GetNeutrinoPercentage();

	std::vector<G4double> neutronEnergy;// = userData.GetNeutronEnergy();
	std::vector<G4double> neutronCDFSpectrum;// = userData.GetNeutronCDFSpectrum();
	std::vector<G4double> positronEnergy;// = userData.GetPositronEnergy();
	std::vector<G4double> positronCDFSpectrum;// = userData.GetPositronCDFSpectrum();

	std::vector<std::array<G4double, 2> > referencePoints;

	G4LogicalVolumeStore* logicVolStroe;

	G4int runID;

	void SamplingForMuon(G4ThreeVector& positionVector, G4ThreeVector& directionVector); //位置和方向抽样函数
	void SamplingForIBD(G4ThreeVector& positionVector, G4ThreeVector& directionVector); //位置和方向抽样函数
	void PositionSampling(G4ThreeVector& positionVector); //位置抽样函数
	G4double EnergySampling(std::vector<G4double> energy, std::vector<G4double> cdfSpectrum); //能量抽样函数

#ifdef __linux__
	G4String CRYDataPath;
	std::vector<CRYParticle *> *vect; // vector of generated particles
	// G4ParticleTable* particleTable;
	CRYGenerator *gen;
	G4int InputState;
	PrimaryGeneratorMessenger *gunMessenger;
#endif

	PANDASimPrimaryGeneratorMessenger* sourceMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
