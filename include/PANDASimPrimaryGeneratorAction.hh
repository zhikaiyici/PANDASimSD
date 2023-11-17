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
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4PhysicalConstants.hh"

#include "UserDataInput.hh"
#include "G4GeneralParticleSource.hh"


using namespace std;

class G4ParticleGun;
class G4Event;
class G4Box;

/// The primary generator action class with particle gun.
///
/// The default kinematic is a 6 MeV gamma, randomly distribued 
/// in front of the phantom across 80% of the (X,Y) phantom size.

class PANDASimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
	PANDASimPrimaryGeneratorAction();
	virtual ~PANDASimPrimaryGeneratorAction();

	// method from the base class
	virtual void GeneratePrimaries(G4Event*);

	// method to access particle gun
	// const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

private:
	G4ParticleGun* fParticleGun; // pointer a to G4 gun class
	//G4GeneralParticleSource* fParticleGun; // pointer a to G4 gun class
	//G4ParticleGun* fParticleGunP; // pointer a to G4 gun class
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
	G4String sourceType;
	G4String sourcePosition;
	G4int arraySize;
	array<G4int, 2> neutrinoPosition;
	G4double neutrinoPercentage;// = UserDataInput::GetNeutrinoPercentage();
	vector<G4double> neutronEnergy;// = userData.GetNeutronEnergy();
	vector<G4double> neutronCDFSpectrum;// = userData.GetNeutronCDFSpectrum();
	vector<G4double> positronEnergy;// = userData.GetPositronEnergy();
	vector<G4double> positronCDFSpectrum;// = userData.GetPositronCDFSpectrum();

	vector<array<G4double, 2> > referencePoints;

	void SamplingForMuon(G4ThreeVector& positionVector, G4ThreeVector& directionVector); //位置和方向抽样函数
	void SamplingForIBD(G4ThreeVector& positionVector, G4ThreeVector& directionVector); //位置和方向抽样函数
	G4double EnergySampling(vector<G4double> energy, vector<G4double> cdfSpectrum); //能量抽样函数
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
