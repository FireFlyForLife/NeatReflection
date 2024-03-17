#pragma once

#include "neat/Reflection.h"

#include "reflifc/Module.h"
#include "reflifc/TupleView.h"
#include "reflifc/Type.h"
#include "reflifc/Name.h"
#include "reflifc/decl/AliasDeclaration.h"
#include "ifc/Environment.h"
#include "ifc/DeclarationFwd.h"

#include <string>
#include <string_view>
#include <optional>
#include <vector>

// State neccesairy to pass onto the next functions
struct RecursionContext
{
	ifc::Environment* environment;
	std::vector<std::vector<reflifc::Expression>> template_argument_sets;

	reflifc::Expression get_template_parameter(reflifc::Parameter parameter) const;
};

std::string render_full_typename(reflifc::Type type, RecursionContext& ctx);
std::string render_full_typename(reflifc::FunctionType type, RecursionContext& ctx);
std::string render_full_typename(const ifc::FundamentalType& type);
std::string render_full_typename(reflifc::Expression expr, RecursionContext& ctx);
std::string render_full_typename(reflifc::Literal literal);
std::string render_full_typename(reflifc::TupleExpressionView tuple, RecursionContext& ctx);
std::string render_full_typename(reflifc::TemplateId template_id, RecursionContext& ctx);
std::string render_full_typename(reflifc::Declaration decl, RecursionContext& ctx);

std::string render_method_pointer(reflifc::MethodType type, RecursionContext& ctx);

std::string render_full_typename_list(reflifc::TupleTypeView types, RecursionContext& ctx);

std::string render_qualifiers(ifc::Qualifiers qualifiers);
std::string render_function_type_traits(ifc::FunctionTypeTraits traits);

std::string render_name(reflifc::Name name, RecursionContext& ctx);
std::string render_name(reflifc::SpecializationName name, RecursionContext& ctx);

std::string render_refered_declaration(reflifc::Declaration decl, RecursionContext& ctx);

std::string render_namespace(reflifc::Declaration decl, RecursionContext& ctx);

std::string_view render_as_neat_access_enum(ifc::Access access, ifc::Access value_for_none);
std::string_view render_as_neat_access_enum(ifc::Access access, std::string_view value_for_none = "");

bool is_member_publicly_accessible(reflifc::Field field_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, RecursionContext& ctx);
bool is_member_publicly_accessible(reflifc::Method method_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, RecursionContext& ctx);
bool is_member_publicly_accessible(reflifc::AliasDeclaration alias_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, RecursionContext& ctx);
bool can_reflect_private_members(reflifc::Declaration type_decl, reflifc::Module root_module, RecursionContext& ctx);
bool is_type_visible_from_module(reflifc::Type type, reflifc::Module root_module, RecursionContext& ctx);
bool is_type_visible_from_module(reflifc::MethodType method, reflifc::Module root_module, RecursionContext& ctx);
bool is_type_visible_from_module(reflifc::FunctionType function, reflifc::Module root_module, RecursionContext& ctx);
bool is_type_visible_from_module(reflifc::Declaration decl, reflifc::Module root_module, RecursionContext& ctx);
bool is_type_visible_from_module(reflifc::Expression expr, reflifc::Module root_module, RecursionContext& ctx);
bool is_type_visible_from_module(reflifc::TemplateId template_id, reflifc::Module root_module, RecursionContext& ctx);

struct ModuleCache {};
bool is_module_imported_in_module(reflifc::Module to_check, reflifc::Module module_, ifc::Environment& environment);

reflifc::Declaration get_home_scope(const reflifc::Declaration& decl, RecursionContext& ctx);
ifc::BasicSpecifiers get_basic_specifiers(reflifc::Declaration decl, RecursionContext& ctx);
