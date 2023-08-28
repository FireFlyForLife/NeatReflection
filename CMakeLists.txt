cmake_minimum_required (VERSION 3.26)

project (NeatReflection)

include(FetchContent)
include(CTest)
include(extras/BMIPath.cmake)


# User Options
option(NEAT_REFLECTION_BUILD_TESTING			"Enable tests for NeatReflection" OFF)
option(NEAT_REFLECTION_BUILD_EXAMPLES			"Enable examples for NeatReflection" OFF)
# option(NEAT_REFLECTION_USE_PREBUILT_CODEGEN_EXE "Don't compile NeatReflectionCodeGen from source but use the prebuilt binary." ON)


# Public CMake Functions

#[[
# @brief	Create a new CMake target named `reflection_data_target_name`, which will contain reflection data from target `target_name`
#
# @argument	reflection_data_target_name		Will be the name for the new target, which will include the reflection data.
# @argument target_name						The name of the target with C++20 code you want to generate reflection data for.
#
# @example	To use this reflection name simply link it with you exe or dll:
#	add_reflection_target(MyAwesomeLibrary_ReflectionData MyAwesomeLibrary)
#	target_link_libraries(MyExeTarget MyAwesomeLibrary MyAwesomeLibrary_ReflectionData)
#]]
function(add_reflection_target reflection_data_target_name target_name)

    # Gather target properties
    get_target_property(_TARGET__BINARY_DIR ${target_name} BINARY_DIR)
    get_target_property(_TARGET__SOURCES ${target_name} SOURCES)
    get_target_property(_TARGET__NAME ${target_name} NAME)
    get_target_property(_TARGET__LINK_LIBRARIES ${target_name} LINK_LIBRARIES)
    
    # Filter to get all module interface files
    list(FILTER _TARGET__SOURCES INCLUDE REGEX ".ixx")
    foreach(_TARGET__SOURCE ${_TARGET__SOURCES})
        
        # Set the output filename based on the source filename.
        cmake_path(GET _TARGET__SOURCE FILENAME _TARGET__SOURCE_FILENAME)
        set(_REFLECTION_TARGET_SOURCE "${_TARGET__BINARY_DIR}/${_TARGET__SOURCE_FILENAME}.cpp")
        list(APPEND _REFLECTION_TARGET_SOURCES ${_REFLECTION_TARGET_SOURCE})

        # Guess the BMI (.ifc) filename based on the source filename.
        guess_bmi_path(${target_name} ${_TARGET__SOURCE} _TARGET__BMI_FILE) # Function from: "extras/BMIPath.cmake"

        # Define the post build command to run the code generator.
        add_custom_command(OUTPUT ${_REFLECTION_TARGET_SOURCE}
            COMMAND ${NEAT_REFLECTION_CODEGEN_EXE} ARGS "${_TARGET__BMI_FILE}" "${_REFLECTION_TARGET_SOURCE}"
            WORKING_DIRECTORY "${_TARGET__BINARY_DIR}"
            DEPENDS "${_TARGET__SOURCE}" "${_TARGET__BMI_FILE}" "${NEAT_REFLECTION_CODEGEN_EXE}")

    endforeach()
    
    # Setup the new target which will contain the reflection data.
    add_library(${reflection_data_target_name} OBJECT ${_REFLECTION_TARGET_SOURCES})
    set_source_files_properties(${_REFLECTION_TARGET_SOURCES} PROPERTIES GENERATED true)
    target_link_libraries(${reflection_data_target_name} PRIVATE NeatReflection ${target_name})
    target_compile_features(${reflection_data_target_name} PUBLIC cxx_std_20)
    set_target_properties(${reflection_data_target_name} PROPERTIES FOLDER "Neat/GeneratedTargets")

endfunction()


# Sources
add_library (NeatReflection
    "include/neat/Reflection.h"
    "include/neat/TemplateTypeId.h"
    "include/neat/DllExportMacro.h"
    "include/neat/ReflectPrivateMembers.h"
    "include/neat/Any.h"
    "src/neat/Reflection.cpp"
    "src/neat/TemplateTypeId.cpp"
    "src/neat/Any.cpp")
target_compile_features(NeatReflection PUBLIC cxx_std_20)
target_include_directories(NeatReflection PUBLIC "include")
target_compile_definitions(NeatReflection PRIVATE BUILDING_REFLECTIONLIB=1) # TODO: Set DLL_REFLECTIONLIB when built as DLL
set_target_properties(NeatReflection PROPERTIES FOLDER "Neat")

add_subdirectory(tools/neat_code_gen)

add_subdirectory(tests)

add_subdirectory(examples)


# Little warning, we only support the latest MSVC
if(NOT ${CMAKE_GENERATOR} STREQUAL "Visual Studio 17 2022")
    message(WARNING "[NeatReflection] Only the Visual Studio 17 2022 generator has support for modules. The current generator is: ${CMAKE_GENERATOR}")
endif()

cmake_policy(SET CMP0143 NEW) # USE_FOLDERS global property is treated as ON by default.
