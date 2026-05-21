cmake_minimum_required(VERSION 3.10)

project(audio-engine)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified. Defaulting to Release (-O3 optimizations).")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# 1. Find libsndfile using PkgConfig (Standard for Linux/Pop!_OS)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)

include(FetchContent)
FetchContent_Declare(
    kissfft
    GIT_REPOSITORY https://github.com/mborgerding/kissfft.git
    GIT_TAG master
)
set(KISSFFT_DATATYPE float CACHE STRING "" FORCE)
set(KISSFFT_TOOLS OFF CACHE BOOL "" FORCE)
set(KISSFFT_TEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(kissfft)

# 2. Audio Engine Library
add_library(AudioEngineLib STATIC
    ./src/audio/AudioEngine.cpp
    ./src/effects/ffDelay.cpp
    ./src/dsp/CircularBuffer.cpp
    ./src/io/AudioFileIO.cpp
    ./src/effects/Gain.cpp
    ./src/effects/FDNReverb.cpp
    ./src/effects/EffectChain.cpp
    ./src/audio/AudioBuffer_opt.h
    ./src/dsp/LowPassFilter.cpp
)

# Tell the library where its own headers AND kissfft headers are
target_include_directories(AudioEngineLib PUBLIC 
    ./src
    ${kissfft_SOURCE_DIR}
)

# Link the library to sndfile and kissfft
target_link_libraries(AudioEngineLib PUBLIC PkgConfig::SNDFILE kissfft)

# 3. Main executable
add_executable(mainApp ./src/app/main_engine.cpp)
target_link_libraries(mainApp PRIVATE AudioEngineLib)

# Benchmarking
add_executable(benchmarkApp ./src/app/benchmark_main.cpp)
target_link_libraries(benchmarkApp PRIVATE AudioEngineLib)

# 4. Optional tests (Ignored for now, but clean)
option(BUILD_TESTING "Build unit tests" ON) 

if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0  
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(AudioTests
        ./src/tests/GainTest.cpp
        ./src/tests/CircularBufferTest.cpp
        ./src/tests/EffectChainTest.cpp
        ./src/tests/ffDelayTest.cpp
        # BlockUtilsTest.cpp has been removed!
    )

    target_link_libraries(AudioTests PRIVATE AudioEngineLib gtest_main)

    include(GoogleTest)
    gtest_discover_tests(AudioTests)
endif()