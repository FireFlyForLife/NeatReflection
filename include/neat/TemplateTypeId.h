#pragma once
#include "neat/DllMacro.h"

#include <cstdint>


namespace Neat
{
	using TemplateTypeId = uint32_t;

	REFL_API TemplateTypeId generate_new_type_id();

	template<typename T>
	TemplateTypeId get_id()
	{
		static TemplateTypeId id = generate_new_type_id();
		return id;
	}
}