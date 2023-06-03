#include "neat/Any.h"


namespace Neat
{
	Any::~Any()
	{
		if (has_value())
		{

		}
	}

	bool Any::has_value() const
	{
		return type_ptr != nullptr;
	}

	const Type* Any::type() const
	{
		return type_ptr;
	}
}