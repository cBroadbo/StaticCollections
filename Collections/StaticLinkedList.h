//
// Created by Craig Home on 8/6/24.
//

#ifndef STATICCOLLECTIONS_STATICLINKEDLIST_H
#define STATICCOLLECTIONS_STATICLINKEDLIST_H

#include "LinkedList.h"

template<typename T, size_t NUM_ELEMS>
class StaticLinkedList: public LinkedList<T> {
public:
    StaticLinkedList(): LinkedList<T>(mStaticAllocator) {}

    /**
     * Copy constructor.  NOTE that this constructor does NOT copy the rhs allocator.  By design,
     * the StaticLinkedList has its own element storage and allocator, so it should not use
     * another list's.  So it calls the superclass: LinkedList<T> constructor, with its own
     * allocator and then it simply makes copies of all the elements.
     *
     * @param rhs - list to copy.
     */
    StaticLinkedList(const StaticLinkedList &rhs):
            LinkedList<T>(mStaticAllocator) {

        for(auto &elem: rhs) {
            LinkedList<T>::push_back(elem);
        }
    }

    explicit StaticLinkedList(const LinkedList<T> &rhs):
            LinkedList<T>(mStaticAllocator) {

        for(auto &elem: rhs) {
            LinkedList<T>::push_back(elem);
        }
    }

    StaticLinkedList(const std::initializer_list<T> initializerList):
            LinkedList<T>(mStaticAllocator) {

        for(const T* elem = initializerList.begin(); elem != initializerList.end(); elem++) {
            LinkedList<T>::push_back(*elem);
        }
    }

private:
    class Allocator: public LinkedList<T>::Allocator {
    public:
        Allocator(): LinkedList<T>::Allocator() {
            initNodes();
        }

        typename LinkedList<T>::Node *alloc() override {
            if(mAvailable) {
                auto ptr = mAvailable;
                mAvailable = mAvailable->mNext;
                if(mAvailable) {
                    mAvailable->mPrev = nullptr;
                }
                mNumAllocations++;
                return ptr;
            } else {
                throw std::bad_alloc();
            }
        }

        void free(typename LinkedList<T>::Node *ptr) override {
            if(ptr) {
                ptr->mNext = mAvailable;
                ptr->mPrev = nullptr;
                if(mAvailable) {
                    mAvailable->mPrev = ptr;
                }
                mAvailable = ptr;
                mNumAllocations--;
            }
        }

        [[nodiscard]] size_t size() const override {
            return NUM_ELEMS - mNumAllocations;
        }

    private:
        size_t mNumAllocations = 0;

        typename LinkedList<T>::Node mNodes[NUM_ELEMS]{};
        typename LinkedList<T>::Node *mAvailable{&mNodes[0]};

    private:
        void initNodes() {
            mNodes[0].mPrev = nullptr;

            //Now link all the nodes together.
            for(unsigned int i = 0; i < std::size(mNodes)-1; i++) {
                mNodes[i].mNext = &mNodes[i+1];
                mNodes[i].mNext->mPrev = &mNodes[i];
            }
            mNodes[NUM_ELEMS-1].mNext = nullptr;
            mAvailable = &mNodes[0];
        }

    } mStaticAllocator;
};

#endif //STATICCOLLECTIONS_STATICLINKEDLIST_H
