#pragma once
#include "neat/Reflection.h"


namespace Neat
{
	class Any
	{
	public:
		Any() = default;
		~Any();

		bool has_value() const;
		const Type* type() const;
		template<typename T>
		T* try_get();

	private:
		void* data = nullptr;
		const Type* type_ptr = nullptr;
	};
}

// Implementation
namespace Neat
{
	template<typename T>
	T* Any::try_get()
	{
		if (!has_value())
		{
			return nullptr;
		}
		if (get_type<T>() != type_ptr)
		{
			return nullptr;
		}

		// TODO: SBO
		return reinterpret_cast<T*>(data);
	}
}