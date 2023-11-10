#include "neat/Any.h"

#include <type_traits>


namespace Neat
{
	static_assert(alignof(Type) >= 4, "We store the trivial / sbo flags in the first 2 bits of the pointer.");

	Any::~Any()
	{
		if (has_value() && type()->destructor)
		{
			type()->destructor({ object_pointer(), type() });
		}

		if (storage_type() == Detail::AnyStorageType::BoxedValue)
		{
			const auto obj_memory = object_pointer();
			delete[] obj_memory;
		}
	}

	bool Any::has_value() const
	{
		return type_ptr != 0;
	}

	const Type* Any::type() const
	{
		return reinterpret_cast<const Type*>(type_ptr & ~Detail::c_storage_type_mask);
	}

	void* Any::object_pointer()
	{
		if (storage_type() == Detail::AnyStorageType::BoxedValue)
		{
			return *reinterpret_cast<void**>(storage.data);
		}
		else
		{
			return storage.data;
		}
	}

	Detail::AnyStorageType Any::storage_type() const
	{
		return static_cast<Detail::AnyStorageType>(type_ptr & Detail::c_storage_type_mask);
	}
}