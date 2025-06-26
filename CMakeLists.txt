cmake_minimum_required(VERSION 3.30)

project(OptionCalculator CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED)

# --- Manual searching for QuantLib (only brew version) ---
find_path(QUANTLIB_INCLUDE_DIR
        NAMES ql/quantlib.hpp
        HINTS $(brew --prefix)/include
)

find_library(QUANTLIB_LIBRARY
        NAMES QuantLib
        HINTS $(brew --prefix)/lib
)

if (NOT QUANTLIB_INCLUDE_DIR OR NOT QUANTLIB_LIBRARY)
    message(FATAL_ERROR "Failed to find QuantLib. Make sure it is installed via brew (`brew install quantlib`) and that `brew --prefix` works.")
endif()

message(STATUS "Found Boost headers: ${Boost_INCLUDE_DIRS}")
message(STATUS "Found QuantLib headers: ${QUANTLIB_INCLUDE_DIR}")
message(STATUS "Found QuantLib library: ${QUANTLIB_LIBRARY}")

add_executable(OptionCalculator main.cpp)

target_include_directories(OptionCalculator PRIVATE
        ${QUANTLIB_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(OptionCalculator PRIVATE
        ${QUANTLIB_LIBRARY}
        Boost::boost
)