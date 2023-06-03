#pragma once
#include "neat/DllMacro.h"
#include "neat/TemplateTypeId.h"
#include "neat/ReflectPrivateMembers.h"

#include <any>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <type_traits>
#include <functional> // Only for std::reference_wrapper<T>
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

	REFL_API std::span<const Type> get_types();
	REFL_API const Type* get_type(std::string_view type_name);
	REFL_API const Type* get_type(TemplateTypeId type_id);
	template<typename T> 
	const Type* get_type() { return get_type(get_id<T>()); }


	// Types
	// ===========================================================================

	enum class Access : uint8_t { Public, Protected, Private };

	struct Type
	{
		// Functions
		template<typename T>
		static Type create(std::string_view name, TemplateTypeId id, 
			std::vector<BaseClass> bases, std::vector<Field> fields, std::vector<Method> methods);

		using Destructor = void (*)(void*);

		Destructor destructor;

		// Data
		std::string name;
		TemplateTypeId id;
		std::vector<BaseClass> bases;
		std::vector<Field> fields;
		std::vector<Method> methods;

		// Operators
		bool operator==(const Type& other) const noexcept = default;
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

		GetValueFunction get_reference;
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

	template<typename T>
	struct VectorTrait
	{
		static constexpr bool is_vector = false;
	};

	template<typename T>
	struct VectorTrait<std::vector<T>>
	{
		static constexpr bool is_vector = true;

		using value_type = T;
	};

	template<typename T>
	struct VectorTrait<T[]>
	{
		static constexpr bool is_vector = true;

		using value_type = T;
	};
}


// Implementation
// ===========================================================================

namespace Neat
{
	namespace Detail
	{
		template<typename T>
		void destructor_erased(void* object)
		{
			T* object_ = static_cast<T*>(object);
			object_->~T();
		}
	}

	template<typename T>
	Type Type::create(std::string_view name, TemplateTypeId id,
		std::vector<BaseClass> bases, std::vector<Field> fields, std::vector<Method> methods)
	{
		return Type{
			.destructor = (std::is_trivially_destructible_v<T> ? nullptr : &Detail::destructor_erased<T>),
			.name = std::string{ name },
			.id = id,
			.bases = std::move(bases),
			.fields = std::move(fields),
			.methods = std::move(methods)
		};
	}

	namespace Detail
	{
		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		std::any get_field_ref_erased(void* object)
		{
			TObject* object_ = static_cast<TObject*>(object);
			return { std::ref(object_->*PtrToMember) };
		}

		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		void set_field_erased(void* object, std::any value)
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
			.get_reference = &Detail::get_field_ref_erased<TObject, TType, PtrToMember>,
			.set_value = &Detail::set_field_erased<TObject, TType, PtrToMember>,
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

namespace Neat::HashUtils
{
	template <typename T, typename... Rest>
	void combine(std::size_t& seed, const T& v, const Rest&... rest)
	{
		seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		(combine(seed, rest), ...);
	}
}
