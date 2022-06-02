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


# Public CMake Functions
function(add_reflection_target reflection_data_target_name target_name)

	get_target_property(_TARGET__BINARY_DIR ${target_name} BINARY_DIR)
	get_target_property(_TARGET__SOURCES ${target_name} SOURCES)
	get_target_property(_TARGET__NAME ${target_name} NAME)
	
	list(FILTER _TARGET__SOURCES INCLUDE REGEX ".ixx")
	foreach(_TARGET__SOURCE ${_TARGET__SOURCES})
		list(APPEND _REFLECTION_TARGET_SOURCES "${_TARGET__BINARY_DIR}/${_TARGET__SOURCE}.cpp")
	endforeach()
	
	set_source_files_properties(${_REFLECTION_TARGET_SOURCES} PROPERTIES GENERATED true)

	add_library(${reflection_data_target_name} OBJECT ${_REFLECTION_TARGET_SOURCES})
	add_custom_command(TARGET ${target_name} 
		POST_BUILD COMMAND NeatReflectionCodeGen ARGS "scan" "${_TARGET__NAME}.dir/Debug/" "."
		WORKING_DIRECTORY "${_TARGET__BINARY_DIR}")
	target_link_libraries(${reflection_data_target_name} PRIVATE NeatReflection ${target_name})
	target_compile_features(${reflection_data_target_name} PUBLIC cxx_std_20)

endfunction()


# Sources
add_subdirectory(NeatReflection) # Library
add_subdirectory(NeatReflectionCodeGen) # Executable tool 
add_subdirectory(tests) # Tests
