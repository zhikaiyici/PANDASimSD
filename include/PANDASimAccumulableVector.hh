#ifndef PANDASimAccumulableVector_h
#define PANDASimAccumulableVector_h 1

#include "G4VAccumulable.hh"
#include "globals.hh"
#include <vector>

template<typename T>
std::vector<T> operator+=(std::vector<T>& a, const std::vector<T>& b)
{
    if (a.size() != b.size())
    {
        G4cout << "a.size() != b.size(), a will be returned." << G4endl;
        return a;
    }
    for (G4int i = 0; i < a.size(); ++i)
    {
        a[i] += b[i];
    }
    return a;
}

template <typename T>
class PANDASimAccumulableVector : public G4VAccumulable
{
public:
    PANDASimAccumulableVector() : G4VAccumulable(), fValue(0), fInitValue(0){};
    PANDASimAccumulableVector(G4int size, T initValue);
    ~PANDASimAccumulableVector() override = default;

    // Operators
    PANDASimAccumulableVector<T>& operator= (const PANDASimAccumulableVector<T>& rhs);
    PANDASimAccumulableVector<T>& operator=(PANDASimAccumulableVector<T>&& rhs) noexcept;
    PANDASimAccumulableVector<T>& operator+=(const PANDASimAccumulableVector<T>& rhs);

    PANDASimAccumulableVector<T>& operator= (const std::vector<T>& value);
    PANDASimAccumulableVector<T>& operator+=(const std::vector<T>& value);

    void Merge(const G4VAccumulable& other) final;
    void Reset() final;

    inline const std::vector<T>  GetValue() const { return fValue; }

private:
    std::vector<T> fValue;
    std::vector<T> fInitValue;
};

template<typename T>
inline PANDASimAccumulableVector<T>::PANDASimAccumulableVector(G4int size, T initValue)
    : G4VAccumulable(),
    fValue(size, initValue), fInitValue(size, initValue)
{}

template<typename T>
inline PANDASimAccumulableVector<T>& PANDASimAccumulableVector<T>::operator=(const PANDASimAccumulableVector<T>& rhs)
{
    // check assignment to self
    if (this == &rhs) return *this;

    // base class assignment
    G4VAccumulable::operator=(rhs);

    // this class data assignment
    fValue = rhs.fValue;
    fInitValue = rhs.fInitValue;

    return *this;
}

template<typename T>
inline PANDASimAccumulableVector<T>& PANDASimAccumulableVector<T>::operator=(PANDASimAccumulableVector<T>&& rhs) noexcept
{
    // check assignment to self
    if (this == &rhs) return *this;

    // base class assignment
    G4VAccumulable::operator=(rhs);

    // this class data assignment
    fValue = std::move(rhs.fValue);
    fInitValue = std::move(rhs.fInitValue);

    return *this;
}

template<typename T>
inline PANDASimAccumulableVector<T>& PANDASimAccumulableVector<T>::operator+=(const PANDASimAccumulableVector<T>& rhs)
{
    // only update the value from rhs
    fValue += rhs.fValue;
    return *this;
}

template<typename T>
inline PANDASimAccumulableVector<T>& PANDASimAccumulableVector<T>::operator=(const std::vector<T>& value)
{
    // only update the value
    fValue = value;
    return *this;
}

template<typename T>
inline PANDASimAccumulableVector<T>& PANDASimAccumulableVector<T>::operator+=(const std::vector<T>& value)
{
    // only update the value
    fValue += value;
    return *this;
}

template<typename T>
inline void PANDASimAccumulableVector<T>::Merge(const G4VAccumulable& other)
{
    const PANDASimAccumulableVector& otherPANDASimAccumulable = static_cast<const PANDASimAccumulableVector<T>&>(other);
    std::vector<T> localValue = otherPANDASimAccumulable.fValue;
    fValue += localValue;
}

template<typename T>
inline void PANDASimAccumulableVector<T>::Reset()
{
    fValue = fInitValue;
}

#endif
