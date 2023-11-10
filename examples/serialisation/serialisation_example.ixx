export module SerialisationExample;
import "neat/Reflection.h";
import "nlohmann/json.hpp";
import <string>;
import <vector>;
import <iostream>;
import <cassert>;

// Types
// ============================================================================

export struct SubObject {
    int id;
    double factor;
};

export struct MyData {
    std::string name;
    int health;
    double damage;
    SubObject sub_object;

    std::vector<int> ids;
    std::vector<std::string> items;
};

using json = nlohmann::json;


// Functions
// ============================================================================

json serialise(Neat::AnyPtr object)
{
    if (object.type == Neat::get_type<int>())
    {
        return *static_cast<int*>(object.value_ptr);
    }
    else if (object.type == Neat::get_type<float>())
    {
        return *static_cast<float*>(object.value_ptr);
    }
    else if (object.type == Neat::get_type<double>())
    {
        return *static_cast<double*>(object.value_ptr);
    }
    else if (object.type == Neat::get_type<std::string>())
    {
        return *static_cast<std::string*>(object.value_ptr);
    }
    else
    {
        return json{};
    }
}

void deserialise(Neat::AnyPtr object, const Neat::Field& field, const json& data)
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
    Neat::AnyPtr my_data_serialisation_ptr{ &data, type };

    // Serialise
    json field_data{};
    for (const auto& field : type->fields)
    {
        field_data.emplace(field.name, serialise(field.get_address(my_data_serialisation_ptr)));
    }

    std::cout << type->name << ": " << field_data << '\n';

    // Deserialise
    MyData object{}; // TODO: Call constructor from reflection
    Neat::AnyPtr my_data_deserialisation_ptr{ &object, type };

    for (auto& field : type->fields)
    {
        deserialise(my_data_deserialisation_ptr, field, field_data[field.name]);
    }

    std::cout << "End!\n";
}
