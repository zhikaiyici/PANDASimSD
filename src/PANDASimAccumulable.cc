
#include "PANDASimAccumulable.hh"
#include "UserDataInput.hh"

PANDASimAccumulable::PANDASimAccumulable(G4int as)
	: G4VAccumulable(), energyDeposit(0), betaKEHe8(0), betaKELi9(0), decayTimeHe8(0), decayTimeLi9(0), runCondition(""), neutrinoPosition({16, 5})
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

	numLi9 = std::vector<std::vector<G4int>>(arraySize, std::vector<G4int>(arraySize, 0));
	numHe8 = std::vector<std::vector<G4int>>(arraySize, std::vector<G4int>(arraySize, 0));
	numNeutron = std::vector<std::vector<G4int>>(arraySize, std::vector<G4int>(arraySize, 0));

	runCondition.clear();
	neutrinoPosition = {16, 5};
}

void PANDASimAccumulable::PushBetaKEHe8(G4double bke)
{
	betaKEHe8.push_back(bke);
}

void PANDASimAccumulable::PushBetaKELi9(G4double bke)
{
	betaKELi9.push_back(bke);
}

void PANDASimAccumulable::PushDecayTimeHe8(G4double dt)
{
	decayTimeHe8.push_back(dt);
}

void PANDASimAccumulable::PushDecayTimeLi9(G4double dt)
{
	decayTimeLi9.push_back(dt);
}

void PANDASimAccumulable::PushNeutronGenicTime(G4double t)
{
	neutronGenicTime.push_back(t);
}

void PANDASimAccumulable::AddNLi9(std::vector<std::vector<G4int> > n)
{
	for (int i = 0; i < numLi9[0].size(); ++i)
	{
		for (int j = 0; j < numLi9.size(); ++j)
		{
			numLi9[i][j] += n[i][j];
		}
	}
}

void PANDASimAccumulable::AddNHe8(std::vector<std::vector<G4int> > n)
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
