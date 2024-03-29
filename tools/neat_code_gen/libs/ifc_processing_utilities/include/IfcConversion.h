#pragma once

#include "neat/Reflection.h"

#include "ifc/DeclarationFwd.h"
#include "ifc/TypeFwd.h"
#include "ifc/ExpressionFwd.h"
#include "ifc/NameFwd.h"

#include <optional>
#include <string_view>


std::optional<Neat::Access> convert_access_enum(ifc::Access);
ifc::Access convert_access_enum(Neat::Access);

std::string_view decl_sort_to_string(ifc::DeclSort);
std::string_view type_sort_to_string(ifc::TypeSort);
std::string_view expr_sort_to_string(ifc::ExprSort);
std::string_view type_basis_to_string(ifc::TypeBasis);
std::string_view type_precicion_to_string(ifc::TypePrecision);
std::string_view type_sign_to_string(ifc::TypeSign);

std::string_view name_sort_to_string(ifc::NameSort);