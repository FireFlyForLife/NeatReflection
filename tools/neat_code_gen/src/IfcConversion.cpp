#include "IfcConversion.h"

#include "ContextualException.h"

#include "ifc/Declaration.h"


std::optional<Neat::Access> convert_access_enum(ifc::Access ifc_access)
{
	switch (ifc_access)
	{
	case ifc::Access::None: return std::nullopt;
	case ifc::Access::Private: return Neat::Access::Private;
	case ifc::Access::Protected: return Neat::Access::Protected;
	case ifc::Access::Public: return Neat::Access::Public;
	default:
		UNREACHABLE();
	}
}

ifc::Access convert_access_enum(Neat::Access neat_access)
{
	switch (neat_access)
	{
	case Neat::Access::Private: return ifc::Access::Private;
	case Neat::Access::Protected: return ifc::Access::Protected;
	case Neat::Access::Public: return ifc::Access::Public;
	default:
		UNREACHABLE();
	}
}
