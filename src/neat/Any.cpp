#include "neat/Any.h"

#include <type_traits>
#include <cassert>


namespace Neat
{
	Any::Any(const Any& other) noexcept
	{
		// Assign other storage
		if (other.storage_mode == StorageMode::InlineValue) {
			memcpy(storage.inline_value, other.storage.inline_value, Storage::c_inline_storage_size);
		} else if (other.storage_mode == StorageMode::BoxedValue) {
			new (&storage.boxed_value) std::shared_ptr<void>{ other.storage.boxed_value };
		}

		// Assign other storage
		template_type_id = other.template_type_id;
		storage_mode = other.storage_mode;
	}

	Any::Any(Any&& other) noexcept
	{
		// Assign other storage
		if (other.storage_mode == StorageMode::InlineValue) {
			memcpy(storage.inline_value, other.storage.inline_value, Storage::c_inline_storage_size);
		} else if (other.storage_mode == StorageMode::BoxedValue) {
			new (&storage.boxed_value) std::shared_ptr<void>{ std::move(other.storage.boxed_value) };
		}

		// Assign other storage
		template_type_id = other.template_type_id;
		storage_mode = other.storage_mode;

		// Clear other
		other.template_type_id = c_empty_type_id;
		other.storage_mode = StorageMode::Empty;
	}

	Any& Any::operator=(const Any& other) noexcept
	{
		// Self assignment check
		if (&other == this) {
			return *this;
		}

		// Destroy this
		this->~Any();

		// Assign other
		new (this) Any{ other };

		return *this;
	}

	Any& Any::operator=(Any&& other) noexcept
	{
		// Self assignment check
		if (&other == this) {
			return *this;
		}

		// Destroy this
		this->~Any();

		// Move assign other
		new (this) Any{ std::move(other) };

		return *this;
	}

	Any::~Any()
	{
		// Destroy storage
		if (storage_mode == StorageMode::BoxedValue) {
			storage.boxed_value.~shared_ptr();
		} else if (storage_mode == StorageMode::InlineValue) {
			// SBO is limited to trivial type currently, so no destruction needs to happen
		}
	}

	bool Any::has_value() const
	{
		return storage_mode != StorageMode::Empty;
	}

	TemplateTypeId Any::type_id() const
	{
		return template_type_id;
	}

	AnyPtr Any::to_any_ptr()
	{
		if (!has_value()) {
			return AnyPtr{};
		}

		return AnyPtr{ object_pointer(), template_type_id };
	}

	void* Any::object_pointer()
	{
		switch (storage_mode) {
		case StorageMode::InlineValue: return storage.inline_value;
		case StorageMode::BoxedValue: return storage.boxed_value.get();
		case StorageMode::Empty: return nullptr;
		}

		assert(false && "Unexpected AnyStorageType flag.");
		return nullptr;
	}
}
