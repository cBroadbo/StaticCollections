//
// Created by Craig Home on 7/31/23.
//

#ifndef STATICVECTOR_H
#define STATICVECTOR_H

#include <stdexcept>
#include <initializer_list>
#include "Vector.h"

template <class T, size_t SIZE>
class StaticVector: public Vector<T> {
private:
    T mData[SIZE]{};

public:

    StaticVector(): Vector<T>(mData, SIZE) {}
    StaticVector(const std::initializer_list<T> &initializerList): Vector<T>(mData, SIZE) {
        if(initializerList.size() > SIZE) {
            throw std::runtime_error("number of initializer elements exceeds capacity.");
        }

        for(const T* elem = initializerList.begin(); elem != initializerList.end(); elem++) {
            this->push_back(*elem);
        }
    }
};


#endif //STATICVECTOR_H
