#ifndef STATICCOLLECTIONS_LINKEDLIST_H
#define STATICCOLLECTIONS_LINKEDLIST_H

#include <iterator>
#include <initializer_list>

#include "List.h"

template <class T>
class LinkedList: public List<T> { //: public List<T> {
public:
    struct Node {
        T mElement = {};
        Node *mNext = nullptr;
        Node *mPrev = nullptr;

        Node() = default;
        explicit Node(const T &data): mElement(data) {}

        void insertBefore(Node *nodeToBeInserted) {
            nodeToBeInserted->mPrev = mPrev;
            nodeToBeInserted->mNext = this;
            if(nodeToBeInserted->mPrev) {
                nodeToBeInserted->mPrev->mNext = nodeToBeInserted;
            }
            mPrev = nodeToBeInserted;
        }

        void insertAfter(Node *nodeToBeInserted) {
            nodeToBeInserted->mPrev = this;
            nodeToBeInserted->mNext = mNext;
            if(nodeToBeInserted->mNext) {
                nodeToBeInserted->mNext->mPrev = nodeToBeInserted;
            }
            mNext = nodeToBeInserted;
        }

        //Remove a node from any linked nodes and set the other nodes
        //links accordingly.
        void remove() {
            if(mPrev) {
                mPrev->mNext = mNext;
            }
            if(mNext) {
                mNext->mPrev = mPrev;
            }
            mPrev = nullptr;
            mNext = nullptr;
        }
    };

    class Allocator {
    public:
        virtual Node *alloc() = 0;
        virtual void free(Node *ptr) = 0;

        /**
         * Total bytes remaining in the allocator
         * @return
         */
        [[nodiscard]] virtual size_t size() const = 0;
    };

    class const_iterator {
    public:
        friend LinkedList<T>;
        typedef T& reference;
        typedef T* pointer;

        explicit const_iterator(const Node *node) : mNode(node) {}

        const_iterator& operator++() {
            if(mNode) {
                mNode = mNode->mNext;
            }
            return *this;
        }

        const_iterator operator++(int) { // NOLINT(cert-dcl21-cpp)
            if(mNode) {
                mNode = mNode->mNext;
            }
            return *this;
        }

        const T &operator*() const { return mNode->mElement; }
        const T *operator->() const { return &mNode->mElement; }
        bool operator==(const const_iterator& rhs) const { return mNode == rhs.mNode; }
        bool operator!=(const const_iterator& rhs) const { return !(*this == rhs); }

    protected:
        const Node *mNode = nullptr;
    };

    class iterator {
    public:
        friend LinkedList<T>;
        typedef T& reference;
        typedef T* pointer;

        explicit iterator(Node *node): mNode(node) {}

        iterator& operator++() {
            if(mNode) {
                mNode = mNode->mNext;
            }
            return *this;
        }

        iterator operator++(int) { // NOLINT(cert-dcl21-cpp)
            if(mNode) {
                mNode = mNode->mNext;
            }
            return *this;
        }

        reference operator*() const { return mNode->mElement; }
        pointer operator->() const { return &mNode->mElement; }
        reference operator*() { return mNode->mElement; }
        pointer operator->() { return &mNode->mElement; }
        bool operator==(const iterator& rhs) const { return mNode == rhs.mNode; }
        bool operator!=(const iterator& rhs) const { return mNode != rhs.mNode; }

    protected:
        Node *mNode = nullptr;
    };


public:
    // Default constructor
    explicit LinkedList(Allocator &allocator): mAllocator(allocator) {
    }

    //Copy constructor
    LinkedList(const LinkedList<T> &rhs):
            mAllocator(rhs.mAllocator) {

        //copy the elements
        for(const T &elem: rhs) {
            push_back(elem);
        }
    }

    LinkedList(Allocator &allocator, const std::initializer_list<T> &initializerList):
        mAllocator(allocator) { // NOLINT
        for(const T* elem = initializerList.begin(); elem != initializerList.end(); elem++) {
            push_back(*elem);
        }
    }

    LinkedList(Allocator &allocator, const LinkedList<T> &other):
        mAllocator(allocator) {

        //copy the elements
        for(const T &elem: other) {
            push_back(elem);
        }
    };

    virtual LinkedList<T> &operator=(const LinkedList<T> &other) {
        //make sure to release all the held node back to the allocator.
        clear();
        
        //now just copy the elements
        for(const T &elem: other) {
            push_back(elem);
        }
        return *this;
    };
    
    [[nodiscard]] bool empty() const override { return mHead == &mEnd; }
    [[nodiscard]] bool isFull() const override { return mAllocator.size() < sizeof(Node);}
    [[nodiscard]] size_t size() const override { return mNumElements; }

    void push_back(const T &elem) override {
        // Create the new Node.
        auto newNode = mAllocator.alloc();

        if(!newNode) {
            throw std::bad_alloc();
        }
        newNode->mNext = nullptr;
        newNode->mPrev = nullptr;
        newNode->mElement = elem;

        //Set a new mHead pointer to the available list.
        mNumElements++;

        mEnd.insertBefore(newNode);
        //was this the first element added?
        if(mHead == &mEnd) {
            mHead = newNode;
        }
    }

    void pop_back() override {
        if(!empty()) {
            //There is at least one element
            mNumElements--;
            Node *oldEnd = mEnd.mPrev;

            //Release the current end node back to the allocator;
            oldEnd->remove();
            mAllocator.free(oldEnd);

            if(mHead == oldEnd) {
                mHead = &mEnd;
            }
        }
    }

    void push_front(const T &elem) override {
        // Create the new Node.
        auto newNode = mAllocator.alloc();

        if(!newNode) {
            throw std::bad_alloc();
        }
        newNode->mNext = nullptr;
        newNode->mPrev = nullptr;
        newNode->mElement = elem;

        //Set a new mHead pointer to the available list.
        mNumElements++;

        mHead->insertBefore(newNode);
        mHead = newNode;
    }

    void pop_front() override {
        if(!empty()) {
            //There is at least one element
            mNumElements--;
            Node *newHead = mHead->mNext;

            //Release the current head node back to the allocator;
            mHead->remove();
            mAllocator.free(mHead);

            //Make the new element the head
            mHead = newHead;
        }
    }

    void clear() override {
        while(mHead != &mEnd) {
            eraseAtIndex(0);
        }
    }

    const T &front() const override {
        if(empty()) {
            throw std::underflow_error("List is empty");
        }
        return mHead->mElement;
    }

    const T &back() const override {
        if(empty()) {
            throw std::underflow_error("List is empty");
        }
        return mEnd.mPrev->mElement;
    }

    T &front() override {
        if(empty()) {
            throw std::underflow_error("List is empty");
        }
        return mHead->mElement;
    }

    T &back() override {
        if(empty()) {
            throw std::underflow_error("List is empty");
        }
        return mEnd.mPrev->mElement;
    }

    void erase(const T &elem) override {
        if(empty()) {
            return;
        }

        for(Node *node = mHead; node != &mEnd; node = node->mNext) {
            if(node->mElement == elem) {
                eraseNode(node);
                return;
            }
        }
    }

    void eraseAtIndex(std::size_t index) override {
        if(empty()) {
            return;
        }
        std::size_t i(0);
        for(Node *node = mHead; node != &mEnd; node = node->mNext, i++) {
            if(i == index) {
                eraseNode(node);
                return;
            }
        }
    }

    iterator erase(iterator &it) {
        iterator ret{it};
        ret++;
        eraseNode(it.mNode);
        return ret;
    }

    iterator begin() { return iterator(mHead); }
    iterator end() { return iterator(&mEnd); }
    const_iterator begin() const { return const_iterator(mHead); }
    const_iterator end() const { return const_iterator(&mEnd); }

    bool operator==(const LinkedList<T> &rhs) const {
        if (size() != rhs.size()) {
            return false;
        }
        auto rhsIt = rhs.begin();
        for (auto it{begin()}; it != end(); it++, rhsIt++) {
            if (*it != *rhsIt) {
                return false;
            }
        }
        return true;
    }

private:
    void eraseNode(Node *node) {
        if(node == mHead) {
            mHead = mHead->mNext;
            if(mHead) {
                mHead->mPrev = nullptr;
            } else {
                //mHead is null, so make tail null too
                mTail = nullptr;
            }
            mAllocator.free(node);
            mNumElements--;
        } else if(node == mTail) {
            mTail = mTail->mPrev;
            mTail->mNext = nullptr;
            mAllocator.free(node);
            mNumElements--;
        } else {
            node->mPrev->mNext = node->mNext;
            node->mNext->mPrev = node->mPrev;
            mAllocator.free(node);
            mNumElements--;
        }
    }

private:
    Node mEnd;
    Node* mHead = &mEnd;
    Node* mTail = &mEnd;
    Allocator &mAllocator;
    size_t mNumElements = 0;
};

#endif //STATICCOLLECTIONS_LINKEDLIST_H