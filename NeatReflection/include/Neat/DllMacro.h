#pragma once

#ifdef DLL_REFLECTIONLIB
	#ifdef BUILDING_REFLECTIONLIB
		#define REFL_API __declspec(dllexport)
	#else
		#define REFL_API __declspec(dllimport)
	#endif
#else
	#define REFL_API
#endif
