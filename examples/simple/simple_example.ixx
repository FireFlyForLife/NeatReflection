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
    using namespace Neat;

    MyStruct my_struct{ .damage = -5 };
    const Type* type = Neat::get_type<MyStruct>();
    AnyPtr my_struct_ptr{ &my_struct, type->id };

    const Field& field = type->fields[0];
    assert(field.name == "damage");

    Any damage_any = field.get_value(my_struct_ptr);
    int* damage = damage_any.value_ptr<int>();
    assert(damage != nullptr && *damage == -5);

    field.set_value(my_struct_ptr, 75);
    assert(my_struct.damage == 75);

    const Method& method = type->methods[0];
    assert(method.name == "get_42");

    Any value_any = method.invoke(my_struct_ptr, {});
    int* value = value_any.value_ptr<int>();
    assert(value != nullptr && *value == 42);
}
