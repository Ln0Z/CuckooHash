cmake_minimum_required(VERSION 3.27)
project(CuckooHash)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(header)

add_executable(CuckooHash
        header/cuckoo_hash.hpp
        header/rand_cuckoo_hash.hpp
        implementation/cuckoo_hash.cpp
        implementation/rand_cuckoo_hash.cpp
        tests/main.cpp
)

add_dependencies(CuckooHash gtest)
target_link_libraries(CuckooHash gtest gtest_main pthread)