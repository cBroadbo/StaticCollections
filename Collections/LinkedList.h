#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iterator>
#include <initializer_list>

#include "List.h"

template <class T, std::size_t SIZE>

class LinkedList: public List<T> {

public:
    typedef T value_type;

private:
    struct Node {
        T *mElement{nullptr};
        Node *mNext{nullptr};
        Node *mPrev{nullptr};

        Node() = default;
        explicit Node(T *ptr): mElement(ptr) {}

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

private:
    class PrivIterator: public List<T>::iteratorBase
    {
    public:
        typedef T& reference;
        typedef T* pointer;
        Node *mNode;

        explicit PrivIterator(Node *node) : mNode(node) { }
        void next() {
            if(mNode) {
                mNode = mNode->mNext;
            }
        }
        reference operator*() { return *mNode->mElement; }
        pointer operator->() { return mNode->mElement; }
        bool operator==(const typename List<T>::iteratorBase& rhs) const { return mNode == ((PrivIterator &)rhs).mNode; }
        bool operator!=(const typename List<T>::iteratorBase& rhs) const { return mNode != ((PrivIterator &)rhs).mNode; }
    };

    class PrivConstIterator: public List<T>::iteratorBase
    {
    public:
        typedef T& reference;
        typedef T* pointer;
        const Node *mNode;

        explicit PrivConstIterator(const Node *node) : mNode(node) { }
        void next() {
            if(mNode) {
                mNode = mNode->mNext;
            }
        }
        reference operator*() { return *mNode->mElement; }
        pointer operator->() { return mNode->mElement; }
        bool operator==(const typename List<T>::iteratorBase& rhs) const { return mNode == ((PrivConstIterator &)rhs).mNode; }
        bool operator!=(const typename List<T>::iteratorBase& rhs) const { return mNode != ((PrivConstIterator &)rhs).mNode; }
    };

private:
    Node mNodes[SIZE]{};
    T mElements[SIZE]{};
    T mEndElement{};
    std::size_t mNumElements{0};
    Node mEnd{&mEndElement};
    Node *mHead{&mEnd};
    Node *mAvailable{&mNodes[0]};

private:
    void initNodes() {
        mNodes[0].mPrev = nullptr;

        //Now link all the nodes together.
        for(unsigned int i = 0; i < std::size(mNodes)-1; i++) {
            mNodes[i].mNext = &mNodes[i+1];
            mNodes[i].mNext->mPrev = &mNodes[i];
            mNodes[i].mElement = &mElements[i];
        }
        mNodes[SIZE-1].mNext = nullptr;
        mNodes[SIZE-1].mElement = &mElements[SIZE-1];
        mNumElements = 0;
        mHead = &mEnd;
        mAvailable = &mNodes[0];
        mEnd.mPrev = nullptr;
    }

private:
    void eraseNode(Node *node) {
        mNumElements--;
        if(node == mHead) {
            mHead = mHead->mNext;
        }
        node->remove();
        if(mAvailable) {
            mAvailable->insertBefore(node);
        }
        mAvailable = node;
    }

public:
    LinkedList() { // NOLINT
        initNodes();
    }

    explicit LinkedList(const LinkedList<T, SIZE> &other) { // NOLINT
        initNodes();
        for(const T &elem: other) {
            push_back(elem);
        }
    }

    explicit LinkedList(const List<T> &other) { // NOLINT
        if(other.size() > max_size()) {
            throw std::invalid_argument("Attempted to copy a larger list into a smaller one.");
        }
        initNodes();
        for(const T &elem: other) {
            push_back(elem);
        }
    }

    LinkedList(const std::initializer_list<T> &initializerList) { // NOLINT
        if(initializerList.size() > max_size()) {
            throw std::invalid_argument("Attempted to initialize static linked list with too many literals.");
        }
        initNodes();

        for(const T* elem = initializerList.begin(); elem != initializerList.end(); elem++) {
            push_back(*elem);
        }
    }

    LinkedList<T, SIZE> &operator=(const List<T> &other) {
        if(other.size() > max_size()) {
            throw std::invalid_argument("Attempted to copy a larger list into a smaller one.");
        }
        initNodes();
        for(const T &elem: other) {
            push_back(elem);
        }
        return *this;
    };

    [[nodiscard]] bool empty() const override { return mHead == &mEnd; }
    [[nodiscard]] bool isFull() const override { return size() == max_size(); }
    [[nodiscard]] std::size_t size() const override { return mNumElements; }
    [[nodiscard]] std::size_t max_size() const override { return SIZE; }

    void push_back(const T &elem) override {
        if(!mAvailable) {
            throw std::overflow_error("Attempted to add element to full list.");
        }

        //Set a new head pointer to the available list.
        mNumElements++;
        Node *node = mAvailable;
        *node->mElement = elem;
        mAvailable = mAvailable->mNext;

        //Call remove node to set the links correctly.
        node->remove();
        mEnd.insertBefore(node);
        //was this the first element added?
        if(mHead == &mEnd) {
            mHead = node;
        }
    }

    void pop_back() override {
        if(!empty()) {
            //There is at least one element
            mNumElements--;
            Node *oldEnd = mEnd.mPrev;

            //Put the current end node back in the available list;
            oldEnd->remove();
            if(mAvailable) {
                mAvailable->insertBefore(oldEnd);
            }
            mAvailable = oldEnd;

            if(mHead == oldEnd) {
                mHead = &mEnd;
            }
        }
    }

    void push_front(const T &elem) override {
        if(!mAvailable) {
            throw std::overflow_error("Attempted to add element to full list.");
        }

        //Set a new head pointer to the available list.
        mNumElements++;
        Node *node = mAvailable;
        *node->mElement = elem;
        mAvailable = mAvailable->mNext;

        //Call remove node to set the links correctly.
        node->remove();
        mHead->insertBefore(node);
        mHead = node;
    }

    void pop_front() override {
        if(!empty()) {
            //There is at least one element
            mNumElements--;
            Node *newHead = mHead->mNext;

            //Put the current end node back in the available list;
            mHead->remove();
            if(mAvailable) {
                mAvailable->insertBefore(mHead);
            }
            mAvailable = mHead;
            mHead = newHead;
        }
    }

    void clear() override {
        initNodes();
    }

    const T &front() const override {
        if(empty()) {
            throw std::underflow_error("List is empty");
        }
        return *mHead->mElement;
    }

    const T &back() const override {
        if(empty()) {
            throw std::underflow_error("List is empty");
        }
        return *mEnd.mPrev->mElement;
    }

    T &front() override {
        if(empty()) {
            throw std::underflow_error("List is empty");
        }
        return *mHead->mElement;
    }

    T &back() override {
        if(empty()) {
            throw std::underflow_error("List is empty");
        }
        return *mEnd.mPrev->mElement;
    }

    void erase(const T &elem) override {

        if(empty()) {
            return;
        }

        for(Node *node = mHead; node != &mEnd; node = node->mNext) {
            if(*node->mElement == elem) {
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

    typename List<T>::iterator erase(typename List<T>::iterator &it) override {
        auto ret{it};
        ret++;
        if(auto linkedListIt = dynamic_cast<PrivIterator*>(it.getImpl())) {
            eraseNode(linkedListIt->mNode);
        }
        return ret;
    }

    typename List<T>::iterator begin() override {
        PrivIterator it(mHead);
        return typename List<T>::iterator(it, sizeof(it));
    }

    typename List<T>::iterator end() override {
        PrivIterator it(&mEnd);
        return typename List<T>::iterator(it, sizeof(it));
    }

    typename List<T>::const_iterator begin() const override {
        PrivConstIterator it(mHead);
        return typename List<T>::const_iterator(it, sizeof(it));
    }

    typename List<T>::const_iterator end() const override {
        PrivConstIterator it(&mEnd);
        return typename List<T>::const_iterator(it, sizeof(it));
    }
};

#endif //LINKEDLIST_H