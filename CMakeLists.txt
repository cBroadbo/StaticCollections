cmake_minimum_required(VERSION 3.24)
project(StaticCollections)

set(CMAKE_CXX_STANDARD 23)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(StaticCollectionsTests
        CollectionsTests/CircularQueueTests.cpp
        CollectionsTests/StaticVectorTests.cpp
        CollectionsTests/VectorTests.cpp
        )

target_link_libraries(StaticCollectionsTests PRIVATE Catch2::Catch2WithMain)

add_library(StaticCollections INTERFACE
        Collections/CircularQueue.h
        Collections/Queue.h
        Collections/StaticVector.h
        Collections/Vector.h
        )