#include "Neat/Reflection.h"

#include <unordered_map>
#include <vector>
#include <string>


namespace Neat
{
	struct TypeContainer
	{
		std::unordered_map<std::string, uint32_t> by_type_name;
		std::unordered_map<TemplateTypeId, uint32_t> by_template_type_id;
		std::vector<Type> types;
	};
	static TypeContainer type_container;


	Type& add_type(Type&& type)
	{
		type_container.by_type_name[type.name] = type_container.types.size();
		type_container.by_template_type_id[type.id] = type_container.types.size();
		type_container.types.push_back(std::move(type));
		return type_container.types.back();
	}

	std::span<Type> Neat::get_types()
	{
		return { type_container.types.begin(), type_container.types.end() };
	}

	Type* get_type(std::string_view type_name)
	{
		auto it = type_container.by_type_name.find(std::string{ type_name }); // TODO: Avoid allocation
		if (it == type_container.by_type_name.end())
		{
			return nullptr;
		}
		return &type_container.types[it->second];
	}

	Type* get_type(TemplateTypeId type_id)
	{
		auto it = type_container.by_template_type_id.find(type_id);
		if (it == type_container.by_template_type_id.end())
		{
			return nullptr;
		}
		return &type_container.types[it->second];
	}
}
