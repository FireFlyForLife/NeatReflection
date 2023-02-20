# if(${NEAT_REFLECTION_USE_PREBUILT_CODEGEN_EXE})
# 	
# 	message(DEBUG "Using prebuilt NeatReflectionCodeGen.")
# 	set(NEAT_REFLECTION_CODEGEN_EXE "${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/NeatReflectionCodeGen.exe" PARENT_SCOPE)
# 
# else()

	# Third party
	FetchContent_Declare(
		docopt
		GIT_REPOSITORY https://github.com/docopt/docopt.cpp.git
		GIT_TAG v0.6.3
	)
	FetchContent_Declare(
		ifc-reader
		GIT_REPOSITORY https://github.com/AndreyG/ifc-reader.git
		GIT_TAG ca45629ce803a022c6a4a78e5631c0f8d6e3549d # Latest at the time of writing
	)
	FetchContent_Declare(
		magic_enum
		GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
		GIT_TAG v0.8.2
	)
	FetchContent_Declare(
		mio
		GIT_REPOSITORY https://github.com/mandreyel/mio.git
		GIT_TAG 3f86a95c0784d73ce6815237ec33ed25f233b643 # Latest at the time or writing
	)

	message(STATUS "[NeatReflection] Fetching third party code gen dependencies with FetchContent_MakeAvailable. This might take a few minutes...")
	FetchContent_MakeAvailable(docopt ifc-reader magic_enum mio)


	# Executable
	add_executable(NeatReflectionCodeGen "src/Main.cpp" "src/CodeGenerator.h" "src/CodeGenerator.cpp" "src/ContextualException.h" "src/ContextualException.cpp")
	target_compile_features(NeatReflectionCodeGen PUBLIC cxx_std_20)
	target_include_directories(NeatReflectionCodeGen PRIVATE "src")
	target_link_libraries(NeatReflectionCodeGen PUBLIC NeatReflection docopt_s reflifc magic_enum mio)

	message(DEBUG "[NeatReflection] Using NeatReflectionCodeGen built from source.")
	set(NEAT_REFLECTION_CODEGEN_EXE "NeatReflectionCodeGen" PARENT_SCOPE)
	
# endif()
