#ifndef LIST_H
#define LIST_H

#include <array>
#include <cstdlib>
#include <iterator>
#include <cstring>

template <class T>
class List {

public:
    class iteratorBase {
    public:
        typedef iteratorBase self_type;
        typedef T &reference;
        typedef T *pointer;

        virtual void next() = 0;
        virtual reference operator*() = 0;
        virtual pointer operator->() = 0;
        virtual bool operator==(const self_type &rhs) const = 0;
        virtual bool operator!=(const self_type &rhs) const = 0;
    };

    class iterator {
    public:
        typedef iterator self_type;
        typedef T value_type;
        typedef T& reference;
        typedef T* pointer;
        typedef std::forward_iterator_tag iterator_category;
        typedef int difference_type;

        iterator(const iterator &rhs) {
            std::memcpy(mImplStorage, rhs.mImplStorage, sizeof(mImplStorage));
            mImplPtr = (iteratorBase*)mImplStorage;
        }

        iterator &operator=(const iterator &rhs) {
            std::memcpy(mImplStorage, rhs.mImplStorage, sizeof(mImplStorage));
            mImplPtr = (iteratorBase*)mImplStorage;
            return *this;
        }

        explicit iterator(iteratorBase &impl, std::size_t implSize) {
            if(implSize > sizeof(mImplStorage)) {
                throw std::invalid_argument("Insufficient storage size for iterator.");
            }
            std::memcpy(mImplStorage, (const void*)&impl, implSize);
        }
        self_type operator++() { self_type i = *this; mImplPtr->next(); return i; }
        const self_type operator++(int junk) { mImplPtr->next(); return *this; }
        reference operator*() { return mImplPtr->operator*(); }
        pointer operator->() { return mImplPtr->operator->(); }
        bool operator==(const self_type &rhs) const { return *mImplPtr == *rhs.mImplPtr; }
        bool operator!=(const self_type &rhs) const { return !(*this == rhs); }

        /**
         * For internal use only!  Please don't use this directly.
         * @return pointer to the implementation
         */
        iteratorBase *getImpl() const { return mImplPtr; }

    private:
        iteratorBase * mImplPtr{(iteratorBase*)mImplStorage};
        std::uint_fast64_t mImplStorage[2]{};
    };

    class const_iterator {
    public:
        typedef const_iterator self_type;
        typedef T value_type;
        typedef T& reference;
        typedef T* pointer;
        typedef std::forward_iterator_tag iterator_category;
        typedef int difference_type;

        const_iterator(const const_iterator &rhs) {
            std::memcpy(mImplStorage, rhs.mImplStorage, sizeof(mImplStorage));
            mImplPtr = (iteratorBase*)mImplStorage;
        }

        const_iterator &operator=(const const_iterator &rhs) {
            std::memcpy(mImplStorage, rhs.mImplStorage, sizeof(mImplStorage));
            mImplPtr = (iteratorBase*)mImplStorage;
            return *this;
        }

        explicit const_iterator(iteratorBase &impl, std::size_t implSize) {
            if(implSize > sizeof(mImplStorage)) {
                throw std::invalid_argument("Insufficient storage size for iterator.");
            }
            std::memcpy(mImplStorage, (const void*)&impl, implSize);
        }
        self_type operator++() { self_type i = *this; mImplPtr->next(); return i; }
        const self_type operator++(int junk) { mImplPtr->next(); return *this; }
        reference operator*() const { return mImplPtr->operator*(); }
        pointer operator->() const { return mImplPtr->operator->(); }
        bool operator==(const self_type &rhs) const { return *mImplPtr == *rhs.mImplPtr; }
        bool operator!=(const self_type &rhs) const { return !(*this == rhs); }

        /**
         * For internal use only!  Please don't use this directly.
         * @return pointer to the implementation
        */
        [[nodiscard]] iteratorBase *getImpl() const { return mImplPtr; }

    private:
        iteratorBase * mImplPtr{(iteratorBase*)mImplStorage};
        std::uint_fast64_t mImplStorage[2]{};
    };

public:
    typedef T value_type;

    [[nodiscard]] virtual bool empty() const = 0;
    [[nodiscard]] virtual bool isFull() const = 0;
    [[nodiscard]] virtual std::size_t size() const = 0;
    [[nodiscard]] virtual std::size_t max_size() const = 0;
    virtual void push_back(const T &elem) = 0;
    virtual void pop_back() = 0;
    virtual void push_front(const T &elem) = 0;
    virtual void pop_front() = 0;
    virtual void clear() = 0;
    virtual void erase(const T &elem) = 0;
    virtual iterator erase(iterator &it) = 0;
    virtual void eraseAtIndex(std::size_t index) = 0;
    virtual const T &front() const = 0;
    virtual const T &back() const = 0;
    virtual T &front() = 0;
    virtual T &back() = 0;
    virtual List<T>::iterator begin() = 0;
    virtual List<T>::iterator end() = 0;
    virtual List<T>::const_iterator begin() const = 0;
    virtual List<T>::const_iterator end() const = 0;
    virtual bool operator==(const List<T> &rhs) const {
        if(size() != rhs.size()) {
            return false;
        }
        auto rhsIt = rhs.begin();
        for(auto it{begin()}; it != end(); it++, rhsIt++) {
            if(*it != *rhsIt) {
                return false;
            }
        }
        return true;
    }
};

#endif //LIST_H