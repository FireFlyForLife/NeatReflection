#pragma once
#include "Neat/DllMacro.h"
#include "Neat/TemplateTypeId.h"

#include <any>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <memory>
#include <span>
#include <cassert>

// Forward Declarations
namespace Neat
{
	struct Type;
	struct Field;
	struct Method;
}


namespace Neat
{
	// Functions
	// ===========================================================================

	REFL_API Type& add_type(Type&&);

	REFL_API Type* get_type(std::string_view type_name);
	REFL_API Type* get_type(TemplateTypeId type_id);
	template<typename T>
	Type* get_type() { return get_type(get_id<T>()); }


	// Types
	// ===========================================================================

	struct REFL_API Type
	{
		std::string name;
		TemplateTypeId id;
		std::vector<std::string> bases;
		std::vector<std::unique_ptr<Field>> fields;
		std::vector<std::unique_ptr<Method>> methods;
	};

	struct REFL_API Field
	{
		virtual ~Field() = default;

		virtual std::any get(void* object) = 0;
		virtual void set(void* object, std::any value) = 0;

		Type* object_type;
		Type* type;
		std::string name;
		std::vector<std::string> attributes;
	};

	struct REFL_API Method
	{
		virtual ~Method() = default;

		virtual std::any invoke(void* object, std::span<std::any> arguments) = 0;

		Type* object_type;
		Type* return_type;
		std::string name;
		std::vector<Type*> argument_types;
	};
}


// Implementation
// ===========================================================================

namespace Neat::Detail
{
	template<typename TObject, typename TType, TType TObject::* ptr_to_member>
	struct FieldImpl final : Field
	{
		FieldImpl(std::string_view name)
		{
			this->object_type = get_type<TObject>();
			this->type = get_type<TType>();
			this->name = name;
		}

		std::any get(void* object) final
		{
			TObject* object_ = reinterpret_cast<TObject*>(object);
			return { object_->*ptr_to_member };
		}

		void set(void* object, std::any value) final
		{
			assert(std::any_cast<TType>(&value) != nullptr);

			TObject* object_ = reinterpret_cast<TObject*>(object);
			object_->*ptr_to_member = std::any_cast<TType>(value);
		}
	};


	template<typename TObject, typename TReturn, typename... TArgs>
	struct MethodImpl final : Method
	{
		using PtrToMemberFunc = TReturn (TObject::*)(TArgs...);

		MethodImpl(std::string_view name, PtrToMemberFunc ptr_to_memberfunc)
		{
			this->object_type = get_type<TObject>();
			this->return_type = get_type<TReturn>();
			this->name = name;
			this->ptr_to_memberfunc = ptr_to_memberfunc;
		}

		std::any invoke(void* object, std::span<std::any> arguments) final
		{
			assert(arguments.size() == sizeof...(TArgs));
			// TODO: Validate arg types

			TObject* object_ = reinterpret_cast<TObject*>(object);
			auto arg_it = arguments.begin();

			if constexpr (std::is_same_v<TReturn, void>)
			{
				(object_->*ptr_to_memberfunc)(std::any_cast<TArgs>(*(arg_it++))...);
				return {};
			}
			else
			{
				return { (object_->*ptr_to_memberfunc)(std::any_cast<TArgs>(*(arg_it++))...) };
			}
		}

		PtrToMemberFunc ptr_to_memberfunc;
	};
}