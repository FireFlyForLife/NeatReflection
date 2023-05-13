# if(${NEAT_REFLECTION_USE_PREBUILT_CODEGEN_EXE})
# 	
# 	message(DEBUG "Using prebuilt NeatReflectionCodeGen.")
# 	set(NEAT_REFLECTION_CODEGEN_EXE "${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/NeatReflectionCodeGen.exe" PARENT_SCOPE)
# 
# else()

	include(FetchContent)

	# Third party
	FetchContent_Declare(
		docopt
		GIT_REPOSITORY https://github.com/docopt/docopt.cpp.git
		GIT_TAG v0.6.3
		FIND_PACKAGE_ARGS
	)
	FetchContent_Declare(
		ifc-reader
		GIT_REPOSITORY https://github.com/FireFlyForLife/ifc-reader.git
		GIT_TAG 25691d13474721c2648484c5fb001efb08c41c5a # Latest at the time of writing
		FIND_PACKAGE_ARGS
	)
	FetchContent_Declare(
		magic_enum
		GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
		GIT_TAG v0.8.2
		FIND_PACKAGE_ARGS
	)
	FetchContent_Declare(
		mio
		GIT_REPOSITORY https://github.com/mandreyel/mio.git
		GIT_TAG 8b6b7d878c89e81614d05edca7936de41ccdd2da # Latest at the time or writing
		FIND_PACKAGE_ARGS
	)
	FetchContent_Declare(
		nlohmann_json 
		GIT_REPOSITORY https://github.com/nlohmann/json.git
		GIT_TAG v3.11.2
		# NOTE: We don't manually call `FetchContent_MakeAvailable()` with nlohmann_json.
		# It's intended to be a fallback for when nlohmann_json is not installed yet for the ifc-msvc dependency.
		OVERRIDE_FIND_PACKAGE
	)

	message(STATUS "[NeatReflection] Fetching third party code gen dependencies with FetchContent_MakeAvailable. This might take a few minutes...")
	FetchContent_MakeAvailable(docopt ifc-reader magic_enum mio)

	add_subdirectory("${ifc-reader_SOURCE_DIR}/lib/msvc" "${ifc-reader_BINARY_DIR}/lib/msvc")


	# Shared Processing Library
	add_subdirectory("libs/ifc_processing_utilities")


	# Executable
	add_executable(NeatReflectionCodeGen "src/Main.cpp" "src/CodeGenerator.h" "src/CodeGenerator.cpp")
	target_compile_features(NeatReflectionCodeGen PUBLIC cxx_std_20)
	target_include_directories(NeatReflectionCodeGen PRIVATE "src")
	target_link_libraries(NeatReflectionCodeGen PUBLIC NeatReflectionIfcProcessingUtilities docopt_s ifc-msvc)
	set_target_properties(NeatReflectionCodeGen PROPERTIES FOLDER "Neat/Tools")

	message(DEBUG "[NeatReflection] Using NeatReflectionCodeGen built from source.")
	set(NEAT_REFLECTION_CODEGEN_EXE "NeatReflectionCodeGen" PARENT_SCOPE)
	
# endif()
