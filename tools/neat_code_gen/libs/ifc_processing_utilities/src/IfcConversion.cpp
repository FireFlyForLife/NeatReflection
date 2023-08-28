#include "IfcConversion.h"

#include "ContextualException.h"

#include "ifc/Declaration.h"
#include "ifc/Expression.h"
#include "ifc/Type.h"

#include <magic_enum.hpp>


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

std::string_view decl_sort_to_string(ifc::DeclSort sort)
{
	// TODO: Get rid of magic_enum, write the switch statement out manually

	return magic_enum::enum_name(sort);
}

std::string_view type_sort_to_string(ifc::TypeSort sort)
{
	return magic_enum::enum_name(sort);
}

std::string_view expr_sort_to_string(ifc::ExprSort sort)
{
	return magic_enum::enum_name(sort);
}

std::string_view type_basis_to_string(ifc::TypeBasis type_basis)
{
	return magic_enum::enum_name(type_basis);
}
