// CDynArray
// Basic dynamic array allowing to add, remove and access elements very quickly
// Note: Removing an element changes the order behind this element
//
// Author: Thomas Brox

#ifndef CDynArrayH
#define CDynArrayH

template <class T>
class CDynArray {
public:
  // constructor
  inline CDynArray(const int aInitialVolume = 256);
  CDynArray(const CDynArray<T>& aCopyFrom);
  // destructor
  inline ~CDynArray();
  // Adds new element
  inline void add(T aItem);
  // Removes element, last element is moved to the index position!!
  inline void remove(int aIndex);
  // Removes all elements
  inline void clear();

  // Access to an element
  inline T& operator[](int aIndex);
  // Copy operator
  void operator=(const CDynArray<T>& aCopyFrom);
  // Supplements the data of another list
  void operator+=(const CDynArray<T>& aArray);
  // Returns the average of all elements
  T average();
  // Returns the variance over all elements
  T variance();
  // Returns number of elements
  inline int size();
protected:
  int mSize;
  int mVolume;
  T* mData;
  void increaseVolume();
};

class EDynArrayOverflow {
};

// I M P L E M E N T A T I O N ----------------------------------------------
// P U B L I C --------------------------------------------------------------

// constructor
template <class T>
inline CDynArray<T>::CDynArray(int aInitialVolume) {
  mSize = 0;
  mVolume = aInitialVolume;
  mData = new T[aInitialVolume];
}

template <class T>
CDynArray<T>::CDynArray(const CDynArray<T>& aCopyFrom) {
  mVolume = aCopyFrom.mVolume;
  mData = new T[mVolume];
  mSize = aCopyFrom.mSize;
  for (int i = 0; i < mSize; i++)
    mData[i] = aCopyFrom.mData[i];
}

// destructor
template <class T>
inline CDynArray<T>::~CDynArray() {
  delete[] mData;
}

// add
template <class T>
inline void CDynArray<T>::add(T aItem) {
  // No place left -> get bigger memory block and copy data to that block
  if (mSize == mVolume) increaseVolume();
  mData[mSize] = aItem;
  mSize++;
}

// remove
template <class T>
inline void CDynArray<T>::remove(int aIndex) {
  mSize--;
  mData[aIndex] = mData[mSize];
}

// clear
template <class T>
inline void CDynArray<T>::clear() {
  mSize = 0;
}

// operator []
template <class T>
inline T& CDynArray<T>::operator[](int aIndex) {
  return mData[aIndex];
}

// operator =
template <class T>
void CDynArray<T>::operator=(const CDynArray<T>& aCopyFrom) {
  if (aCopyFrom.mSize > mVolume) {
    delete[] mData;
    mVolume = aCopyFrom.mVolume;
    mData = new T[mVolume];
  }
  mSize = aCopyFrom.mSize;
  for (int i = 0; i < mSize; i++)
    mData[i] = aCopyFrom.mData[i];
}

// operator +=
template <class T>
void CDynArray<T>::operator+=(const CDynArray<T>& aArray) {
  if (mSize+aArray.mSize > mVolume) {
    T* aOldData = mData;
    mVolume = mSize+aArray.mSize;
    mData = new T[mVolume];
    for (int i = 0; i < mSize; i++)
      mData[i] = aOldData[i];
    delete[] aOldData;
  }
  for (int i = 0; i < aArray.mSize; i++)
    mData[i+mSize] = aArray.mData[i];
  mSize += aArray.mSize;
}

// average
template <class T>
T CDynArray<T>::average() {
  T aSum = 0;
  for (int i = 0; i < mSize; i++)
    aSum += mData[i];
  return aSum / mSize;
}

// variance
template <class T>
T CDynArray<T>::variance() {
  T aSum = 0;
  T aAvg = average();
  for (int i = 0; i < mSize; i++)
    aSum += (mData[i]-aAvg)*(mData[i]-aAvg);
  return aSum / mSize;
}

// size
template <class T>
inline int CDynArray<T>::size() {
  return mSize;
}

// P R I V A T E --------------------------------------------------------

// increaseVolume
template <class T>
void CDynArray<T>::increaseVolume() {
  T* aOldData = mData;
  mVolume *= 2;
  mData = new T[mVolume];
  for (int i = 0; i < mSize; i++)
    mData[i] = aOldData[i];
  delete[] aOldData;
}

#endif
