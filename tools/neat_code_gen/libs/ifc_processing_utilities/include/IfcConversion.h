#pragma once

#include "neat/Reflection.h"

#include "ifc/DeclarationFwd.h"

#include <optional>


std::optional<Neat::Access> convert_access_enum(ifc::Access);
ifc::Access convert_access_enum(Neat::Access);
