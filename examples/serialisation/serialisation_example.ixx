module;
#include "neat/Reflection.h"

#include "nlohmann/json.hpp"

#include <charconv>
#include <string>
#include <iostream>
#include <cassert>
export module SerialisationExample;

// Types
// ============================================================================

export struct MyData {
    std::string name;
    int health;
    double damage;

    std::vector<int> ids;
    std::vector<std::string> items;
};

using json = nlohmann::json;


// Functions
// ============================================================================

json serialise(std::any object)
{
    if (auto typed_object = std::any_cast<std::reference_wrapper<int>>(&object))
    {
        return typed_object->get();
    }
    else if (auto typed_object = std::any_cast<std::reference_wrapper<double>>(&object))
    {
        return typed_object->get();
    }
    else if (auto typed_object = std::any_cast<std::reference_wrapper<std::string>>(&object))
    {
        return typed_object->get();
    }
    else
    {
        return json{};
    }
}

void deserialise(void* object, const Neat::Field& field, const json& data)
{
    if (data.is_null())
    {
        return;
    }

    std::any value{};

    if (field.type == Neat::get_id<int>()) 
    {
        value = data.get<int>();
    }
    else if (field.type == Neat::get_id<bool>())
    {
        value = data.get<bool>();
    }
    else if (field.type == Neat::get_id<float>())
    {
        value = data.get<float>();
    }
    else if (field.type == Neat::get_id<double>()) 
    {
        value = data.get<double>();
    }
    else if (field.type == Neat::get_id<std::string>()) 
    {
        value = data.get<std::string>();
    }
    //else if (data.is_array()) {
    //    std::vector<std::any> vec;
    //    for (const auto& element : data) {
    //        vec.push_back(jsonToAny(element));
    //    }
    //    return vec;
    //}
    //else if (data.is_object()) {
    //    std::map<std::string, std::any> obj;
    //    for (auto it = data.begin(); it != data.end(); ++it) {
    //        obj[it.key()] = jsonToAny(it.value());
    //    }
    //    return obj;
    //}

    field.set_value(object, value);
}

export void serialisation_example()
{
    MyData data{
        .name = "Hello Object!",
        .health = 100,
        .damage = 15.5,
        .ids = {3, 5, 7, 11},
        .items = {"Bucket", "Battery", "Shovel"}
    };

    const Neat::Type* type = Neat::get_type<MyData>();

    json field_data{};
    for (const auto& field : type->fields)
    {
        field_data.emplace(field.name, serialise(field.get_reference(&data)));
    }

    std::cout << type->name << ": " << field_data << '\n';


    // TODO: Call constructor from reflection
    MyData object{};

    for (auto& field : type->fields)
    {
        deserialise(&object, field, field_data[field.name]);
    }

    std::cout << "End!\n";
}
