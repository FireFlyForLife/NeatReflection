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

int main() {
    MyStruct my_struct{ .damage = -5 };

    Neat::Type* type = Neat::get_type<MyStruct>();

    auto& field = type->fields[0];
    assert(field.name == "damage");

    auto old_damage = field.get_value(&my_struct);
    auto old_damage_int = std::any_cast<int>(&value);
    assert(old_damage_int != nullptr && *old_damage_int == -5);

    field.set_value(&my_struct, 75);
    assert(my_struct.damage == 75);

    auto& method = type->methods[0];
    assert(method.name == "get_42");

    auto value = method.invoke(&my_struct, {});
    auto value_int = std::any_cast<int>(&value);
    assert(value_int != nullptr && *value_int == 42);
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
