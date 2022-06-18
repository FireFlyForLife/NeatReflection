set(PROJECT_IS_NEAT_REFLECTION CMAKE_PROJECT_NAME STREQUAL "NeatReflection")
if((PROJECT_IS_NEAT_REFLECTION OR NEAT_REFLECTION_BUILD_TESTING) AND BUILD_TESTING)
	message(STATUS "Configuring NeatReflection tests...")


	# Third party
	FetchContent_Declare(
		Catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG v3.0.1
	)

	message(STATUS "Fetching third party test dependencies with FetchContent_MakeAvailable. This might take a few minutes...")
	FetchContent_MakeAvailable(Catch2)

	list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras") # Add catch2's `extra` folder to CMake module includes
	include(Catch) # Defines `catch_discover_tests()` 

	target_compile_features(Catch2 PUBLIC cxx_std_17) # Enables support for string_views etc
	target_compile_features(Catch2WithMain PUBLIC cxx_std_17) # For good measure on the main too...


	# Tests
	add_library(NeatReflectionTests "TestModule1.ixx" "TestModule1.cpp")
	target_compile_features(NeatReflectionTests PUBLIC cxx_std_20)
	target_link_libraries(NeatReflectionTests PUBLIC NeatReflection)

	add_reflection_target(NeatReflectionTests_ReflectionData NeatReflectionTests)

	add_executable(NeatReflectionTestsExe "TestBasics.cpp")
	target_compile_features(NeatReflectionTestsExe PUBLIC cxx_std_20)
	target_link_libraries(NeatReflectionTestsExe PUBLIC NeatReflectionTests NeatReflectionTests_ReflectionData)
	target_link_libraries(NeatReflectionTestsExe PRIVATE Catch2::Catch2WithMain)

	catch_discover_tests(NeatReflectionTestsExe)
endif()
