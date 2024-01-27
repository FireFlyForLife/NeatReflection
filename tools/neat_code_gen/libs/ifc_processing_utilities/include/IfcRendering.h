#pragma once

#include "neat/Reflection.h"

#include "reflifc/Module.h"
#include "reflifc/TupleView.h"
#include "reflifc/Type.h"
#include "reflifc/Name.h"
#include "ifc/Environment.h"
#include "ifc/DeclarationFwd.h"

#include <string>
#include <string_view>
#include <optional>


std::string render_full_typename(reflifc::Type type, ifc::Environment& environment);
std::string render_full_typename(reflifc::FunctionType type, ifc::Environment& environment);
std::string render_full_typename(const ifc::FundamentalType& type);
std::string render_full_typename(reflifc::Expression expr, ifc::Environment& environment);
std::string render_full_typename(reflifc::Literal literal);
std::string render_full_typename(reflifc::TupleExpressionView tuple, ifc::Environment& environment);
std::string render_full_typename(reflifc::TemplateId template_id, ifc::Environment& environment);
std::string render_full_typename(reflifc::Declaration decl, ifc::Environment& environment);

std::string render_method_pointer(reflifc::MethodType type, ifc::Environment& environment);

std::string render_full_typename_list(reflifc::TupleTypeView types, ifc::Environment& environment);

std::string render_qualifiers(ifc::Qualifiers qualifiers);
std::string render_function_type_traits(ifc::FunctionTypeTraits traits);

std::string render_name(reflifc::Name name, ifc::Environment& environment);
std::string render_name(reflifc::SpecializationName name, ifc::Environment& environment);

std::string render_refered_declaration(reflifc::Declaration decl, ifc::Environment& environment);

std::string render_namespace(reflifc::Declaration decl, ifc::Environment& environment);

std::string render_as_neat_access_enum(ifc::Access access, std::string_view value_for_none = "");
std::string render_neat_access_enum(Neat::Access access);

bool is_member_publicly_accessible(reflifc::Field field_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, ifc::Environment& environment);
bool is_member_publicly_accessible(reflifc::Method method_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, ifc::Environment& environment);
bool can_reflect_private_members(reflifc::Declaration type_decl, reflifc::Module root_module, ifc::Environment& environment);
bool is_type_visible_from_module(reflifc::Type type, reflifc::Module root_module, ifc::Environment& environment);
bool is_type_visible_from_module(reflifc::MethodType method, reflifc::Module root_module, ifc::Environment& environment);
bool is_type_visible_from_module(reflifc::Declaration decl, reflifc::Module root_module, ifc::Environment& environment);
bool is_type_visible_from_module(reflifc::Expression expr, reflifc::Module root_module, ifc::Environment& environment);
bool is_type_visible_from_module(reflifc::TemplateId template_id, reflifc::Module root_module, ifc::Environment& environment);

struct ModuleCache {};
bool is_module_imported_in_module(reflifc::Module to_check, reflifc::Module module_, ifc::Environment& environment);

reflifc::Declaration get_home_scope(const reflifc::Declaration& decl, ifc::Environment& environment);
ifc::BasicSpecifiers get_basic_specifiers(reflifc::Declaration decl, ifc::Environment& environment);
