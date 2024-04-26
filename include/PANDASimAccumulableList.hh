#ifndef PANDASimAccumulableList_h
#define PANDASimAccumulableList_h 1

#include "G4VAccumulable.hh"
#include "globals.hh"
#include <list>

//using namespace std;

template <typename T>
class PANDASimAccumulableList : public G4VAccumulable
{
public:
    PANDASimAccumulableList() : G4VAccumulable(), fValue(0) {};
    ~PANDASimAccumulableList() override = default;

    void Merge(const G4VAccumulable& other) final;
    void Reset() final;

    inline const std::list<T>  GetValue() const { return fValue; }

    void PushValue(const T& aValue);

private:
    std::list<T> fValue;
};

template<typename T>
inline void PANDASimAccumulableList<T>::Merge(const G4VAccumulable& other)
{
    const PANDASimAccumulableList& otherPANDASimAccumulable = static_cast<const PANDASimAccumulableList<T>&>(other);

    std::list<T> localValue = otherPANDASimAccumulable.fValue;
    fValue.merge(localValue);
}

template<typename T>
inline void PANDASimAccumulableList<T>::Reset()
{
    fValue.clear();
}

template<typename T>
inline void PANDASimAccumulableList<T>::PushValue(const T& aValue)
{
    fValue.push_back(aValue);
}

#endif
