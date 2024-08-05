cmake_minimum_required(VERSION 3.24)
project(StaticCollections)

set(CMAKE_CXX_STANDARD 23)

set(INCLUDE_EXAMPLES ${EXAMPLES_AND_UNIT_TESTS})

Include(FetchContent)

add_library(StaticCollections INTERFACE
        Collections/CircularQueue.h
        Collections/Queue.h
        Collections/StaticVector.h
        Collections/Vector.h
        Collections/LinkedList.h
        )

target_include_directories(StaticCollections INTERFACE
        "${PROJECT_SOURCE_DIR}"
        )

# If you would like to add the following targets, pass this define into cmake(In CLion you would add this to
# your Preferences/Build,Execution,Deployment in the cmake options):
# -DEXAMPLES_AND_UNIT_TESTS="ON"
if(${INCLUDE_EXAMPLES})
    FetchContent_Declare(
            doctest
            GIT_REPOSITORY https://github.com/doctest/doctest.git
            GIT_TAG        v2.4.11 # or a later release
    )

    FetchContent_MakeAvailable(doctest)

    message("Including examples and unit tests")

    add_executable(StaticCollectionsTests EXCLUDE_FROM_ALL
            CollectionsTests/CircularQueueTests.cpp
            CollectionsTests/StaticVectorTests.cpp
            CollectionsTests/VectorTests.cpp
            CollectionsTests/LinkedListTests.cpp
            )
    target_include_directories(StaticCollectionsTests PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}/_deps/doctest-src/doctest
            )

endif()
