#include "PANDASimGdFilmHit.hh"

G4ThreadLocal G4Allocator<PANDASimGdFilmHit>* PANDASimGdFilmHitAllocator = 0;

PANDASimGdFilmHit::PANDASimGdFilmHit()
	: G4VHit(), 
	capTimeGd(0.)
{
}

PANDASimGdFilmHit::PANDASimGdFilmHit(const PANDASimGdFilmHit& right) : G4VHit()
{
	capTimeGd = right.capTimeGd;
}

PANDASimGdFilmHit::~PANDASimGdFilmHit()
{
}

const PANDASimGdFilmHit& PANDASimGdFilmHit::operator=(const PANDASimGdFilmHit& right)
{
	capTimeGd = right.capTimeGd;
	return *this;
}

inline G4bool PANDASimGdFilmHit::operator==(const PANDASimGdFilmHit& right) const
{
	return (this == &right) ? true : false;
}
