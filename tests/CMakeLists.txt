set(PROJECT_IS_NEAT_REFLECTION CMAKE_PROJECT_NAME STREQUAL "NeatReflection")
if((PROJECT_IS_NEAT_REFLECTION OR NEAT_REFLECTION_BUILD_TESTING) AND BUILD_TESTING)
	message(STATUS "Configuring NeatReflection tests...")


	# Third party
	FetchContent_Declare(
		Catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG v3.0.0-preview4
	)

	message(STATUS "Fetching third party test dependencies with FetchContent_MakeAvailable. This might take a few minutes...")
	FetchContent_MakeAvailable(Catch2)

	list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
	include(Catch)


	# Tests
	add_library(NeatReflectionTests "TestModule1.ixx" "TestModule1.cpp")
	target_compile_features(NeatReflectionTests PUBLIC cxx_std_23)
	target_link_libraries(NeatReflectionTests PUBLIC NeatReflection)


	# Reflection Data
	get_target_property(NeatReflectionTests__BINARY_DIR NeatReflectionTests BINARY_DIR)
	#get_target_property(NeatReflectionTests__SOURCES NeatReflectionTests SOURCES)
	get_target_property(NeatReflectionTests__NAME NeatReflectionTests NAME)
	#list(FILTER NeatReflectionTests__SOURCES INCLUDE REGEX ".ixx")
	
	add_library(NeatReflectionTests_ReflectionData OBJECT "${NeatReflectionTests__BINARY_DIR}/TestModule1.ixx.cpp")
	add_custom_command(TARGET NeatReflectionTests 
		POST_BUILD COMMAND NeatReflectionCodeGen ARGS "scan" "${NeatReflectionTests__NAME}.dir/Debug/" "."
		WORKING_DIRECTORY "${NeatReflectionTests__BINARY_DIR}")
	# add_dependencies(NeatReflectionTests_ReflectionData NeatReflectionCodeGen)
	target_link_libraries(NeatReflectionTests_ReflectionData PRIVATE NeatReflectionTests)
	target_compile_features(NeatReflectionTests_ReflectionData PUBLIC cxx_std_20)

	set_source_files_properties("${NeatReflectionTests__BINARY_DIR}/TestModule1.ixx.cpp" PROPERTIES GENERATED true)

	add_executable(NeatReflectionTestsExe "TestBasics.cpp")
	target_link_libraries(NeatReflectionTestsExe PUBLIC NeatReflectionTests NeatReflectionTests_ReflectionData)
	target_link_libraries(NeatReflectionTestsExe PRIVATE Catch2::Catch2WithMain)

	# get_target_property(NeatReflectionTests_ARCHIVE_OUTPUT_DIRECTORY_DEBUG NeatReflectionTests ARCHIVE_OUTPUT_DIRECTORY_DEBUG)
	# message(STATUS "ARCHIVE_OUTPUT_DIRECTORY_DEBUG: '${NeatReflectionTests_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}'")
	# get_target_property(NeatReflectionTests_ARCHIVE_OUTPUT_DIRECTORY NeatReflectionTests ARCHIVE_OUTPUT_DIRECTORY)
	# message(STATUS "ARCHIVE_OUTPUT_DIRECTORY: '${NeatReflectionTests_ARCHIVE_OUTPUT_DIRECTORY}'")
	# 
	# get_target_property(NeatReflectionTests_RUNTIME_OUTPUT_DIRECTORY NeatReflectionTests RUNTIME_OUTPUT_DIRECTORY)
	# message(STATUS "RUNTIME_OUTPUT_DIRECTORY: '${NeatReflectionTests_RUNTIME_OUTPUT_DIRECTORY}'")
	# get_target_property(NeatReflectionTests_RUNTIME_OUTPUT_DIRECTORY_DEBUG NeatReflectionTests RUNTIME_OUTPUT_DIRECTORY_DEBUG)
	# message(STATUS "RUNTIME_OUTPUT_DIRECTORY_DEBUG: '${NeatReflectionTests_RUNTIME_OUTPUT_DIRECTORY_DEBUG}'")
	# 
	# get_target_property(NeatReflectionTests_LIBRARY_OUTPUT_DIRECTORY NeatReflectionTests LIBRARY_OUTPUT_DIRECTORY)
	# message(STATUS "LIBRARY_OUTPUT_DIRECTORY: '${NeatReflectionTests_LIBRARY_OUTPUT_DIRECTORY}'")
	# get_target_property(NeatReflectionTests_LIBRARY_OUTPUT_DIRECTORY_DEBUG NeatReflectionTests LIBRARY_OUTPUT_DIRECTORY_DEBUG)
	# message(STATUS "LIBRARY_OUTPUT_DIRECTORY_DEBUG: '${NeatReflectionTests_LIBRARY_OUTPUT_DIRECTORY_DEBUG}'")
	# 
	# get_target_property(NeatReflectionTests_BINARY_DIR NeatReflectionTests BINARY_DIR)
	# message(STATUS "BINARY_DIR: '${NeatReflectionTests_BINARY_DIR}'")

	catch_discover_tests(NeatReflectionTestsExe)
endif()
