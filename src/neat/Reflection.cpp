#include "neat/Reflection.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <cassert>


namespace Neat
{
	struct TypeContainer
	{
		struct string_hash : std::hash<std::string_view> 
		{
			using is_transparent = std::true_type;
		};

		std::unordered_map<std::string, uint32_t, string_hash, std::equal_to<>> by_type_name;
		std::unordered_map<TemplateTypeId, uint32_t> by_template_type_id;
		std::vector<Type> types;
	};
	static TypeContainer type_container;


	Type& add_type(Type&& type)
	{
		auto type_by_id_it = type_container.by_template_type_id.find(type.id);
		if (type_by_id_it != type_container.by_template_type_id.end())
		{
			return type_container.types[type_by_id_it->second];
		}

		type_container.by_type_name[type.name] = type_container.types.size();
		type_container.by_template_type_id[type.id] = type_container.types.size();
		type_container.types.push_back(std::move(type));
		return type_container.types.back();
	}

	std::span<const Type> get_types()
	{
		return { type_container.types.begin(), type_container.types.end() };
	}

	const Type* get_type(std::string_view type_name)
	{
		auto it = type_container.by_type_name.find(type_name);
		if (it == type_container.by_type_name.end())
		{
			return nullptr;
		}
		return &type_container.types[it->second];
	}

	const Type* get_type(TemplateTypeId type_id)
	{
		auto it = type_container.by_template_type_id.find(type_id);
		if (it == type_container.by_template_type_id.end())
		{
			return nullptr;
		}
		return &type_container.types[it->second];
	}


	std::strong_ordering Type::operator<=>(const Type& other) const noexcept
	{
		return id <=> other.id;
	}

	std::strong_ordering Field::operator<=>(const Field& other) const noexcept
	{
		std::strong_ordering order;

		order = (object_type <=> other.object_type);
		if (order != 0) { return order; }
		order = (name <=> other.name);

		return order;
	}

	std::strong_ordering Method::operator<=>(const Method& other) const noexcept
	{
		std::strong_ordering order;

		order = (object_type <=> other.object_type);
		if (order != 0) { return order; }
		order = (return_type <=> other.return_type);
		if (order != 0) { return order; }
		order = (argument_types <=> other.argument_types);
		if (order != 0) { return order; }
		order = (name <=> other.name);

		return order;
	}
}

namespace std
{
	size_t hash<Neat::Type>::operator()(const Neat::Type& type) const noexcept 
	{
		return type.id;
	}

	size_t hash<Neat::Field>::operator()(const Neat::Field& field) const 
	{
		size_t h = 0;
		Neat::HashUtils::combine(h, field.object_type, field.name);
		return h;
	}

	size_t hash<Neat::Method>::operator()(const Neat::Method& method) const 
	{
		size_t h = 0;

		for (const auto& argument_type : method.argument_types)
		{
			Neat::HashUtils::combine(h, argument_type);
		}
		Neat::HashUtils::combine(h, method.object_type, method.return_type, method.name);

		return h;
	}

	size_t hash<Neat::BaseClass>::operator()(const Neat::BaseClass& base_class) const noexcept 
	{
		return base_class.base_id;
	}
}
