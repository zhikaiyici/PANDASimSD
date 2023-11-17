
#include "CalculateReferencePoints.hh"
#include "UserDataInput.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"

//vector<array<G4double, 2> > CalculateReferencePoints::referencePoints = *(new vector<array<G4double, 2> >);

CalculateReferencePoints::CalculateReferencePoints() 
{
	Calculate();
}

CalculateReferencePoints::~CalculateReferencePoints()
{
}

void CalculateReferencePoints::Calculate()
{
	G4LogicalVolumeStore* logicVolStroe = G4LogicalVolumeStore::GetInstance();
	G4double containerXHalfLength = 0.;
	G4double containerYHalfLength = 0.;
	G4double containerZHalfLength = 0.;
	G4double moduleXHalfLength = 0.;
	G4double moduleYHalfLength = 0.;
	G4double moduleZHalfLength = 0.;
	G4double scinitillatorXHalfLength = 0.;
	G4double scinitillatorYHalfLength = 0.;
	G4double scinitillatorZHalfLength = 0.;

	G4LogicalVolume* logicContainer = logicVolStroe->GetVolume("ContainerLV");
	G4Box* containerBox = dynamic_cast<G4Box*>(logicContainer->GetSolid());
	containerXHalfLength = containerBox->GetXHalfLength();
	containerYHalfLength = containerBox->GetYHalfLength();
	containerZHalfLength = containerBox->GetZHalfLength();

	G4LogicalVolume* logicModule = logicVolStroe->GetVolume("ModuleLV");
	G4Box* moduleBox = dynamic_cast<G4Box*>(logicModule->GetSolid());
	moduleXHalfLength = moduleBox->GetXHalfLength();
	moduleYHalfLength = moduleBox->GetYHalfLength();
	moduleZHalfLength = moduleBox->GetZHalfLength();

	G4LogicalVolume* logicPlasticScintillator = logicVolStroe->GetVolume("PlasticScintillatorLV");
	G4Box* scinitillatorBox = dynamic_cast<G4Box*>(logicPlasticScintillator->GetSolid());
	scinitillatorXHalfLength = scinitillatorBox->GetXHalfLength();
	scinitillatorYHalfLength = scinitillatorBox->GetYHalfLength();
	scinitillatorZHalfLength = scinitillatorBox->GetZHalfLength();

	G4int arraySize = UserDataInput::GetSizeOfArray();
	//referencePoints.resize(arraySize);
	for (G4int i = 0; i < arraySize; i++)
	{
		for (G4int j = 0; j < arraySize; j++)
		{
			G4double refZ = -containerZHalfLength + moduleZHalfLength * (2. * i + 1);
			G4double refY = -containerYHalfLength + moduleYHalfLength * (2. * j + 1);
			referencePoints.push_back({ refZ, refY });
			//G4cout << "refpoint: {" << refZ << ", " << refY << " }" << G4endl;
			//getchar();
		}
	}
}
