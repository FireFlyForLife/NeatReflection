#include "Neat/TemplateTypeId.h"

#include <atomic>


namespace Neat
{
	TemplateTypeId generate_new_type_id()
	{
		static std::atomic<TemplateTypeId> id_counter = 0;
		return ++id_counter;
	}
}