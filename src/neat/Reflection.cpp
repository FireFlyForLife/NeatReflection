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
		if (it != type_container.by_type_name.end())
		{
			return &type_container.types[it->second];
		}
		return nullptr;
	}

	const Type* get_type(TemplateTypeId type_id)
	{
		auto it = type_container.by_template_type_id.find(type_id);
		if (it != type_container.by_template_type_id.end())
		{
			return &type_container.types[it->second];
		}
		return nullptr;
	}
}
