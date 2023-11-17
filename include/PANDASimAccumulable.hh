#ifndef PANDASimAccumulable_h
#define PANDASimAccumulable_h 1

#include "G4VAccumulable.hh"
#include "globals.hh"
#include <list>
#include <vector>

using namespace std;

class PANDASimAccumulable : public G4VAccumulable
{
public:
    PANDASimAccumulable();
    ~PANDASimAccumulable();
    void Merge(const G4VAccumulable& other);
    void Reset();

private:
    list<G4double> energyDeposit;
};

#endif
