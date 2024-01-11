// Neat reflection preprocessor defines.
#pragma once

// Dll export / import macro
#ifdef DLL_REFLECTIONLIB
	#ifdef BUILDING_REFLECTIONLIB
		#define REFL_API __declspec(dllexport)
	#else
		#define REFL_API __declspec(dllimport)
	#endif
#else
	#define REFL_API
#endif

// Compiler identifier macro
#ifdef _MSC_VER
	#define REFL_COMPILER_MSVC
#else
	#warning "Cannot deduce compiler"
#endif

// C++ language level RTTI macro
#ifdef REFL_COMPILER_MSVC
	#ifdef _CPPRTTI
		#define REFL_CPP_LANG_RTTI
	#endif
#else
	#warning "Cannot infer c++ language level RTTI enabledness"
#endif
