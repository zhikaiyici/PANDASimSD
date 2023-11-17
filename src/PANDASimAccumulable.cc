#include "PANDASimAccumulable.hh"

PANDASimAccumulable::PANDASimAccumulable() : G4VAccumulable(), energyDeposit(0)
{

}

PANDASimAccumulable::~PANDASimAccumulable()
{
}

void PANDASimAccumulable::Merge(const G4VAccumulable& other)
{
	const PANDASimAccumulable& otherPANDASimAccumulable = static_cast<const PANDASimAccumulable&>(other);
	list<double> edep = otherPANDASimAccumulable.energyDeposit;
	energyDeposit.merge(edep);

}

void PANDASimAccumulable::Reset()
{
	energyDeposit.clear();
}
