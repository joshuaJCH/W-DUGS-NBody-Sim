cmake_minimum_required(VERSION 3.22)

project(W_DUGS_NBody_Sim)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(COMPILER_FLAGS /W4 /WX)
else()
    set(COMPILER_FLAGS -Wall -Wextra -Wpedantic -Werror)
endif()

add_compile_options(${COMPILER_FLAGS})

include_directories(include)

add_executable(nbody_sim
    src/main.cpp
    src/body.cpp
    src/physics.cpp
    src/simulation.cpp
    src/quadtree.cpp
)

option(ENABLE_TESTS "Build unit tests that depend on Catch2" OFF)

if(ENABLE_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(tests
        test/test.cpp
        src/body.cpp
        src/physics.cpp
        src/simulation.cpp
        src/quadtree.cpp
    )

    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
endif()
