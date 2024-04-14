# if(${NEAT_REFLECTION_USE_PREBUILT_CODEGEN_EXE})
# 	
# 	message(DEBUG "Using prebuilt NeatReflectionCodeGen.")
# 	set(NEAT_REFLECTION_CODEGEN_EXE "${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/NeatReflectionCodeGen.exe" PARENT_SCOPE)
# 
# else()

	include(FetchContent)

	# Third party
	FetchContent_Declare(
		ifc-reader
		GIT_REPOSITORY https://github.com/FireFlyForLife/ifc-reader.git
		GIT_TAG e4d9cc54276ccd00f39249c90712823f97bb06ea # Latest at the time of writing
		FIND_PACKAGE_ARGS
	)
	FetchContent_Declare(
		mio
		GIT_REPOSITORY https://github.com/mandreyel/mio.git
		GIT_TAG 8b6b7d878c89e81614d05edca7936de41ccdd2da # Latest at the time or writing
		FIND_PACKAGE_ARGS
	)
	FetchContent_Declare(
		nlohmann_json # We don't directly use nlohmann_json in our tools, but we provide it for the dependency: `ifc-msvc`
		GIT_REPOSITORY https://github.com/nlohmann/json.git
		GIT_TAG v3.11.2
		FIND_PACKAGE_ARGS
	)

	message(STATUS "[NeatReflection] Fetching third party code gen dependencies with FetchContent_MakeAvailable. This might take a few minutes...")
	FetchContent_MakeAvailable(ifc-reader mio nlohmann_json)

	add_subdirectory("${ifc-reader_SOURCE_DIR}/lib/msvc" "${ifc-reader_BINARY_DIR}/lib/msvc") # Build `ifc-msvc` component from the `ifc-reader` repository


	# Shared Processing Library
	add_subdirectory("libs/ifc_processing_utilities")


	# Executable
	add_executable(NeatReflectionCodeGen "src/Main.cpp" "src/CodeGenerator.h" "src/CodeGenerator.cpp")
	target_compile_features(NeatReflectionCodeGen PUBLIC cxx_std_20)
	target_include_directories(NeatReflectionCodeGen PRIVATE "src")
	target_link_libraries(NeatReflectionCodeGen PUBLIC NeatReflectionIfcProcessingUtilities ifc-msvc)
	set_target_properties(NeatReflectionCodeGen PROPERTIES FOLDER "Neat/Tools")

	message(DEBUG "[NeatReflection] Using NeatReflectionCodeGen built from source.")
	set(NEAT_REFLECTION_CODEGEN_EXE "NeatReflectionCodeGen" PARENT_SCOPE)
	
# endif()
