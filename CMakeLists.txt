cmake_minimum_required(VERSION 3.22)  # JUCE requires 3.22 minimum
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
project(audio-engine VERSION 1.0.0)

# =========================
# C++ Standard
# =========================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =========================
# Default Build Type
# =========================

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified. Defaulting to Release.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# =========================
# JUCE
# =========================

add_subdirectory(libs/JUCE)

# =========================
# Platform-Specific Dependencies (sndfile)
# =========================

if(WIN32)
    find_package(SndFile CONFIG REQUIRED)
    add_library(SndFileTarget INTERFACE)
    target_link_libraries(SndFileTarget INTERFACE SndFile::sndfile)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
    add_library(SndFileTarget INTERFACE)
    target_link_libraries(SndFileTarget INTERFACE PkgConfig::SNDFILE)
endif()

# =========================
# Fetch kissfft
# =========================

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

# =========================
# Audio Engine Library
# =========================

add_library(AudioEngineLib STATIC
    src/audio/AudioEngine.cpp
    src/effects/ffDelay.cpp
    src/dsp/CircularBuffer.cpp
    src/io/AudioFileIO.cpp
    src/effects/Gain.cpp
    src/effects/FDNReverb.cpp
    src/effects/EffectChain.cpp
    src/dsp/LowPassFilter.cpp
)

target_include_directories(AudioEngineLib
    PUBLIC
        src
        ${kissfft_SOURCE_DIR}
)

target_link_libraries(AudioEngineLib
    PUBLIC
        SndFileTarget
        kissfft
)

juce_add_plugin(DelayPlugin
    COMPANY_NAME "ADITYA"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    FORMATS VST3 Standalone
    PRODUCT_NAME "DelayPlugin"

)

target_sources(
    DelayPlugin PRIVATE
    src/FfDelay/PluginProcessor.cpp
    src/FfDelay/PluginProcessor.h
    src/FfDelay/PluginEditor.h
    src/FfDelay/PluginEditor.cpp
)

add_library(
    DelayLib STATIC
    src/effects/FfDelay.cpp
    src/dsp/CircularBuffer.cpp  
    src/
)

target_link_libraries(
    DelayPlugin PRIVATE
    DelayLib

    juce::juce_audio_utils
    juce::juce_audio_processors
    juce_recommended_config_flags
    juce_recommended_lto_flags
    juce_recommended_warning_flags
)
target_compile_definitions(DelayPlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0 
)
juce_generate_juce_header(DelayPlugin)

# =========================
# Main / Benchmark Applications
# =========================

add_executable(mainApp src/app/main_engine.cpp)
target_link_libraries(mainApp PRIVATE AudioEngineLib)

add_executable(benchmarkApp src/app/benchmark_main.cpp)
target_link_libraries(benchmarkApp PRIVATE AudioEngineLib)

# =========================
# Testing
# =========================

option(BUILD_TESTING "Build unit tests" ON)

if(BUILD_TESTING)

    enable_testing()

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(AudioTests
        src/tests/GainTest.cpp
        src/tests/CircularBufferTest.cpp
        src/tests/EffectChainTest.cpp
        src/tests/ffDelayTest.cpp
    )

    target_link_libraries(AudioTests
        PRIVATE
            AudioEngineLib
            gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(AudioTests)

endif()

# =========================
# Generated JUCE Plugins
# =========================

# Translator drops new plugin folders here — just add them as subdirectories.
# Each plugin's own CMakeLists.txt handles juce_add_plugin and target_link_libraries.

# add_subdirectory(generated/MyDelayPlugin)