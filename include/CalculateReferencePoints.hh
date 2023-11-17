#ifndef CalculateReferencePoints_h
#define CalculateReferencePoints_h 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

#include<vector>
#include<array>

using namespace std;
class CalculateReferencePoints
{
public:
	CalculateReferencePoints();
	~CalculateReferencePoints();

	//static 
		inline vector<array<G4double, 2> > GetRefrencePoints() const { return referencePoints; };

private:
	//static 
		vector<array<G4double, 2> > referencePoints;

	//static 
		void Calculate();
};



#endif