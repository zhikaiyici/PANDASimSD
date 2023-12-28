#ifndef PANDASimAccumulable_h
#define PANDASimAccumulable_h 1

#include "G4VAccumulable.hh"
#include "globals.hh"
#include <list>
#include <vector>

//using namespace std;

class PANDASimAccumulable : public G4VAccumulable
{
public:
    PANDASimAccumulable();
    ~PANDASimAccumulable();
    void Merge(const G4VAccumulable& other);
    void Reset();

    void PushBetaKEHe8(G4double bke);
    void PushBetaKELi9(G4double bke);
    void PushDecayTimeHe8(G4double dt);
    void PushDecayTimeLi9(G4double dt);

    void AddNLi9(std::vector<std::vector<G4int> > n);
    void AddNHe8(std::vector<std::vector<G4int> > n);

    inline std::list<G4double> GetBetaKEHe8() const { return betaKEHe8; }
    inline std::list<G4double> GetBetaKELi9() const { return betaKELi9; }
    inline std::list<G4double> GetDecayTimeHe8() const { return decayTimeHe8; }
    inline std::list<G4double> GetDecayTimeLi9() const { return decayTimeLi9; }

    inline std::vector<std::vector<G4int> > GetNLi9() { return numLi9; }
    inline std::vector<std::vector<G4int> > GetNHe8() { return numHe8; }

private:
    std::list<G4double> energyDeposit;
    std::list<G4double> decayTimeHe8;
    std::list<G4double> decayTimeLi9;
    std::list<G4double> betaKEHe8;
    std::list<G4double> betaKELi9;

    std::vector<std::vector<G4int> >  numLi9;
    std::vector<std::vector<G4int> >  numHe8;

    G4int arraySize;
};

#endif
