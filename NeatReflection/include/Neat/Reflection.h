#pragma once
#include "Neat/DllMacro.h"
#include "Neat/TemplateTypeId.h"
#include "Neat/ReflectPrivateMembers.h"

#include <any>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <type_traits>
#include <span>
#include <cassert>
#include <cstdint>

// Forward Declarations
namespace Neat
{
	struct Type;
	struct Field;
	struct Method;
	struct BaseClass;
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

	enum class Access : uint8_t { Public, Protected, Private };

	struct Type
	{
		std::string name;
		TemplateTypeId id;
		std::vector<BaseClass> bases;
		std::vector<Field> fields;
		std::vector<Method> methods;

		auto operator<=>(const Type& other) const noexcept = default;
	};

	struct BaseClass
	{
		TemplateTypeId base_id;
		Access access;

		auto operator<=>(const BaseClass& other) const noexcept = default;
	};

	struct Field
	{
		// Functions
		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		static Field create(std::string_view name, Access access);

		using GetValueFunction = std::any (*)(void* object);
		using SetValueFunction = void (*)(void* object, std::any value);

		GetValueFunction get_value;
		SetValueFunction set_value;

		// Data
		TemplateTypeId object_type;
		TemplateTypeId type;
		std::string name;
		std::vector<std::string> attributes; // Unused currently
		Access access;

		// Operators
		auto operator<=>(const Field& other) const noexcept = default;
	};

	struct Method
	{
		// Functions
		template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename... TArgs>
		static Method create(std::string_view name, Access access);

		using InvokeFunction = std::any (*)(void* object, std::span<std::any> arguments);
		
		InvokeFunction invoke;

		// Data
		TemplateTypeId object_type;
		TemplateTypeId return_type;
		std::string name;
		std::vector<TemplateTypeId> argument_types;
		std::vector<std::string> attributes; // Unused currently
		Access access;

		// Operators
		auto operator<=>(const Method& other) const noexcept = default;
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
	Field Field::create(std::string_view name, Access access)
	{
		return Field{
			.get_value = &Detail::get_value_erased<TObject, TType, PtrToMember>,
			.set_value = &Detail::set_value_erased<TObject, TType, PtrToMember>,
			.object_type = get_id<TObject>(),
			.type = get_id<TType>(),
			.name = std::string{ name },
			.access = access
		};
	}

	namespace Detail
	{
		template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename ...TArgs>
		std::any invoke_erased(void* object, std::span<std::any> arguments)
		{
			assert(arguments.size() == sizeof...(TArgs));
			// TODO: Validate arg types

			TObject* object_ = reinterpret_cast<TObject*>(object);

			// here we use __cdecl call convention. Maybe add __stdcall call convention support later on.
			auto arg_it = arguments.rbegin();

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
	Method Method::create(std::string_view name, Access access)
	{
		static_assert(std::is_same_v<decltype(PtrToMemberFunction), TReturn (TObject::*)(TArgs...)>,
			"PtrToMemberFunction needs to be a value of type `TReturn (TObject::*)(TArgs...)`");

		return Method{
			.invoke = &Detail::invoke_erased<PtrToMemberFunction, TObject, TReturn, TArgs...>,
			.object_type = get_id<TObject>(),
			.return_type = get_id<TReturn>(),
			.name = std::string{name},
			.argument_types = {get_id<TArgs>()...},
			.access = access
		};
	}
}
