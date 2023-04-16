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
#include <compare>
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
		// Data
		std::string name;
		TemplateTypeId id;
		std::vector<BaseClass> bases;
		std::vector<Field> fields;
		std::vector<Method> methods;

		// Operators
		bool operator==(const Type& other) const noexcept;
		std::strong_ordering operator<=>(const Type& other) const noexcept;
	};

	struct BaseClass
	{
		// Data
		TemplateTypeId base_id;
		Access access;

		// Operators
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
		bool operator==(const Field& other) const noexcept = default;
		std::strong_ordering operator<=>(const Field& other) const noexcept;
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
		bool operator==(const Method& other) const noexcept = default;
		std::strong_ordering operator<=>(const Method& other) const noexcept;
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
			TObject* object_ = static_cast<TObject*>(object);
			return { object_->*PtrToMember };
		}

		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		void set_value_erased(void* object, std::any value)
		{
			assert(std::any_cast<TType>(&value) != nullptr);

			TObject* object_ = static_cast<TObject*>(object);
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

			auto unwrap_arguments_and_invoke = []<size_t... I>(TObject* t_object, std::span<std::any> arguments, std::index_sequence<I...>)
			{
				return (t_object->*PtrToMemberFunction)(std::any_cast<TArgs>(arguments[I])...);
			};
			
			TObject* object_ = static_cast<TObject*>(object);

			if constexpr (std::is_same_v<TReturn, void>)
			{
				unwrap_arguments_and_invoke(object_, arguments, std::index_sequence_for<TArgs...>{});
				return {};
			}
			else
			{
				return unwrap_arguments_and_invoke(object_, arguments, std::index_sequence_for<TArgs...>{});
			}
		}
	}

	template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename ...TArgs>
	Method Method::create(std::string_view name, Access access)
	{
		static_assert(
			std::is_same_v<decltype(PtrToMemberFunction), TReturn (TObject::*)(TArgs...)>
			|| std::is_same_v<decltype(PtrToMemberFunction), TReturn(TObject::*)(TArgs...) const>,
			"PtrToMemberFunction needs to be a value of type `TReturn (TObject::*)(TArgs...)` or `TReturn (TObject::*)(TArgs...) const`.");

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

namespace std
{
	template<typename T>
	struct hash;
}

namespace Neat::HashUtils
{
	template <typename T, typename... Rest>
	void combine(std::size_t& seed, const T& v, const Rest&... rest)
	{
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(combine(seed, rest), ...);
	}
}

namespace std
{
	template<>
	struct hash<Neat::Type>
	{
		size_t operator()(const Neat::Type& type) const noexcept;
	};

	template<>
	struct hash<Neat::Field>
	{
		size_t operator()(const Neat::Field& field) const;
	};

	template<>
	struct hash<Neat::Method>
	{
		size_t operator()(const Neat::Method& method) const;
	};

	template<>
	struct hash<Neat::BaseClass>
	{
		size_t operator()(const Neat::BaseClass& base_class) const noexcept;
	};
}