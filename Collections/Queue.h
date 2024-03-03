//
// Created by Craig Home on 8/7/23.
//

#ifndef STATICCOLLECTIONS_QUEUE_H
#define STATICCOLLECTIONS_QUEUE_H

#include <cstdio>

template <class T>
class Queue {
public:
    virtual void clear() = 0;
    virtual bool push(const T& item) = 0;
    virtual bool push(const T* items, size_t count) = 0;
    virtual bool pop(T& item) = 0;
    virtual bool popElements(size_t count) = 0;
    virtual bool peek(T& item) = 0;

    [[nodiscard]] virtual bool empty() const = 0;
    [[nodiscard]] virtual bool full() const = 0;
    [[nodiscard]] virtual size_t size() const = 0;
    [[nodiscard]] virtual size_t capacity() const = 0;

};
#endif //STATICCOLLECTIONS_QUEUE_H
