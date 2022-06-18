# NeatReflection
A neat and tidy reflection system. Based on C++20 modules output (.ifc)

## How does it work?
After C++20 modules are compiled, they are stored in a Binary Module Interface (BMI) file. This file will contain all the semantic information about the types exposed in that module. We parse this information and codegen C++ which registers each type and it's members.

## Advantages over alternatives
**Little build system dependencies**: include dirs & preprocessor defines don't need to be propagated to this system. All we need are the module interface build artifacts, which are produced during normal compilation.

**Fast**: Utilizes existing compiler outputs (BMI files) to generate reflection information. Each file produces it's own C++ source file (with reflection info). Thus incremental builds will stay fast. 

## Future tasks
 - [] Expose the Reflection library as a module instead of #include
 - [] IFC-reader library pulls in Boost just for memory mapping the file. This dependency should be removed
 - [] Add support for GCC (via .cmi) 
 - [] Clang (via clang's BMI solution)
 - [] User Attributes are not exposed in .ifc 
