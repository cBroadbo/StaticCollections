//
// Created by Craig Home on 8/6/23.
//

#ifndef STATICCOLLECTIONS_VECTOR_H
#define STATICCOLLECTIONS_VECTOR_H

#include <cstdlib>
#include <stdexcept>
#include <exception>

template <class T>
class Vector {
public:
    typedef T                                     value_type;
    typedef value_type&                           reference;
    typedef const value_type&                     const_reference;
    typedef value_type*                           iterator;
    typedef const value_type*                     const_iterator;
    typedef value_type*                           pointer;
    typedef const value_type*                     const_pointer;
    typedef size_t                                size_type;
    typedef ptrdiff_t                             difference_type;

protected:
    T* mDataPtr;
    size_t mCapacity;
    size_t mCount{0};

public:
    Vector(T *ptr, size_t capacity): mDataPtr{ptr}, mCapacity{capacity} {
        if(!ptr) {
            throw std::invalid_argument("Vector constructor must have valid data ptr.");
        }
        if(!capacity) {
            throw std::invalid_argument("Zero capacity Vector not permitted.");
        }
    }

    virtual bool push_back(const T& elem) {
        if(mCount < mCapacity) {
            mDataPtr[mCount++] = elem;
            return true;
        }
        return false;
    }

    virtual void pop_back() {
        if(mCount > 0) {
            mCount--;
        }
    }
    const T* data() const { return mDataPtr; }
    [[nodiscard]] size_t size() const { return mCount; }
    [[nodiscard]] size_t capacity() const { return mCapacity; }
    [[nodiscard]] bool empty() const { return mCount == 0; }
    [[nodiscard]] bool full() const { return mCount == mCapacity; }

    void clear() { mCount = 0; }
    T& front() { if(empty()) { throw std::range_error("front() called on empty vector"); } return mDataPtr[0]; }
    T& back() { if(empty()) { throw std::range_error("back() called on empty vector"); } return mDataPtr[mCount-1]; }
    const T& front() const { if(empty()) { throw std::range_error("front() called on empty vector"); } return mDataPtr[0]; }
    const T& back() const { if(empty()) { throw std::range_error("back() called on empty vector"); } return mDataPtr[mCount-1]; }

    T& operator[](int index){ return const_cast<T&>((*const_cast<const Vector*>(this))[index]); }
    const T& operator[](int index) const {
        if(index<0 || index>=mCapacity) {
            throw std::out_of_range("Index out of range!");
        }
        return mDataPtr[index];
    }

    iterator begin() {return iterator(data());}
    const_iterator begin() const {return const_iterator(data());}
    iterator end() {return iterator(data() + mCount);}
    const_iterator end() const {return const_iterator(data() + mCount);}
};


#endif //STATICCOLLECTIONS_VECTOR_H
