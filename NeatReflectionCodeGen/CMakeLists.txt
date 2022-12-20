if(${NEAT_REFLECTION_USE_PREBUILT_CODEGEN_EXE})
	
	message(DEBUG "Using prebuilt NeatReflectionCodeGen.")
	set(NEAT_REFLECTION_CODEGEN_EXE "${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/NeatReflectionCodeGen.exe" PARENT_SCOPE)

else()

	# Third party
	FetchContent_Declare(
		docopt
		GIT_REPOSITORY https://github.com/docopt/docopt.cpp.git
		GIT_TAG v0.6.3
	)
	FetchContent_Declare(
		ifc-reader
		GIT_REPOSITORY https://github.com/AndreyG/ifc-reader.git
		GIT_TAG 13214adb6b1d6fc57aa652f6248e499b112332fe # Latest at the time of writing
	)

	message(STATUS "Fetching third party code gen dependencies with FetchContent_MakeAvailable. This might take a few minutes...")
	FetchContent_MakeAvailable(docopt ifc-reader)


	# Executable
	add_executable(NeatReflectionCodeGen "src/Main.cpp" "include/CodeGenerator.h" "src/CodeGenerator.cpp" "include/ContextualException.h" "src/ContextualException.cpp")
	target_compile_features(NeatReflectionCodeGen PUBLIC cxx_std_20)
	target_include_directories(NeatReflectionCodeGen PUBLIC "include")
	target_link_libraries(NeatReflectionCodeGen PUBLIC NeatReflection docopt_s ifc-core)

	message(DEBUG "Using NeatReflectionCodeGen built from source.")
	set(NEAT_REFLECTION_CODEGEN_EXE "NeatReflectionCodeGen" PARENT_SCOPE)

endif()
