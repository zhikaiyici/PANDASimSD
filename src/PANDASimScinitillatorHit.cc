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
//
/// \file PANDASimScinitillatorHit.cc
/// \brief Implementation of the PANDASimScinitillatorHit class

#include "PANDASimScinitillatorHit.hh"

G4ThreadLocal G4Allocator<PANDASimScinitillatorHit>* PANDASimScinHitAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimScinitillatorHit::PANDASimScinitillatorHit()
 : G4VHit(),
	fEdep(0.),fEdepH(0.),fEdepGd(0.), fEdepMu(0.), capTimeH(0.), decayTimeMu(0.), muTrack(0.), muEdep(0.),
	numHe8(0), numLi9(0)
{
	//ResizeVector(energyDeposit, UserDataInput::GetSizeOfArray());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimScinitillatorHit::~PANDASimScinitillatorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PANDASimScinitillatorHit::PANDASimScinitillatorHit(const PANDASimScinitillatorHit& right)
  : G4VHit()
{
	fEdep = right.fEdep;
	fEdepH = right.fEdepH;
	fEdepGd = right.fEdepGd;
	fEdepMu = right.fEdepMu;

	capTimeH = right.capTimeH;
	decayTimeMu = right.decayTimeMu;

	muTrack = right.muTrack;
	muEdep = right.muEdep;

	numHe8 = right.numHe8;
	numLi9 = right.numLi9;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const PANDASimScinitillatorHit& PANDASimScinitillatorHit::operator=(const PANDASimScinitillatorHit& right)
{
	fEdep = right.fEdep;
	fEdepH = right.fEdepH;
	fEdepGd = right.fEdepGd;
	fEdepMu = right.fEdepMu;

	capTimeH = right.capTimeH;
	decayTimeMu = right.decayTimeMu;

	muTrack = right.muTrack;
	muEdep = right.muEdep;

	numHe8 = right.numHe8;
	numLi9 = right.numLi9;

	return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PANDASimScinitillatorHit::operator==(const PANDASimScinitillatorHit& right) const
{
	return (this == &right) ? true : false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

