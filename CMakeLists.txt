cmake_minimum_required (VERSION 3.24)

project (NeatReflection)

include(FetchContent)
include(CTest)


# User Options
option(NEAT_REFLECTION_BUILD_TESTING			"Enable tests for NeatReflection" OFF)
# option(NEAT_REFLECTION_USE_PREBUILT_CODEGEN_EXE "Don't compile NeatReflectionCodeGen from source but use the prebuilt binary." ON)


# Public CMake Functions

# Create a new CMake target named `reflection_data_target_name`, which will contain reflection data from target `target_name`
# To use this reflection name simply link it with you exe or dll: 
# 
# add_reflection_target(MyAwesomeLibrary_ReflectionData MyAwesomeLibrary)
# target_link_libraries(MyExeTarget MyAwesomeLibrary MyAwesomeLibrary_ReflectionData)
function(add_reflection_target reflection_data_target_name target_name)

	get_target_property(_TARGET__BINARY_DIR ${target_name} BINARY_DIR)
	get_target_property(_TARGET__SOURCES ${target_name} SOURCES)
	get_target_property(_TARGET__NAME ${target_name} NAME)
	
	list(FILTER _TARGET__SOURCES INCLUDE REGEX ".ixx")
	foreach(_TARGET__SOURCE ${_TARGET__SOURCES})
		cmake_path(GET _TARGET__SOURCE FILENAME _TARGET__SOURCE_FILENAME)
		list(APPEND _REFLECTION_TARGET_SOURCES "${_TARGET__BINARY_DIR}/${_TARGET__SOURCE_FILENAME}.cpp")
	endforeach()
	
	set_source_files_properties(${_REFLECTION_TARGET_SOURCES} PROPERTIES GENERATED true)

	add_library(${reflection_data_target_name} OBJECT ${_REFLECTION_TARGET_SOURCES})
	add_custom_command(OUTPUT ${_REFLECTION_TARGET_SOURCES}
		COMMAND ${NEAT_REFLECTION_CODEGEN_EXE} ARGS "scan" "${_TARGET__NAME}.dir/$<CONFIG>/" "."
		WORKING_DIRECTORY "${_TARGET__BINARY_DIR}"
		DEPENDS ${_TARGET__SOURCES})
	target_link_libraries(${reflection_data_target_name} PRIVATE NeatReflection ${target_name})
	target_compile_features(${reflection_data_target_name} PUBLIC cxx_std_20)

endfunction()


# Sources
add_library (NeatReflection
	"include/neat/Reflection.h"
	"include/neat/TemplateTypeId.h"
	"include/neat/DllMacro.h" 
	"include/neat/ReflectPrivateMembers.h"
	"src/neat/Reflection.cpp"
	"src/neat/TemplateTypeId.cpp")
target_compile_features(NeatReflection PUBLIC cxx_std_20)
target_include_directories(NeatReflection PUBLIC "include")
target_compile_definitions(NeatReflection PRIVATE BUILDING_REFLECTIONLIB=1) # TODO: Set DLL_REFLECTIONLIB when built as DLL

add_subdirectory(tools/neat_code_gen)

add_subdirectory(tests)


# Little warning, we only support the latest MSVC
if(NOT ${CMAKE_GENERATOR} STREQUAL "Visual Studio 17 2022")
	message(WARNING "[NeatReflection] Only the Visual Studio 17 2022 generator has support for modules. The current generator is: ${CMAKE_GENERATOR}")
endif()
