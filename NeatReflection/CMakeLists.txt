# TODO: Set DLL_REFLECTIONLIB when built as DLL
add_library (NeatReflection 
	"src/Neat/Reflection.cpp"
	"src/Neat/TemplateTypeId.cpp"
	"include/Neat/Reflection.h"
	"include/Neat/TemplateTypeId.h"
	"include/Neat/DllMacro.h" 
	"include/Neat/ReflectPrivateMembers.h")
target_compile_features(NeatReflection PUBLIC cxx_std_20)
target_include_directories(NeatReflection PUBLIC "include")
target_compile_definitions(NeatReflection PRIVATE BUILDING_REFLECTIONLIB=1)
