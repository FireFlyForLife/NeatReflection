// Simple and powerful runtime reflection library.
// Illustrates how low level reflection features are implemented for libraries like RTTR or refl-cpp.
#pragma once
#include "neat/Defines.h"
#include "neat/TemplateTypeId.h"
#include "neat/ReflectPrivateMembers.h"
#include "neat/Any.h"

#include <array>
#include <variant>
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
	struct BaseClass;
	struct Field;
	struct Method;
	struct TemplateArgument;
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

		using DefaultConstructor = void (*)(AnyPtr uninitialised_object);
		using Destructor = void (*)(AnyPtr object);
		DefaultConstructor default_constructor;
		Destructor destructor;

		// Data
		std::string name;
		TemplateTypeId id;
		size_t size;
		std::vector<BaseClass> bases;
		std::vector<Field> fields;
		std::vector<Method> methods;
		std::vector<TemplateArgument> template_arguments;

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

		using GetValueFunction = Any (*)(AnyPtr object);
		using SetValueFunction = void (*)(AnyPtr object, Any value);
		using GetAddressFunction = AnyPtr (*)(AnyPtr object);
		GetValueFunction get_value;
		SetValueFunction set_value;
		GetAddressFunction get_address;

		// Data
		TemplateTypeId object_type;
		TemplateTypeId type;
		std::string name;
		std::vector<std::string> attributes; // Unused currently
		Access access;

		// Operators
		bool operator==(const Field& other) const noexcept;
		std::strong_ordering operator<=>(const Field& other) const noexcept;
	};

	struct Method
	{
		// Functions
		template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename... TArgs>
		static Method create(std::string_view name, Access access);

		using InvokeFunction = Any (*)(AnyPtr object, std::span<Any> arguments);
		InvokeFunction invoke;

		// Data
		TemplateTypeId object_type;
		TemplateTypeId return_type;
		std::string name;
		std::vector<TemplateTypeId> argument_types;
		std::vector<std::string> attributes; // Unused currently
		Access access;

		// Operators
		bool operator==(const Method& other) const noexcept;
		std::strong_ordering operator<=>(const Method& other) const noexcept;
	};

	struct TemplateArgument
	{
		std::variant<TemplateTypeId, Any> type_or_value;
	};
}


// Implementation
// ===========================================================================

namespace Neat
{
	namespace Detail
	{
		template<typename T>
		void default_constructor_erased(AnyPtr uninitialised_object)
		{
			assert(uninitialised_object.type_id == get_id<T>());

			new (uninitialised_object.value_ptr) T{};
		}

		template<typename T>
		void destructor_erased(AnyPtr object)
		{
			assert(object.type_id == get_id<T>());

			T* object_ = static_cast<T*>(object.value_ptr);
			object_->~T();
		}
	}

	template<typename T>
	Type Type::create(std::string_view name, TemplateTypeId id,
		std::vector<BaseClass> bases, std::vector<Field> fields, std::vector<Method> methods)
	{
		return Type{
			.default_constructor = (std::is_default_constructible_v<T> ? &Detail::destructor_erased<T> : nullptr),
			.destructor = (std::is_trivially_destructible_v<T> ? nullptr : &Detail::destructor_erased<T>),
			.name = std::string{ name },
			.id = id,
			.size = sizeof(T),
			.bases = std::move(bases),
			.fields = std::move(fields),
			.methods = std::move(methods)
		};
	}

	namespace Detail
	{
		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		Any get_field_erased(AnyPtr object)
		{
			assert(object.type_id == get_id<TObject>());

			TObject* object_ = static_cast<TObject*>(object.value_ptr);
			return object_->*PtrToMember;
		}

		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		void set_field_erased(AnyPtr object, Any value)
		{
			assert(object.type_id == get_id<TObject>());
			assert(value.type_id() == get_id<TType>());

			TObject* object_ = static_cast<TObject*>(object.value_ptr);
			object_->*PtrToMember = value.value<TType>();
		}

		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		AnyPtr get_field_address_erased(AnyPtr object)
		{
			assert(object.type_id == get_id<TObject>());

			TObject* object_ = static_cast<TObject*>(object.value_ptr);
			return { &(object_->*PtrToMember), get_id<TType>() };
		}
	}

	template<typename TObject, typename TType, TType TObject::* PtrToMember>
	Field Field::create(std::string_view name, Access access)
	{
		return Field{
			.get_value = &Detail::get_field_erased<TObject, TType, PtrToMember>,
			.set_value = &Detail::set_field_erased<TObject, TType, PtrToMember>,
			.get_address = &Detail::get_field_address_erased<TObject, TType, PtrToMember>,
			.object_type = get_id<TObject>(),
			.type = get_id<TType>(),
			.name = std::string{ name },
			.access = access
		};
	}

	namespace Detail
	{
		template<size_t TTemplateArgCount>
		inline bool validate_function_arguments(std::array<TemplateTypeId, TTemplateArgCount> template_arguments, std::span<Any> arguments)
		{
			if (template_arguments.size() != arguments.size()) {
				return false;
			}

			for (size_t i = 0; i < template_arguments.size(); ++i) {
				if (template_arguments[i] != arguments[i].type_id()) {
					return false;
				}
			}

			return true;
		}

		template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename ...TArgs>
		Any invoke_erased(AnyPtr object, std::span<Any> arguments)
		{
			// Validate object
			assert(object.type_id == get_id<TObject>());
			
			// Validate arguments
			assert(validate_function_arguments(std::array<TemplateTypeId, sizeof...(TArgs)>{ get_id<std::decay_t<TArgs>>()... }, arguments));
			
			// Invoke method
			auto unwrap_arguments_and_invoke = []<size_t... I>(TObject* t_object, std::span<Any> arguments, std::index_sequence<I...>)
			{
				return (t_object->*PtrToMemberFunction)((std::move(arguments[I].value<std::decay_t<TArgs>>()))...);
			};
			
			TObject* object_ = static_cast<TObject*>(object.value_ptr);

			if constexpr (std::is_void_v<TReturn>)
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

	inline bool Type::operator==(const Type& other) const noexcept
	{
		return id == other.id;
	}

	inline std::strong_ordering Type::operator<=>(const Type& other) const noexcept
	{
		return id <=> other.id;
	}

	inline bool Field::operator==(const Field& other) const noexcept
	{
		return object_type == other.object_type
			&& name == other.name;
	}

	inline std::strong_ordering Field::operator<=>(const Field& other) const noexcept
	{
		std::strong_ordering order;

		order = (object_type <=> other.object_type);
		if (order != 0) { return order; }
		order = (name <=> other.name);

		return order;
	}

	inline bool Method::operator==(const Method& other) const noexcept
	{
		return object_type == other.object_type
			&& return_type == other.return_type
			&& argument_types == other.argument_types
			&& name == other.name;
	}

	inline std::strong_ordering Method::operator<=>(const Method& other) const noexcept
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
	template<typename T>
	struct hash;

	template<>
	struct hash<Neat::Type>
	{
		size_t operator()(const Neat::Type& type) const noexcept
		{
			return type.id;
		}
	};

	template<>
	struct hash<Neat::Field>
	{
		size_t operator()(const Neat::Field& field) const noexcept
		{
			size_t h = 0;
			Neat::HashUtils::combine(h, field.object_type, field.name);
			return h;
		}
	};

	template<>
	struct hash<Neat::Method>
	{
		size_t operator()(const Neat::Method& method) const noexcept
		{
			size_t h = 0;

			for (const auto& argument_type : method.argument_types) {
				Neat::HashUtils::combine(h, argument_type);
			}
			Neat::HashUtils::combine(h, method.object_type, method.return_type, method.name);

			return h;
		}
	};

	template<>
	struct hash<Neat::BaseClass>
	{
		size_t operator()(const Neat::BaseClass& base_class) const noexcept
		{
			return base_class.base_id;
		}
	};
}
