#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <iostream>
#include <cstddef>
#include <atomic>
#include <algorithm>
#include <span>
#include "Queue.h"

template<typename T, size_t SIZE>
class CircularQueue: public Queue<T> {
public:
    enum {CAPACITY = SIZE};
    CircularQueue() : mTail(0), mHead(0){}
    CircularQueue(const std::initializer_list<T> &initializerList) {
        if(initializerList.size() > SIZE) {
            throw std::runtime_error("number of initializer elements exceeds capacity.");
        }

        for(const T* elem = initializerList.begin(); elem != initializerList.end(); elem++) {
            this->push(*elem);
        }
    }
    virtual ~CircularQueue() = default;

    void clear() override { mTail = 0; mHead = 0; }
    bool push(const T& item) override;
    bool pop(T& item) override;
    bool popElements(size_t count) override;
    bool peek(T& item) override;

    [[nodiscard]] bool empty() const override;
    [[nodiscard]] bool full() const override;
    [[nodiscard]] size_t size() const override;
    [[nodiscard]] size_t capacity() const  override { return CAPACITY; }

    /**
     * Gets the span that includes the longest contiguous bytes from the front of the queue.  This is useful because
     * A circular queue, once filled, will have data in two contiguous runs (i.e. from the head of the queue
     * to the end of the physical storage, and then continue from the beginning of the storage to the end of the queue
     *
     * The best use of the this call are to copy the data out of the queue in the in bulk with largest chunks you're
     * able to get.  Used in conjunction with popElements, here's an example:
     *
     * uint8_t *dest = somebuffer;
     * while(!queue.empty()) {
     *      const auto block{queue.getBlock()};
     *      memcpy(dest, block.data(), block.size());
     *      dest += block.size();
     *      queue.popElements(block.size());
     * }
     *
     * @return a span of the data in the first block.
     */
    std::span<T const> getBlock() const {
        if(empty()) {
            return {};
        }

        const int tail = mTail.load();
        const int head = mHead.load();
        const std::uint16_t count = (head < tail)?
                                    (tail - head):
                                    ((CAPACITY+1) - head);

        return {mArray + head, count};
    }

private:
    [[nodiscard]] size_t increment(size_t idx) const;

    std::atomic_size_t  mTail;  // tail(input) index
    T                   mArray[SIZE + 1]{};
    std::atomic_size_t  mHead; // head(output) index
};

template<typename T, size_t Size>
bool CircularQueue<T, Size>::push(const T& item) {
    const auto current_tail = mTail.load();
    const auto next_tail = increment(current_tail);
    if(next_tail != mHead.load()) {
        mArray[current_tail] = item;
        mTail.store(next_tail);
        return true;
    }
    return false;  // full queue
}

// Pop by Consumer can only update the head
template<typename T, size_t Size>
bool CircularQueue<T, Size>::pop(T& item) {
    const auto current_head = mHead.load();
    if(current_head == mTail.load()) {
        return false;   // empty queue
    }

    item = mArray[current_head];
    mHead.store(increment(current_head));
    return true;
}

// Pop by Consumer can only update the head
template<typename T, size_t Size>
bool CircularQueue<T, Size>::popElements(size_t count) {
    const uint16_t numToPop = std::min(count, size());
    auto current_head = mHead.load();
    current_head = (std::uint16_t) ((current_head + numToPop) % std::size(mArray));
    mHead.store(current_head);
    return true;
}

// Pop by Consumer can only update the head
template<typename T, size_t Size>
bool CircularQueue<T, Size>::peek(T& item) {
    const auto current_head = mHead.load();
    if(current_head == mTail.load()) {
        return false;   // empty queue
    }

    item = mArray[current_head];
    return true;
}

template<typename T, size_t Size>
size_t CircularQueue<T, Size>::size() const {
    const int tail = mTail.load();
    const int head = mHead.load();
    if(tail >= head) {
        return tail - head;
    } else {
        return (std::uint16_t) ((CAPACITY+1) - head + tail);
    }
}

// snapshot with acceptance of that this comparison function is not atomic
// (*) Used by clients or test, since pop() avoid double load overhead by not
// using empty()
template<typename T, size_t Size>
bool CircularQueue<T, Size>::empty() const {
    return (mHead.load() == mTail.load());
}

// snapshot with acceptance that this comparison is not atomic
// (*) Used by clients or test, since push() avoid double load overhead by not
// using full()
template<typename T, size_t Size>
bool CircularQueue<T, Size>::full() const {
    const auto next_tail = increment(mTail.load());
    return (next_tail == mHead.load());
}

template<typename T, size_t Size>
size_t CircularQueue<T, Size>::increment(size_t idx) const {
    return (idx + 1) % std::size(mArray);
}

#endif //CIRCULARQUEUE_H
