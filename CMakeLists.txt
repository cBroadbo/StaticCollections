cmake_minimum_required(VERSION 3.24)
project(StaticCollections)

set(CMAKE_CXX_STANDARD 23)

Include(FetchContent)

FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG        v2.4.11 # or a later release
)

FetchContent_MakeAvailable(doctest)

add_executable(StaticCollectionsTests EXCLUDE_FROM_ALL
        CollectionsTests/CircularQueueTests.cpp
        CollectionsTests/StaticVectorTests.cpp
        CollectionsTests/VectorTests.cpp
        )
target_include_directories(StaticCollectionsTests PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/_deps/doctest-src/doctest
        )

add_library(StaticCollections INTERFACE
        Collections/CircularQueue.h
        Collections/Queue.h
        Collections/StaticVector.h
        Collections/Vector.h
        )