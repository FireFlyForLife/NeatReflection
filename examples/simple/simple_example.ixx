module;
#include "neat/Reflection.h"
#include <cassert>
export module SimpleExample;

// Types
// ============================================================================

export struct MyStruct {
    int damage;

    int get_42() { return 42; }
};


// Functions
// ============================================================================

export void simple_example()
{
    MyStruct my_struct{ .damage = -5 };

    const Neat::Type* type = Neat::get_type<MyStruct>();

    const Neat::Field& field = type->fields[0];
    assert(field.name == "damage");

    std::any damage_any = field.get_reference(&my_struct);
    auto damage = std::any_cast<std::reference_wrapper<int>>(&damage_any);
    assert(damage != nullptr && *damage == -5);

    field.set_value(&my_struct, 75);
    assert(my_struct.damage == 75);

    const Neat::Method& method = type->methods[0];
    assert(method.name == "get_42");

    std::any value_any = method.invoke(&my_struct, {});
    int* value = std::any_cast<int>(&value_any);
    assert(value != nullptr && *value == 42);
}
