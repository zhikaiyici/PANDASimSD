#include"PANDASimPhotocathodeHit.hh"

G4ThreadLocal G4Allocator<PANDASimPhotocathodeHit>* PANDASimPhotocathodeHitAllocator = 0;

PANDASimPhotocathodeHit::PANDASimPhotocathodeHit()
	:G4VHit(), 
	fCalPheNum(0.),fCalPheNumH(0.),fCalPheNumGd(0.), fCalPheNumMu(0.), fCalPheNumDecay(0.)
{
}

PANDASimPhotocathodeHit::PANDASimPhotocathodeHit(const PANDASimPhotocathodeHit& right)
	:G4VHit()
{
	fCalPheNum = right.fCalPheNum;
	fCalPheNumH = right.fCalPheNumH;
	fCalPheNumGd = right.fCalPheNumGd;
	fCalPheNumMu = right.fCalPheNumMu;
	fCalPheNumDecay = right.fCalPheNumDecay;
}

PANDASimPhotocathodeHit::~PANDASimPhotocathodeHit()
{
}

const PANDASimPhotocathodeHit& PANDASimPhotocathodeHit::operator=(const PANDASimPhotocathodeHit& right)
{
	fCalPheNum = right.fCalPheNum;
	fCalPheNumH = right.fCalPheNumH;
	fCalPheNumGd = right.fCalPheNumGd;
	fCalPheNumMu = right.fCalPheNumMu;
	fCalPheNumDecay = right.fCalPheNumDecay;
	return *this;
}

G4bool PANDASimPhotocathodeHit::operator==(const PANDASimPhotocathodeHit& right) const
{
	return (this == &right) ? true : false;
}
