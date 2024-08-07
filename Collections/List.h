#ifndef LIST_H
#define LIST_H

#include <array>
#include <cstdlib>
#include <iterator>
#include <cstring>

template <class T>
class List {

public:
    typedef T value_type;

    [[nodiscard]] virtual bool empty() const = 0;
    [[nodiscard]] virtual bool isFull() const = 0;
    [[nodiscard]] virtual std::size_t size() const = 0;
    virtual void push_back(const T &elem) = 0;
    virtual void pop_back() = 0;
    virtual void push_front(const T &elem) = 0;
    virtual void pop_front() = 0;
    virtual void clear() = 0;
    virtual void erase(const T &elem) = 0;
    virtual void eraseAtIndex(std::size_t index) = 0;
    virtual const T &front() const = 0;
    virtual const T &back() const = 0;
    virtual T &front() = 0;
    virtual T &back() = 0;
};

#endif //LIST_H