set(PROJECT_IS_NEAT_REFLECTION CMAKE_PROJECT_NAME STREQUAL "NeatReflection")
if((PROJECT_IS_NEAT_REFLECTION OR NEAT_REFLECTION_BUILD_TESTING) AND BUILD_TESTING)
	
	message(STATUS "[NeatReflection] Configuring NeatReflection tests...")

	include(FetchContent)


	# Third party
	FetchContent_Declare(
		Catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG v3.3.2
		FIND_PACKAGE_ARGS
	)

	message(STATUS "[NeatReflection] Fetching third party test dependencies with FetchContent_MakeAvailable. This might take a few minutes...")
	FetchContent_MakeAvailable(Catch2)

	list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras") # Add catch2's `extra` folder to CMake module includes
	include(Catch) # Defines `catch_discover_tests()` 

	target_compile_features(Catch2 PUBLIC cxx_std_17) # Enables support for string_views etc
	target_compile_features(Catch2WithMain PUBLIC cxx_std_17) # For good measure on the main too...


	# Tests
	add_library(NeatReflectionTestingTypes "modules_code/TestModule1.ixx" "modules_code/TestModule1.cpp" "modules_code/folder/TestModule2.ixx" "modules_code/folder/TestModule2.cpp" "modules_code/DependantModule.ixx" "modules_code/TestModuleWithTemplates.ixx")
	target_compile_features(NeatReflectionTestingTypes PUBLIC cxx_std_20)
	target_link_libraries(NeatReflectionTestingTypes PUBLIC NeatReflection)
	set_target_properties(NeatReflectionTestingTypes PROPERTIES FOLDER "Neat/Tests")

	add_reflection_target(NeatReflectionTestingTypes_ReflectionData NeatReflectionTestingTypes)

	add_library(NeatReflectionSomeMoreTestingTypes "more_modules_code/TestModuleUsingOtherLibrariesModule.ixx")
	target_compile_features(NeatReflectionSomeMoreTestingTypes PUBLIC cxx_std_20)
	target_link_libraries(NeatReflectionSomeMoreTestingTypes PUBLIC NeatReflection NeatReflectionTestingTypes)
	set_target_properties(NeatReflectionSomeMoreTestingTypes PROPERTIES FOLDER "Neat/Tests")

	add_reflection_target(NeatReflectionSomeMoreTestingTypes_ReflectionData NeatReflectionSomeMoreTestingTypes)

	add_executable(NeatReflectionTestRunner "test_runner/TestBasics.cpp" "test_runner/TestMethods.cpp" "test_runner/TestHashAndComparison.cpp" "test_runner/TestExternalReference.cpp" "test_runner/TestTemplateTypeId.cpp" "test_runner/TestAny.cpp" "test_runner/TestAliases.cpp" "test_runner/TestTemplateArgs.cpp")
	target_compile_features(NeatReflectionTestRunner PUBLIC cxx_std_20)
	target_link_libraries(NeatReflectionTestRunner PUBLIC NeatReflectionTestingTypes NeatReflectionTestingTypes_ReflectionData)
	target_link_libraries(NeatReflectionTestRunner PRIVATE Catch2::Catch2WithMain)
	set_target_properties(NeatReflectionTestRunner PROPERTIES FOLDER "Neat/Tests")
	catch_discover_tests(NeatReflectionTestRunner)

endif()
