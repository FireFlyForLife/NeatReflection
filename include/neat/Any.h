// A type erased value.
// Similar idea to std::any, rttr::Variant, QVariant but with NeatReflection as the type id
// Implements basic Small Buffer Optimization limited to trivial types
#pragma once
#include "neat/Defines.h"
#include "neat/TemplateTypeId.h"

#include <cassert>
#include <cstdint>
#include <memory>
#include <utility>

// Forward declarations
namespace Neat
{
	struct AnyPtr;
	class Any;
}


namespace Neat
{
	struct AnyPtr
	{
		// Data
		void* value_ptr = nullptr;
		TemplateTypeId type_id = c_empty_type_id;

		// Operators
		auto operator<=>(const AnyPtr& other) const noexcept = default;
	};

	template<typename T>
	concept NotAny = !std::is_same_v<std::decay_t<T>, Any>;

	class Any
	{
	public:
		// Construction & Deconstruction
		Any() = default;
		template<NotAny T>
		Any(T&& value);
		REFL_API Any(const Any& other) noexcept;
		REFL_API Any(Any&& other) noexcept;
		REFL_API Any& operator=(const Any& other) noexcept;
		REFL_API Any& operator=(Any&& other) noexcept;
		template<NotAny T>
		Any& operator=(T&& value);
		REFL_API ~Any();

		// Accessors
		REFL_API bool has_value() const;
		REFL_API TemplateTypeId type_id() const;
		template<typename T>
		T& value();
		template<typename T>
		T* value_ptr();

		// Conversion
		REFL_API AnyPtr to_any_ptr();

	private:
		// Helpers
		REFL_API void* object_pointer();

		// Private data types
		enum class StorageMode : uint8_t { Empty, InlineValue, BoxedValue };

		union alignas(max_align_t) Storage
		{
			static constexpr size_t c_inline_storage_size = 24;

			~Storage() {} // Destruction handled in Any

			std::shared_ptr<void> boxed_value{}; // Active when `storage_mode == StorageMode::BoxedValue`
			std::byte inline_value[c_inline_storage_size]; // Active when `storage_mode == StorageMode::InlineValue`
		};

		// Data
		Storage storage;
		TemplateTypeId template_type_id = c_empty_type_id;
		StorageMode storage_mode = StorageMode::Empty;
	};
}


// Implementation
namespace Neat
{
	template<NotAny T>
	Any::Any(T&& value)
	{
		using CleanT = std::remove_cvref_t<T>;

		template_type_id = get_id<CleanT>();

		if (sizeof(CleanT) <= Storage::c_inline_storage_size && alignof(CleanT) <= alignof(Storage) && std::is_trivial_v<CleanT>) {
			new (storage.inline_value) CleanT{ value };
			storage_mode = StorageMode::InlineValue;
		} else {
			new (&storage.boxed_value) std::shared_ptr<void>{ std::make_shared<CleanT>(value) };
			storage_mode = StorageMode::BoxedValue;
		}
	}

	template<NotAny T>
	Any& Any::operator=(T&& value)
	{
		// Destruct this
		this->~Any();

		// Call into this value constructor
		new (this) Any{ std::forward<T>(value) };

		return *this;
	}

	template<typename T>
	T& Any::value()
	{
		assert(has_value());
		assert(get_id<T>() == template_type_id);
		return *static_cast<T*>(object_pointer());
	}

	template<typename T>
	T* Any::value_ptr()
	{
		if (get_id<T>() != template_type_id) {
			return nullptr;
		}

		return static_cast<T*>(object_pointer());
	}
}
