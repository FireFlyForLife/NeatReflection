# NeatReflection
A neat and tidy reflection system. Based on C++20 modules output (.ifc)

## How does it work?
After C++20 modules are compiled, they are stored in a Binary Module Interface (BMI) file. This file will contain all the semantic information about the types exposed in that module. We parse this information and codegen C++ which registers each type and it's members.

## Advantages over alternatives
**Little build system dependencies**: include dirs & preprocessor defines don't need to be propagated to this system. All we need are the module interface build artifacts, which are produced during normal compilation.

**Fast**: Utilizes existing compiler outputs (BMI files) to generate reflection information. Each file produces it's own C++ source file (with reflection info). Thus incremental builds will stay fast. 

## API Example
```cpp
export struct MyStruct {
    int damage;

    int get_42() { return 42; }
};

void example() {
    MyStruct my_struct{ .damage = -5 };
    const Neat::Type* type = Neat::get_type<MyStruct>();
    Neat::AnyPtr my_struct_ptr{ &my_struct, type };

    const Neat::Field& field = type->fields[0];
    assert(field.name == "damage");

    auto damage_any = field.get_value(my_struct_ptr);
    auto damage = std::any_cast<int>(&damage_any);
    assert(damage != nullptr && *damage == -5);

    field.set_value(my_struct_ptr, 75);
    assert(my_struct.damage == 75);

    const Neat::Method& method = type->methods[0];
    assert(method.name == "get_42");

    auto value_any = method.invoke(my_struct_ptr, {});
    auto value = std::any_cast<int>(&value_any);
    assert(value != nullptr && *value == 42);
}
```

## CMake integration
Currently only CMake integration is builtin. But extending it to other build systems should be fairly trivial.
 
Small CMake example:
```cmake
add_library(MyCode "MyModule.ixx" "MyModule.cpp")
target_compile_features(MyCode PUBLIC cxx_std_20)
target_link_libraries(MyCode PUBLIC NeatReflection)

add_reflection_target(MyCode_ReflectionData MyCode)

add_executable(MyExe PRIVATE "main.cpp")
target_link_libraries(MyExe PRIVATE MyCode MyCode_ReflectionData)
```
