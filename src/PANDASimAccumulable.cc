
#include "PANDASimAccumulable.hh"
#include "UserDataInput.hh"

PANDASimAccumulable::PANDASimAccumulable(G4int as)
	: G4VAccumulable(), 
	energyDeposit(0), betaKEHe8(0), betaKELi9(0), decayTimeHe8(0), decayTimeLi9(0),
	neutronGenicTime(0), neutronKineticEnergy(0), neutronGT(0), neutronKE(0),
	capTimeH(0), capTimeGd(0),
	runCondition(""), neutrinoPosition({16, 5})
{
	arraySize = as;// UserDataInput::GetSizeOfArray();
	numLi9 = std::vector<std::vector<G4int>>(arraySize, std::vector<G4int>(arraySize, 0));
	numHe8 = std::vector<std::vector<G4int>>(arraySize, std::vector<G4int>(arraySize, 0));
	numNeutron = std::vector<std::vector<G4int>>(arraySize, std::vector<G4int>(arraySize, 0));
}

PANDASimAccumulable::~PANDASimAccumulable()
{
}

void PANDASimAccumulable::Merge(const G4VAccumulable& other)
{
	const PANDASimAccumulable& otherPANDASimAccumulable = static_cast<const PANDASimAccumulable&>(other);

	std::list<G4double> edep = otherPANDASimAccumulable.energyDeposit;
	energyDeposit.merge(edep);

	std::list<G4double> bkeHe8 = otherPANDASimAccumulable.betaKEHe8;
	betaKEHe8.merge(bkeHe8);

	std::list<G4double> bkeLi9 = otherPANDASimAccumulable.betaKELi9;
	betaKELi9.merge(bkeLi9);

	std::list<G4double> dtHe8 = otherPANDASimAccumulable.decayTimeHe8;
	decayTimeHe8.merge(dtHe8);

	std::list<G4double> dtLi9 = otherPANDASimAccumulable.decayTimeLi9;
	decayTimeLi9.merge(dtLi9);

	std::list<G4double> nGT = otherPANDASimAccumulable.neutronGT;
	neutronGT.merge(nGT);

	std::list<G4double> nKE = otherPANDASimAccumulable.neutronKE;
	neutronKE.merge(nKE);

	std::list<G4double> capTH = otherPANDASimAccumulable.capTimeH;
	capTimeH.merge(capTH);

	std::list<G4double> capTGd = otherPANDASimAccumulable.capTimeGd;
	capTimeGd.merge(capTGd);

	std::list<std::vector<std::vector<G4double>>> nGenicT = otherPANDASimAccumulable.neutronGenicTime;
	neutronGenicTime.merge(nGenicT);

	std::list<std::vector<std::vector<G4double>>> nKineticE = otherPANDASimAccumulable.neutronKineticEnergy;
	neutronKineticEnergy.merge(nKineticE);

	for (int i = 0; i < numLi9[0].size(); ++i) 
	{
		for (int j = 0; j < numLi9.size(); ++j) 
		{
			numLi9[i][j] += otherPANDASimAccumulable.numLi9[i][j];
			numHe8[i][j] += otherPANDASimAccumulable.numHe8[i][j];
			numNeutron[i][j] += otherPANDASimAccumulable.numNeutron[i][j];
		}
	}

	runCondition = otherPANDASimAccumulable.runCondition;
	neutrinoPosition = otherPANDASimAccumulable.neutrinoPosition;

}

void PANDASimAccumulable::Reset()
{
	energyDeposit.clear();
	betaKEHe8.clear();
	betaKELi9.clear();
	decayTimeHe8.clear();
	decayTimeLi9.clear();
	neutronGenicTime.clear();
	neutronKineticEnergy.clear();

	neutronGT.clear();
	neutronKE.clear();

	capTimeH.clear();
	capTimeGd.clear();

	numLi9 = std::vector<std::vector<G4int>>(arraySize, std::vector<G4int>(arraySize, 0));
	numHe8 = std::vector<std::vector<G4int>>(arraySize, std::vector<G4int>(arraySize, 0));
	numNeutron = std::vector<std::vector<G4int>>(arraySize, std::vector<G4int>(arraySize, 0));

	runCondition.clear();
	neutrinoPosition = {16, 5};
}

void PANDASimAccumulable::PushBetaKEHe8(const G4double& bke)
{
	betaKEHe8.push_back(bke);
}

void PANDASimAccumulable::PushBetaKELi9(const G4double& bke)
{
	betaKELi9.push_back(bke);
}

void PANDASimAccumulable::PushDecayTimeHe8(const G4double& dt)
{
	decayTimeHe8.push_back(dt);
}

void PANDASimAccumulable::PushDecayTimeLi9(const G4double& dt)
{
	decayTimeLi9.push_back(dt);
}

void PANDASimAccumulable::PushNeutronGenicTime(const std::vector<std::vector<G4double>>& t)
{
	neutronGenicTime.push_back(t);
}

void PANDASimAccumulable::PushNeutronGenicTime(const G4double& t)
{
	neutronGT.push_back(t);
}

void PANDASimAccumulable::PushNeutronKE(const std::vector<std::vector<G4double>>& ke)
{
	neutronKineticEnergy.push_back(ke);
}

void PANDASimAccumulable::PushNeutronKE(const G4double& ke)
{
	neutronKE.push_back(ke);
}

void PANDASimAccumulable::PushCapTimeH(const G4double& ct)
{
	capTimeH.push_back(ct);
}

void PANDASimAccumulable::PushCapTimeGd(const G4double& ct)
{
	capTimeGd.push_back(ct);
}

void PANDASimAccumulable::AddNLi9(std::vector<std::vector<G4int>> n)
{
	for (int i = 0; i < numLi9[0].size(); ++i)
	{
		for (int j = 0; j < numLi9.size(); ++j)
		{
			numLi9[i][j] += n[i][j];
		}
	}
}

void PANDASimAccumulable::AddNHe8(std::vector<std::vector<G4int>> n)
{
	for (int i = 0; i < numHe8[0].size(); ++i)
	{
		for (int j = 0; j < numHe8.size(); ++j)
		{
			numHe8[i][j] += n[i][j];
		}
	}
}

void PANDASimAccumulable::AddNNeurtron(std::vector<std::vector<G4int>> n)
{
	for (int i = 0; i < numNeutron[0].size(); ++i)
	{
		for (int j = 0; j < numNeutron.size(); ++j)
		{
			numNeutron[i][j] += n[i][j];
		}
	}
}
