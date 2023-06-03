#include "neat/TemplateTypeId.h"

#include <atomic>


namespace Neat
{
	TemplateTypeId generate_new_type_id()
	{
		static std::atomic<TemplateTypeId> id_counter = 1;
		return id_counter.fetch_add(1, std::memory_order_relaxed);
	}
}