#include "../Collections/CircularQueue.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE( "CircularQueue is created" ) {
    CircularQueue<int, 4> queue;
    REQUIRE(queue.empty());
    REQUIRE(queue.capacity() == 4);
    REQUIRE(queue.size() == 0); // NOLINT(readability-container-size-empty)
    REQUIRE(queue.empty());
    REQUIRE_FALSE(queue.full());
}

TEST_CASE( "CircularQueue is cleared") {
    CircularQueue<int, 4> queue = { 6, 7, 8, 9 };
    REQUIRE(!queue.empty());
    REQUIRE(queue.size() == 4);
    queue.clear();
    REQUIRE(queue.empty());
}

TEST_CASE("CircularQueue Initializer List") {

    //Initializer list
    {
        CircularQueue<int, 4> queue = {4, 3, 2, 1 };
        REQUIRE(queue.size() == 4);
        REQUIRE(queue.full());
        REQUIRE(queue.CAPACITY == 4);
        const int expected[]{4, 3, 2, 1};
        int got[queue.CAPACITY];
        for(auto &v: got) {
            REQUIRE(queue.pop(v));
        }
        REQUIRE(memcmp(expected, got, sizeof(expected)) == 0);
    }

    //Initializer list, too many
    {
        try {
            const CircularQueue<int, 4> v = { 5, 4, 3, 2, 1 };
            (void)v;
        } catch(const std::runtime_error &e) {
            REQUIRE(true);
        } catch(const std::exception &e) {
            REQUIRE(false);
        }
    }
}
TEST_CASE( "CircularQueue single pushes and pops") {
    CircularQueue<int, 4> queue;
    REQUIRE(queue.push(1));
    REQUIRE(queue.size() == 1);
    REQUIRE_FALSE(queue.empty());
    REQUIRE(queue.push(2));
    REQUIRE(queue.size() == 2);
    REQUIRE_FALSE(queue.empty());
    REQUIRE(queue.push(3));
    REQUIRE(queue.size() == 3);
    REQUIRE_FALSE(queue.empty());
    REQUIRE(queue.push(4));
    REQUIRE(queue.size() == 4);
    REQUIRE_FALSE(queue.empty());
    REQUIRE_FALSE(queue.push(5));
    REQUIRE(queue.size() == 4);
    REQUIRE_FALSE(queue.empty());
    REQUIRE(queue.full());

    int value;
    REQUIRE(queue.pop(value));
    REQUIRE(queue.size() == 3);
    REQUIRE_FALSE(queue.empty());
    REQUIRE_FALSE(queue.full());
    REQUIRE(value == 1);
    REQUIRE(queue.pop(value));
    REQUIRE(queue.size() == 2);
    REQUIRE_FALSE(queue.empty());
    REQUIRE_FALSE(queue.full());
    REQUIRE(value == 2);
    REQUIRE(queue.pop(value));
    REQUIRE(queue.size() == 1);
    REQUIRE_FALSE(queue.empty());
    REQUIRE_FALSE(queue.full());
    REQUIRE(value == 3);
    REQUIRE(queue.pop(value));
    REQUIRE(queue.size() == 0); // NOLINT(readability-container-size-empty)
    REQUIRE(queue.empty());
    REQUIRE_FALSE(queue.full());
    REQUIRE(value == 4);
}

TEST_CASE( "CircularQueue bulk push") {
    CircularQueue<int, 4> queue;
    REQUIRE_FALSE(queue.push(nullptr, 10));
    const int items0[] = { 1, 2, 3, 4 };
    REQUIRE(queue.push(items0, std::size(items0)));
    REQUIRE(queue.full());
    const int items1[] = { 1, 2, 3, 4, 5, 6 };
    REQUIRE_FALSE(queue.push(items1, std::size(items1)));
    REQUIRE(queue.full());
}

TEST_CASE( "CircularQueue bulk pop") {
    {
        CircularQueue<int, 4> queue = { 4, 3, 2, 1};
        REQUIRE(queue.popElements(2));
        REQUIRE(queue.size() == 2);
        REQUIRE(queue.popElements(2));
        REQUIRE(queue.empty());
    }
    CircularQueue<int, 4> queue = { 4, 3, 2, 1};
}

TEST_CASE( "CircularQueue block test") {

    CircularQueue<int, 4> queue = { 4, 3, 2, 1};
    REQUIRE(queue.getBlock().size() == 4);
    {
        const int expected[]{4, 3, 2, 1};
        REQUIRE(memcmp(queue.getBlock().data(), expected, sizeof(int) * 4) == 0);
    }

    //Remove 2 elements from the queue.  The last 2 elements should still be present and contiguous, so
    //a single block will encapsulate both.
    {
        REQUIRE(queue.popElements(2));
        REQUIRE(queue.getBlock().size() == 2);
        const int expected[]{2, 1};
        REQUIRE(memcmp(queue.getBlock().data(), expected, sizeof(int) * 2) == 0);
    }

    //Now push two new elements to the queue.  Note that a CircularQueue with the capacity of 4 actually
    //has storage for 5.  The queue now store the first added element in the array[4] location, and wrap around
    //and store the remaining element at index 0.  At this point the elements of the queue are NOT
    //contiguous and will require that getBlock() be called to get at the first three elements, then
    //we pop those elements.  Follow with another getBlock() that will return a span of the
    //remaining element.
    {
        REQUIRE(queue.push(7));
        REQUIRE(queue.push(8));
        REQUIRE(queue.getBlock().size() == 3);
        const int expected0[]{2, 1, 7};
        REQUIRE(memcmp(queue.getBlock().data(), expected0, sizeof(int) * queue.getBlock().size()) == 0);
        REQUIRE(queue.popElements(queue.getBlock().size()));
        const int expected1[]{8};
        REQUIRE(queue.getBlock().size() == 1);
        REQUIRE(memcmp(queue.getBlock().data(), expected1, sizeof(int) * queue.getBlock().size()) == 0);
    }
}