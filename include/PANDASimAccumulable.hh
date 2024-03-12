#ifndef PANDASimAccumulable_h
#define PANDASimAccumulable_h 1

#include "G4VAccumulable.hh"
#include "globals.hh"
#include <list>
#include <vector>
#include <array>

//using namespace std;

class PANDASimAccumulable : public G4VAccumulable
{
public:
    PANDASimAccumulable(G4int as);
    ~PANDASimAccumulable();
    void Merge(const G4VAccumulable& other);
    void Reset();

    void PushBetaKEHe8(const G4double& bke);
    void PushBetaKELi9(const G4double& bke);
    void PushDecayTimeHe8(const G4double& dt);
    void PushDecayTimeLi9(const G4double& dt);

    void PushEdepDecay(const std::vector<std::vector<G4double>>& de);
    void PushModuleCalPhDecay(const std::vector<std::vector<std::vector<G4double>>>& nCalPhVec);

    void PushNeutronGenicTime(const std::vector<std::vector<G4double>>& t);
    void PushNeutronGenicTime(const G4double& t);
    void PushNeutronKE(const std::vector<std::vector<G4double>>& ke);
    void PushNeutronKE(const G4double& ke);
    void PushCapTimeH(const G4double& ct);
    void PushCapTimeGd(const G4double& ct);

    void AddNLi9(std::vector<std::vector<G4int>> n);
    void AddNHe8(std::vector<std::vector<G4int>> n);
    void AddNNeurtron(std::vector<std::vector<G4int>> n);

    inline void SetRunCondition(G4String rc) { runCondition = rc; };
    inline void SetArraySize(G4int as) { arraySize = as; };

    inline G4String GetRunCondition() const { return runCondition; }
    inline G4int GetArraySize() const { return arraySize; }

    inline std::list<G4double> GetBetaKEHe8() const { return betaKEHe8; }
    inline std::list<G4double> GetBetaKELi9() const { return betaKELi9; }
    inline std::list<G4double> GetDecayTimeHe8() const { return decayTimeHe8; }
    inline std::list<G4double> GetDecayTimeLi9() const { return decayTimeLi9; }
    inline std::list<G4double> GetNeutronGT() const { return neutronGT; }
    inline std::list<G4double> GetNeutronKE() const { return neutronKE; }
    inline std::list<G4double> GetCapTimeH() const { return capTimeH; }
    inline std::list<G4double> GetCapTimeGd() const { return capTimeGd; }

    inline std::list<std::vector<std::vector<G4double>>> GetEdepDecay() const { return edepDecay; }
    inline std::list<std::vector<std::vector<std::vector<G4double>>>> GetModuleCalPhDecay() const { return moduleCalPhDecay; }

    inline std::list<std::vector<std::vector<G4double>>> GetNeutronGenicTime() const { return neutronGenicTime; }
    inline std::list<std::vector<std::vector<G4double>>> GetNeutronKineticEnergy() const { return neutronKineticEnergy; }

    inline std::vector<std::vector<G4int> > GetNLi9() { return numLi9; }
    inline std::vector<std::vector<G4int> > GetNHe8() { return numHe8; }
    inline std::vector<std::vector<G4int> > GetNNeutron() { return numNeutron; }

private:
    std::list<G4double> energyDeposit;
    std::list<G4double> decayTimeHe8;
    std::list<G4double> decayTimeLi9;
    std::list<G4double> betaKEHe8;
    std::list<G4double> betaKELi9;

    std::list<G4double> neutronGT;
    std::list<G4double> neutronKE;

    std::list<G4double> capTimeH;
    std::list<G4double> capTimeGd;

    std::list<std::vector<std::vector<G4double>>> edepDecay;
    std::list<std::vector<std::vector<std::vector<G4double>>>> moduleCalPhDecay;

    std::list<std::vector<std::vector<G4double>>> neutronGenicTime;
    std::list<std::vector<std::vector<G4double>>> neutronKineticEnergy;

    std::vector<std::vector<G4int>>  numLi9;
    std::vector<std::vector<G4int>>  numHe8;
    std::vector<std::vector<G4int>>  numNeutron;

    G4int arraySize;

    G4String runCondition;
    std::array<G4int, 2> neutrinoPosition;
};

#endif
