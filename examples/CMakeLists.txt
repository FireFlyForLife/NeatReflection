set(PROJECT_IS_NEAT_REFLECTION CMAKE_PROJECT_NAME STREQUAL "NeatReflection")
if(NEAT_REFLECTION_BUILD_EXAMPLES OR PROJECT_IS_NEAT_REFLECTION)

	message(STATUS "[NeatReflection] Configuring NeatReflection examples...")

	include(FetchContent)


	# Third party
	FetchContent_Declare(
		flecs
		GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
		GIT_TAG v3.2.3
		FIND_PACKAGE_ARGS
	)

	FetchContent_Declare(
		nlohmann_json
		GIT_REPOSITORY https://github.com/nlohmann/json.git
		GIT_TAG v3.11.2
		FIND_PACKAGE_ARGS
	)

	message(STATUS "[NeatReflection] Fetching third party example dependencies with FetchContent_MakeAvailable. This might take a few minutes...")
	FetchContent_MakeAvailable(flecs nlohmann_json)
	

	# Examples
	add_subdirectory(simple)
	add_subdirectory(serialisation) # Requires nlohmann_json

endif()
