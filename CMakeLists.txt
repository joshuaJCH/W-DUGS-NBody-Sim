# Minimum version of Cmake you need
cmake_minimum_required(VERSION 3.22)

# Project name
project(W_DUGS_NBody_Sim)

# Tell the compiler to use the 2014 c++ standards
set(CMAKE_CXX_STANDARD 14)

# =========================================================
# RULES AND COMPATABILITY
# =========================================================
# Safety flags (so it's clean code)
# -Wall means "shows all warnings"
# -Werror means "treat every warning as a hard error"

if(MSVC)
    set(COMPILER_FLAGS "/W4 /WX") # in case you use MSVC
else()
    set(COMPILER_FLAGS "-Wall -Werror") #    works for everything else
endif()

# This line appends the safety flags to the default C++ settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}")

# =========================================================
# EXTERNAL TOOLS
# =========================================================

# This section tells cmake to go to the internet and download the Catch2 testing library/framework
# Using Catch2 to test like in project 1, same version too

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.2
)

# Make the library available for the code to use
FetchContent_MakeAvailable(Catch2)

# =========================================================
# MAP (search paths)
# =========================================================

# This tells the compiler "when you see a #include, look inside these folders to find the files"
include_directories(src)
include_directories(include)

# =========================================================================
#                          TEAM: ADD NEW LOGIC FILES HERE
# OUTPUT (the simulation)  if you create a new class like "QuadTree.cpp",
#                          add it to the list below
# =========================================================================

# This creates the actual simulation program (the "main" executable)
# we list every .cpp and .hpp or .h files that belongs to the logic here
add_executable(main
        # ---- Add new files below this line ---
        src/main.cpp
        src/body.cpp
        include/body.hpp
        src/physics.cpp
        src/kdtree.cpp
        include/kdtree.hpp
        include/simulation.hpp
        src/simulation.cpp
        src/quadtree.cpp
)

# =========================================================
# TESTING              TEAM: ADD UNIT TESTS FILES HERE
# =========================================================


# This creates a second, sepatate program just for the unit test
add_executable(tests
        test/test.cpp
        src/body.cpp
        include/body.hpp
        src/physics.cpp
)

# This links the catch2 library to the 'test' program so it can run things like #REQUIRE" or "CHECK"
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)