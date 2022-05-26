cmake_minimum_required (VERSION 3.20)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

if(NOT ${CMAKE_GENERATOR} STREQUAL "Visual Studio 17 2022")
	message(WARNING "Only the Visual Studio 17 2022 generator has support for modules. The current generator is: ${CMAKE_GENERATOR}")
endif()

project (NeatReflection)

include(FetchContent)
include(CTest)


# User Options
option(NEAT_REFLECTION_BUILD_TESTING "Enable tests for NeatReflection" ON)

# Sources
add_subdirectory(NeatReflection) # Library
add_subdirectory(NeatReflectionCodeGen) # Executable tool 
add_subdirectory(tests) # Tests
