#pragma once
#include "neat/DllExportMacro.h"
#include "neat/TemplateTypeId.h"
#include "neat/ReflectPrivateMembers.h"

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
#include <typeindex>
#ifdef REFL_CPP_RTTI
#include <typeinfo>
#endif

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
	REFL_API const Type* get_type(std::type_index rtti_type_index);
	template<typename T> 
	const Type* get_type() { return get_type(get_id<T>()); }


	// Types
	// ===========================================================================

	struct AnyPtr
	{
		void* value_ptr = nullptr;
		const Type* type = nullptr;
	};

	enum class Access : uint8_t { Public, Protected, Private };

	struct Type
	{
		// Functions
		template<typename T>
		static Type create(std::string_view name, TemplateTypeId id, 
			std::vector<BaseClass> bases, std::vector<Field> fields, std::vector<Method> methods);

		using Destructor = void (*)(AnyPtr object);

		Destructor destructor;

		// Data
		std::string name;
		TemplateTypeId id;
#ifdef REFL_CPP_RTTI
		std::type_index rtti_type_index;
#endif
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

		using GetValueFunction = std::any (*)(AnyPtr object);
		using SetValueFunction = void (*)(AnyPtr object, std::any value);
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
		bool operator==(const Field& other) const noexcept = default;
		std::strong_ordering operator<=>(const Field& other) const noexcept;
	};

	struct Method
	{
		// Functions
		template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename... TArgs>
		static Method create(std::string_view name, Access access);

		using InvokeFunction = std::any (*)(AnyPtr object, std::span<std::any> arguments);
		
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

	/*template<typename T>
	struct VectorTrait
	{
		static constexpr bool is_specialised = false;
	};

	template<typename TElement>
	struct VectorTrait<std::vector<TElement>>
	{
		static constexpr bool is_specialised = true;

		using ElementType = TElement;
		using ContainerType = std::vector<T>;
		using IteratorTag = std::contiguous_iterator_tag;

		static size_t size(const ContainerType& container) { return container.size(); }
		static void set_element(ContainerType& container, size_t index, ElementType* element) { container[index] = *element; }
		static const ElementType* get_element(const ContainerType& container, size_t index) { return &container[index]; }
		static ElementType* get_element(ContainerType& container, size_t index) { return &container[index]; }

		static void clear(ContainerType& container) { container.clear(); }
		static void add_elements(ContainerType& container, ElementType* element) { container.push_back(*element); }
		static void remove_element(ContainerType& container, size_t index) { container.erase(container.begin() + index); }
	};

	template<typename TElement, size_t Size>
	struct VectorTrait<TElement[Size]>
	{
		static constexpr bool is_specialised = true;

		using ElementType = TElement;
		using ContainerType = TElement[Size];
		using IteratorTag = std::contiguous_iterator_tag;
		
		static size_t size(const ContainerType& container) { (void)container; return Size; }
		static void set_element(ContainerType& container, size_t index, ElementType* element) { container[index] = *element; }
		static const ElementType* get_element(const ContainerType& container, size_t index) { return &container[index]; }
		static ElementType* get_element(ContainerType& container, size_t index) { return &container[index]; }
	};

	template<typename T>
	struct AssociateContainerTrait
	{
		static constexpr bool is_specialised = false;
	};

	template<typename TKey, typename TValue, typename THasher, typename TComparator, typename TAllocator>
	struct AssociateContainerTrait<std::unordered_map<TKey, TValue, THasher, TComparator, TAllocator>>
	{
		static constexpr bool is_specialised = true;

		using ElementType = std::pair<const TKey, TValue>;
		using ContainerType = std::unordered_map<TKey, TValue, THasher, TComparator, TAllocator>;
		using IteratorTag = std::bidirectional_iterator_tag;

		static size_t size(const ContainerType& container)
		{
			return container.size();
		}

		static void clear(ContainerType& container)
		{
			container.clear();
		}

		static void set_element(ContainerType& container, TKey* key, TValue* value)
		{
			container.emplace(*key, *value);
		}

		static const ElementType* get_element(const ContainerType& container, const TKey* key)
		{
			auto it = container.find(*key);
			return it != container.end() ? *it : nullptr;
		}

		static void erase_element(ContainerType& containre, TKey* key)
		{
			container.erase(*key);
		}

		static std::vector<ElementType*> get_all_elements(ContainerType& container)
		{
			std::vector<ElementType*> all_elements;
			all_elements.reserve(container.size());
			for (auto& element : container) {
				all_elements.push_back(&element);
			}
			return all_elements;
		}
	};*/
}


// Implementation
// ===========================================================================

namespace Neat
{
	namespace Detail
	{
		template<typename T>
		void destructor_erased(AnyPtr object)
		{
			assert(object.type == get_type<T>());

			T* object_ = static_cast<T*>(object.value_ptr);
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
#ifdef REFL_CPP_RTTI
			.rtti_type_index = std::type_index{ typeid(T) },
#endif
			.bases = std::move(bases),
			.fields = std::move(fields),
			.methods = std::move(methods)
		};
	}

	namespace Detail
	{
		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		std::any get_field_erased(AnyPtr object)
		{
			assert(object.type == get_type<TObject>());

			TObject* object_ = static_cast<TObject*>(object.value_ptr);
			return object_->*PtrToMember;
		}

		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		void set_field_erased(AnyPtr object, std::any value)
		{
			assert(object.type == get_type<TObject>());
			assert(std::any_cast<TType>(&value) != nullptr);

			TObject* object_ = static_cast<TObject*>(object.value_ptr);
			object_->*PtrToMember = std::any_cast<TType>(value);
		}

		template<typename TObject, typename TType, TType TObject::* PtrToMember>
		AnyPtr get_field_address_erased(AnyPtr object)
		{
			assert(object.type == get_type<TObject>());

			TObject* object_ = static_cast<TObject*>(object.value_ptr);
			return { &(object_->*PtrToMember), get_type<TType>() };
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
		template<auto PtrToMemberFunction, typename TObject, typename TReturn, typename ...TArgs>
		std::any invoke_erased(AnyPtr object, std::span<std::any> arguments)
		{
			// Validate object
			assert(object.type == get_type<TObject>());
			
			// Validate arguments
			assert(arguments.size() == sizeof...(TArgs));
			auto check_all_argument_types = []<size_t... I>(std::span<std::any> arguments, std::index_sequence<I...>)
			{
				return ((std::any_cast<TArgs>(&arguments[I]) != nullptr) && ...);
			};
			assert(check_all_argument_types(arguments, std::index_sequence_for<TArgs...>{}));
			
			// Invoke method
			auto unwrap_arguments_and_invoke = []<size_t... I>(TObject * t_object, std::span<std::any> arguments, std::index_sequence<I...>)
			{
				return (t_object->*PtrToMemberFunction)(std::any_cast<TArgs>(arguments[I])...);
			};
			
			TObject* object_ = static_cast<TObject*>(object.value_ptr);

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
