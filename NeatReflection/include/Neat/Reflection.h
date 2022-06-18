#pragma once
#include "Neat/DllMacro.h"
#include "Neat/TemplateTypeId.h"

#include <any>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
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

	REFL_API std::span<Type> get_types();
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
		std::vector<TemplateTypeId> bases;
		std::vector<Field> fields;
		std::vector<Method> methods;
	};

	struct REFL_API Field
	{
		// Functions
		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		static Field create(std::string_view name);

		using GetValueFunction = std::any (*)(void* object);
		using SetValueFunction = void (*)(void* object, std::any value);

		GetValueFunction get_value;
		SetValueFunction set_value;

		// Data
		TemplateTypeId object_type;
		TemplateTypeId type;
		std::string name;
		std::vector<std::string> attributes; // Unused currently
	};

	struct REFL_API Method
	{
		// Functions
		template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename... TArgs>
		static Method create(std::string_view name);

		using InvokeFunction = std::any (*)(void* object, std::span<std::any> arguments);
		
		InvokeFunction invoke;

		// Data
		TemplateTypeId object_type;
		TemplateTypeId return_type;
		std::string name;
		std::vector<TemplateTypeId> argument_types;
		std::vector<std::string> attributes; // Unused currently
	};
}


// Implementation
// ===========================================================================

namespace Neat
{
	namespace Detail
	{
		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		std::any get_value_erased(void* object)
		{
			TObject* object_ = reinterpret_cast<TObject*>(object);
			return { object_->*PtrToMember };
		}

		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		void set_value_erased(void* object, std::any value)
		{
			assert(std::any_cast<TType>(&value) != nullptr);

			TObject* object_ = reinterpret_cast<TObject*>(object);
			object_->*PtrToMember = std::any_cast<TType>(value);
		}
	}

	template<typename TObject, typename TType, TType TObject::* PtrToMember>
	Field Field::create(std::string_view name)
	{
		return Field{
			.get_value = &Detail::get_value_erased<TObject, TType, PtrToMember>,
			.set_value = &Detail::set_value_erased<TObject, TType, PtrToMember>,
			.object_type = get_id<TObject>(),
			.type = get_id<TType>(),
			.name = std::string{ name }
		};
	}

	namespace Detail
	{
		template<typename T, typename E>
		struct IsSame { constexpr static bool value = false; };

		template<typename T>
		struct IsSame<T, T> { constexpr static bool value = true; };

		template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename ...TArgs>
		std::any invoke_erased(void* object, std::span<std::any> arguments)
		{
			assert(arguments.size() == sizeof...(TArgs));
			// TODO: Validate arg types

			TObject* object_ = reinterpret_cast<TObject*>(object);
			auto arg_it = arguments.begin();

			if constexpr (std::is_same_v<TReturn, void>)
			{
				(object_->*PtrToMemberFunction)(std::any_cast<TArgs>(*(arg_it++))...);
				return {};
			}
			else
			{
				return { (object_->*PtrToMemberFunction)(std::any_cast<TArgs>(*(arg_it++))...) };
			}
		}
	}

	template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename ...TArgs>
	Method Method::create(std::string_view name)
	{
		static_assert(Detail::IsSame<decltype(PtrToMemberFunction), TReturn (TObject::*)(TArgs...)>::value,
			"PtrToMemberFunction needs to be a value of type `TReturn (TObject::*)(TArgs...)`");

		return Method{
			.invoke = &Detail::invoke_erased<PtrToMemberFunction, TObject, TReturn, TArgs...>,
			.object_type = get_id<TObject>(),
			.return_type = get_id<TReturn>(),
			.name = std::string{name},
			.argument_types = {get_id<TArgs>()...}
		};
	}
}
