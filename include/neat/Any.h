// (WIP) A type erased value.
// Similar idea to std::any, rttr::Variant, QVariant but with NeatReflection as the type id
#pragma once
#include "neat/Reflection.h"

#include <cstdint>

namespace Neat
{
	namespace Detail
	{
		enum class AnyStorageType { Trivial, SmallBuffer, BoxedValue };

		struct alignas(max_align_t) AnyStorage
		{
			static constexpr size_t c_any_storage_size = 24;

			char data[c_any_storage_size];
		};
	}

	class Any
	{
	public:
		// Construction & Deconstruction
		Any() = default;
		template<typename T>
		Any create_by_value(const T& value);
		~Any();

		// Accessors
		bool has_value() const;
		const Type* type() const;
		template<typename T>
		T* try_get();

	private:
		// Helpers
		void* object_pointer();
		Detail::AnyStorageType storage_type() const;

		// Data
		Detail::AnyStorage storage;
		uintptr_t type_ptr = 0;
	};
}

// Implementation
namespace Neat
{
	namespace Detail
	{
		// The alignment of Neat::Type is at least 4, so we can use the 2 first significant bits for the storage type flags.
		constexpr uintptr_t c_storage_type_mask = 0b11;
	}

	template<typename T>
	Any Any::create_by_value(const T& value)
	{
		auto type = get_type<T>();
		assert(type != nullptr);

		Any any{};

		any.type_ptr = reinterpret_cast<uintptr_t>(type);

		if (sizeof(T) <= Detail::AnyStorage::c_any_storage_size)
		{
			reinterpret_cast<T&>(any.storage.data) = value;
			any.type_ptr |= static_cast<uint32_t>(Detail::AnyStorageType::Trivial);
			// TODO: Set SmallBuffer when not trivially copyable
		}
		else
		{
			any.storage.data = new T(value);
			any.type_ptr |= static_cast<uint32_t>(Detail::AnyStorageType::BoxedValue);
		}

		// TODO: Impl move/copy constructor
		return any;
	}

	template<typename T>
	T* Any::try_get()
	{
		if (!has_value())
		{
			return nullptr;
		}
		if (get_type<T>() != type())
		{
			return nullptr;
		}

		return static_cast<T*>(object_pointer());
	}
}