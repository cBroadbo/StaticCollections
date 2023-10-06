//
// Created by Craig Home on 7/31/23.
//

#include "../Collections/Vector.h"
#include "doctest.h"

#include <cstring>
#include <numeric>

TEST_CASE("Vector Construction") {

    //Invalid args to constructor
    {
        int data[4];
        REQUIRE_THROWS_AS(Vector<int>(nullptr, 0), std::invalid_argument);
        REQUIRE_THROWS_AS(Vector<int>(data, 0), std::invalid_argument);
    }

    //Simple default constructor
    {
        int data[4];
        Vector<int> v{data, std::size(data)};
        REQUIRE(v.empty() == true);
    }
}

TEST_CASE("Vector push back") {
    int data[4];
    Vector<int> v{data, std::size(data)};

    for(int i{1}; i <= 4; i++) {
        REQUIRE(v.push_back(i) == true);
        REQUIRE(v.size() == i);
    }

    const int expected[]{1, 2, 3, 4};
    REQUIRE(memcmp(expected, v.data(), sizeof(expected)) == 0);
}

TEST_CASE("Vector accessors") {
    int data[4];
    Vector<int> v{data, std::size(data)};
    REQUIRE_THROWS_AS((void)(v.front() == 0), std::range_error);
    REQUIRE_THROWS_AS((void)(v.back() == 0), std::range_error);

    v.push_back(4);
    REQUIRE(v.front() == 4);
    REQUIRE(v.back() == 4);
    v.push_back(3);
    REQUIRE(v.front() == 4);
    REQUIRE(v.back() == 3);
    v.push_back(2);
    REQUIRE(v.front() == 4);
    REQUIRE(v.back() == 2);
    v.push_back(1);
    REQUIRE(v.front() == 4);
    REQUIRE(v.back() == 1);
    REQUIRE(v[0] == 4);
    REQUIRE(v[1] == 3);
    REQUIRE(v[2] == 2);
    REQUIRE(v[3] == 1);
    REQUIRE_THROWS_AS(v[5], std::out_of_range);
}

TEST_CASE("Vector iterators") {
    int data[4];
    Vector<int> v{data, std::size(data)};
    v.push_back(1);
    v.push_back(2);
    auto it{v.begin()};
    REQUIRE(*it == 1);
    REQUIRE(it[1] == 2);
    REQUIRE(std::accumulate(v.begin(), v.end(), 0) == 3);
    v.push_back(3);
    v.push_back(4);
    REQUIRE(std::accumulate(v.begin(), v.end(), 0) == 10);
}