#include <stdexcept>
#include <list>
#include "../Collections/LinkedList.h"

#include "doctest.h"

namespace {
    const std::size_t MAX_TAG_SIZE = 8;
    struct Tag {
        char mTag[MAX_TAG_SIZE]{};
        Tag() = default;
        explicit Tag(const char *tag) { std::strncpy(mTag, tag, sizeof(mTag)-1); }
        [[nodiscard]] const char *getTag() const { return mTag; }
        bool operator==(const Tag &rhs) const { return std::strcmp(mTag, rhs.mTag); }
    };

    template<class T, size_t MAX_ALLOCATIONS>
    class Allocator: public LinkedList<T>::Allocator {
    public:
        typename LinkedList<T>::Node *alloc() override {
            if(mNumAllocations < MAX_ALLOCATIONS) {
                auto ptr = new typename LinkedList<T>::Node;
                if(ptr) {
                    mNumAllocations++;
                }
                return ptr;
            }
            return nullptr;
        }

        void free(typename LinkedList<T>::Node *ptr) override {
            delete ptr;
            mNumAllocations--;
        }

        [[nodiscard]] size_t size() const override {
            return UINT32_MAX;
        }

    private:
        size_t mNumAllocations = 0;
    };

}

TEST_CASE("TestLinkedList_copyConstructorOfIdenticalType") {

    const std::size_t MAX_TAGS = 8;
    Allocator<Tag, MAX_TAGS> allocator;

    //When copying a list of the same EXACT type (same template args), a specific version of
    //the copy constructor is called.  So we need to test that exact one.
    //If it doesn't exist this when we iterate through the copied list we will have a bad access fault.
    LinkedList<Tag> tagList = {allocator, { Tag("one"), Tag("two"), Tag("three"), Tag("four") }};
    LinkedList<Tag> tagListCopy{tagList};

    auto origIt{tagList.begin()};
    for(const auto tag: tagListCopy) {
        REQUIRE(std::strcmp(tag.getTag(), origIt->getTag()) == 0);
        origIt++;
    }
}

TEST_CASE("TestLinkedList_copyConstructorOfSameElementTypeDifferentSize") {

    const std::size_t MAX_TAGS = 4;
    Allocator<Tag, MAX_TAGS> allocator0;

    LinkedList<Tag> tagList = {allocator0, { Tag("one"), Tag("two"), Tag("three"), Tag("four") }};

    //Copy to a destination that has more capacity.
    {
        Allocator<Tag, MAX_TAGS+1> allocator1;
        LinkedList<Tag> tagListCopy(allocator1, tagList);

        auto origIt{tagList.begin()};
        for (const auto tag: tagListCopy) {
            REQUIRE(std::strcmp(tag.getTag(), origIt->getTag()) == 0);
            origIt++;
        }
    }

    //Copy to a destination that has less capacity.  Should result in an exception
    {
        Allocator<Tag, MAX_TAGS-1> allocator1;
        typedef LinkedList<Tag> ListType;
        REQUIRE_THROWS_AS({ListType list(allocator1, tagList); (void) list; }, std::bad_alloc);
    }
}

TEST_CASE("TestLinkedList_empty") {
    Allocator<int, 3> allocator;
    LinkedList<int> list(allocator);
    REQUIRE(list.empty());
    list.push_back(123);
    REQUIRE(!list.empty());
    list.clear();
    REQUIRE(list.empty());
    list.push_back(123);
    list.push_back(234);
    list.push_back(345);
    REQUIRE(!list.empty());
}

TEST_CASE("LinkedList Size") {
    Allocator<int, 4> allocator;
    LinkedList<int> list(allocator);
    REQUIRE(list.size() == (std::size_t)0U);
    list.push_back(123);
    REQUIRE(list.size() == (std::size_t)1U);
    list.push_back(234);
    REQUIRE(list.size() == (std::size_t)2U);
    list.push_back(345);
    REQUIRE(list.size() == (std::size_t)3U);
    list.clear();
    REQUIRE(list.size() == (std::size_t)0U);
}

TEST_CASE("TestLinkedList_push_back") {

    Allocator<int, 3> allocator;
    LinkedList<int> list(allocator);
    REQUIRE(list.size() == list.size());
    list.push_back(123);
    REQUIRE(list.size() == list.size());
    list.push_back(234);
    REQUIRE(list.size() == list.size());
    list.push_back(345);
    REQUIRE(list.size() == list.size());
    REQUIRE_THROWS_AS(list.push_back(456), std::bad_alloc);
}

TEST_CASE("TestLinkedList_pop_back") {

    Allocator<int, 3> allocator;
    LinkedList<int> list(allocator);
    list.pop_back();
    REQUIRE(list.size() == list.size());
    list.push_back(123);
    REQUIRE(list.size() == list.size());
    list.pop_back();
    REQUIRE(list.size() == list.size());

    list.push_back(123);
    list.push_back(234);
    list.push_back(345);
    list.pop_back();
    REQUIRE(list.size() == list.size());
    list.pop_back();
    REQUIRE(list.size() == list.size());
    list.pop_back();
    REQUIRE(list.size() == list.size());
}

TEST_CASE("TestLinkedList_push_front") {

    Allocator<int, 3> allocator;
    LinkedList<int> list(allocator);
    REQUIRE(list.size() == list.size());
    list.push_front(123);
    REQUIRE(list.size() == list.size());
    list.push_front(234);
    REQUIRE(list.size() == list.size());
    list.push_front(345);
    REQUIRE(list.size() == list.size());
    REQUIRE_THROWS_AS(list.push_front(456), std::bad_alloc);
}

TEST_CASE("TestLinkedList_pop_front") {

    Allocator<int, 3> allocator;
    LinkedList<int> list(allocator);
    list.push_front(123);
    REQUIRE(list.size() == list.size());
    list.pop_front();
    REQUIRE(list.size() == list.size());

    list.push_front(123);
    list.push_front(234);
    list.push_front(345);
    list.pop_front();
    REQUIRE(list.size() == list.size());
    list.pop_front();
    REQUIRE(list.size() == list.size());
    list.pop_front();
    REQUIRE(list.size() == list.size());
}

TEST_CASE("TestLinkedList_back") {

    Allocator<int, 3> allocator;
    LinkedList<int> list(allocator);
    list.push_back(123);
    REQUIRE(list.back() == list.back());
    REQUIRE(list.front() == list.front());
    list.push_back(234);
    REQUIRE(list.back() == list.back());
    REQUIRE(list.front() == list.front());
    list.push_back(345);
    REQUIRE(list.back() == list.back());
    REQUIRE(list.front() == list.front());
    list.pop_back();
    REQUIRE(list.back() == list.back());
    REQUIRE(list.front() == list.front());
    list.pop_back();
    REQUIRE(list.back() == list.back());
    REQUIRE(list.front() == list.front());
    list.pop_back();
    REQUIRE_THROWS_AS(list.back(), std::underflow_error);
}

TEST_CASE("TestLinkedList_front") {

    Allocator<int, 3> allocator;
    LinkedList<int> list(allocator);
    list.push_front(123);
    REQUIRE(list.front() == list.front());
    REQUIRE(list.back() == list.back());
    list.push_front(234);
    REQUIRE(list.front() == list.front());
    REQUIRE(list.back() == list.back());
    list.push_front(345);
    REQUIRE(list.front() == list.front());
    REQUIRE(list.back() == list.back());
    list.pop_front();
    REQUIRE(list.front() == list.front());
    REQUIRE(list.back() == list.back());
    list.pop_front();
    REQUIRE(list.front() == list.front());
    REQUIRE(list.back() == list.back());
    list.pop_front();
    REQUIRE_THROWS_AS(list.front(), std::underflow_error);
}

TEST_CASE("TestLinkedList_erase") {

    Allocator<int, 3> allocator;
    LinkedList<int> list(allocator);

    //Do this twice to exercise the node assignment
    //multiple times.
    for(int i = 0; i < 2; i++) {
        list.erase(123);

        list.push_back(123);
        list.push_back(234);
        list.push_back(345);

        list.erase(234);
        REQUIRE(list.size() == list.size());
        REQUIRE(list.front() == list.front());
        REQUIRE(list.back() == list.back());

        list.erase(345);
        REQUIRE(list.size() == list.size());
        REQUIRE(list.front() == list.front());
        REQUIRE(list.back() == list.back());

        list.erase(123);
        REQUIRE(list.size() == list.size());
    }

}

TEST_CASE("TestLinkedList_eraseAtIndex") {

    const std::size_t LIST_SIZE = 5;
    Allocator<int, LIST_SIZE> allocator;
    LinkedList<int> list(allocator, {99, 7, 88, 5, 77});

    //remove the middle element leaving 4 elements
    {
        list.eraseAtIndex(2);
        REQUIRE(list.size() == list.size());
        int resultInts[LIST_SIZE - 1];
        std::copy(list.begin(), list.end(), resultInts);
        const int expected[] = { 99, 7, 5, 77 };
        bool same = std::memcmp(expected, resultInts, sizeof(resultInts)) == 0;
        REQUIRE(same);
    }

    //remove the middle element leaving 3 elements
    {
        list.eraseAtIndex(0);
        REQUIRE(list.size() == list.size());
        int resultInts[LIST_SIZE - 2];
        std::copy(list.begin(), list.end(), resultInts);
        const int expected[] = { 7, 5, 77 };
        bool same = std::memcmp(expected, resultInts, sizeof(resultInts)) == 0;
        REQUIRE(same);
    }

    //remove the end element leaving 2 elements
    {
        list.eraseAtIndex(2);
        REQUIRE(list.size() == list.size());
        int resultInts[LIST_SIZE - 3];
        std::copy(list.begin(), list.end(), resultInts);
        const int expected[] = { 7, 5 };
        bool same = std::memcmp(expected, resultInts, sizeof(resultInts)) == 0;
        REQUIRE(same);
    }
}

TEST_CASE("TestLinkedList_iterator") {
    Allocator<int, 3> allocator;
    LinkedList<int> list0(allocator);
    list0.push_back(1);
    list0.push_back(2);
    list0.push_back(3);

    LinkedList<int>::iterator it = list0.begin();
    REQUIRE(*it == *it);
    it++;
    REQUIRE(*it == *it);
    ++it;
    REQUIRE(*it == *it);
    ++it;
    bool eq = it == list0.end();
    REQUIRE(eq);

    const LinkedList<int> &list1 = list0;
    LinkedList<int>::const_iterator constIt = list1.begin();
    REQUIRE(*constIt == *constIt);
    constIt++;
    REQUIRE(*constIt == *constIt);
    ++constIt;
    REQUIRE(*constIt == *constIt);
    ++constIt;
    bool cEq = constIt == list1.end();
    REQUIRE(cEq);
}

TEST_CASE("TestLinkedList_copyAlgorithm") {
    int myInts[] = { 1, 2, 3, 4, 5 };
    Allocator<int, 10> allocator;
    LinkedList<int> list0(allocator);
    std::copy(myInts, myInts + std::size(myInts), std::back_inserter(list0));

    int resultInts[std::size(myInts)];
    std::copy(list0.begin(), list0.end(), resultInts);

    bool same = std::memcmp(myInts, resultInts, sizeof(resultInts)) == 0;
    REQUIRE(same);
}

TEST_CASE("TestLinkedList_literalConstructor") {
    const std::size_t LIST_SIZE = 5;
    Allocator<int, LIST_SIZE> allocator;
    LinkedList<int> list0 = {allocator, { 1, 2, 3, 4, 5 }};

    int resultInts[LIST_SIZE];
    std::copy(list0.begin(), list0.end(), resultInts);

    const int expected[] = { 1, 2, 3, 4, 5 };
    bool same = std::memcmp(expected, resultInts, sizeof(resultInts)) == 0;
    REQUIRE(same);
}

TEST_CASE("TestLinkedList_eraseWithIterator") {
    const std::size_t LIST_SIZE = 10;
    Allocator<int, LIST_SIZE * 2> allocator;
    LinkedList<int> list0 = { allocator, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }};

    for(auto it{list0.begin()}; it != list0.end(); ) {
        if((*it & 1) == 1) {
            it = list0.erase(it);
        } else {
            it++;
        }
    }

    LinkedList<int> expected = {allocator, { 0, 2, 4, 6, 8 }};
    REQUIRE(list0 == expected);
}

TEST_CASE("TestLinkedList_mutableListWithIterator") {
    const std::size_t LIST_SIZE = 10;
    Allocator<int, LIST_SIZE * 2> allocator;
    LinkedList<int> list0 = { allocator, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }};

    for (int &it : list0) {
        it *= 10;
    }

    LinkedList<int> expected = { allocator, { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 }};
    REQUIRE(list0 == expected);
}

TEST_CASE("TestLinkedList_frontAndBackMutability") {

    const std::size_t LIST_SIZE = 10;
    Allocator<int, LIST_SIZE * 2> allocator;
    LinkedList<int> list0 = { allocator, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }};

    list0.front() = 99;
    list0.back() = 77;

    const LinkedList<int> expected = { allocator, { 99, 1, 2, 3, 4, 5, 6, 7, 8, 77 }};
    REQUIRE(list0 == expected);
}