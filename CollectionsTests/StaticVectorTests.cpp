//
// Created by Craig Home on 7/31/23.
//

#include "../Collections/StaticVector.h"
#include "doctest.h"

#include <cstring>

//----------------------------------------------------------------------------
// Snitch test cases:
//----------------------------------------------------------------------------

TEST_CASE("StaticVector Construction") {

    //Simple default constructor
    {
        StaticVector<int, 4> v;
        REQUIRE(v.capacity() == 4);
    }

    //Initializer list
    {
        StaticVector<int, 4> v = { 4, 3, 2, 1 };
        REQUIRE(v.size() == 4);
        const int expected[]{4, 3, 2, 1};
        REQUIRE(memcmp(expected, v.data(), sizeof(expected)) == 0);
    }

    //Initializer list, too many
    {
        try {
            const StaticVector<int, 4> v = { 5, 4, 3, 2, 1 };
            (void)v;
        } catch(const std::runtime_error &e) {
            REQUIRE(true);
        } catch(const std::exception &e) {
            REQUIRE(false);
        }
    }
}