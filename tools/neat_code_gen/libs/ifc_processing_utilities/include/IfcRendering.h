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
	// Aliases
	using TemplateArgumentSets = std::vector<std::vector<reflifc::Expression>>;

	// Data
	ifc::Environment* environment;
	TemplateArgumentSets template_argument_sets;

	// Functions
	std::pair<reflifc::Expression, RecursionContext> get_template_parameter(reflifc::Parameter parameter) const;
};
// Immutably pass the recursion context down to functions
using RecursionContextArg = const RecursionContext&;

std::string render_full_typename(reflifc::Type type, RecursionContextArg ctx);
std::string render_full_typename(reflifc::FunctionType type, RecursionContextArg ctx);
std::string render_full_typename(const ifc::FundamentalType& type);
std::string render_full_typename(reflifc::Expression expr, RecursionContextArg ctx);
std::string render_full_typename(reflifc::Literal literal);
std::string render_full_typename(reflifc::TupleExpressionView tuple, RecursionContextArg ctx);
std::string render_full_typename(reflifc::TemplateId template_id, RecursionContextArg ctx);
std::string render_full_typename(reflifc::Declaration decl, RecursionContextArg ctx);

std::string render_method_pointer(reflifc::MethodType type, std::string_view outer_class_type, RecursionContextArg ctx);

std::string render_full_typename_list(reflifc::TupleTypeView types, RecursionContextArg ctx);

std::string render_qualifiers(ifc::Qualifiers qualifiers);
std::string render_function_type_traits(ifc::FunctionTypeTraits traits);

std::string render_name(reflifc::Name name, RecursionContextArg ctx);
std::string render_name(reflifc::SpecializationName name, RecursionContextArg ctx);

std::string render_refered_declaration(reflifc::Declaration decl, RecursionContextArg ctx);

std::string render_namespace(reflifc::Declaration decl, RecursionContextArg ctx);

std::string_view render_as_neat_access_enum(ifc::Access access, ifc::Access value_for_none);
std::string_view render_as_neat_access_enum(ifc::Access access, std::string_view value_for_none = "");

bool is_member_publicly_accessible(reflifc::Field field_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, RecursionContextArg ctx);
bool is_member_publicly_accessible(reflifc::Method method_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, RecursionContextArg ctx);
bool is_member_publicly_accessible(reflifc::AliasDeclaration alias_declaration, ifc::TypeBasis type, bool reflects_private_members, reflifc::Module root_module, RecursionContextArg ctx);
bool can_reflect_private_members(reflifc::Declaration type_decl, reflifc::Module root_module, RecursionContextArg ctx);
bool is_type_visible_from_module(reflifc::Type type, reflifc::Module root_module, RecursionContextArg ctx);
bool is_type_visible_from_module(reflifc::MethodType method, reflifc::Module root_module, RecursionContextArg ctx);
bool is_type_visible_from_module(reflifc::FunctionType function, reflifc::Module root_module, RecursionContextArg ctx);
bool is_type_visible_from_module(reflifc::Declaration decl, reflifc::Module root_module, RecursionContextArg ctx);
bool is_type_visible_from_module(reflifc::Expression expr, reflifc::Module root_module, RecursionContextArg ctx);
bool is_type_visible_from_module(reflifc::TemplateId template_id, reflifc::Module root_module, RecursionContextArg ctx);
bool is_type_visible_from_module(reflifc::PathExpression path, reflifc::Module root_module, RecursionContextArg ctx);

struct ModuleCache {};
bool is_module_imported_in_module(reflifc::Module to_check, reflifc::Module module_, ifc::Environment& environment);

bool is_reference_type(reflifc::Declaration decl, RecursionContextArg ctx);
bool is_reference_type(reflifc::Type type, RecursionContextArg ctx);
bool is_reference_type(reflifc::Expression expr, RecursionContextArg ctx);

reflifc::Declaration get_home_scope(const reflifc::Declaration& decl, RecursionContextArg ctx);
ifc::BasicSpecifiers get_basic_specifiers(reflifc::Declaration decl, RecursionContextArg ctx);
std::optional<std::string_view> get_declaration_name(reflifc::Declaration decl, RecursionContextArg ctx);

reflifc::Type resolve_type(reflifc::PathExpression path, RecursionContextArg ctx);
reflifc::Type resolve_type(reflifc::Expression scope, std::string_view dependant_name, RecursionContextArg ctx);
reflifc::Type resolve_type(reflifc::Declaration scope, std::string_view dependant_name, RecursionContextArg ctx);

reflifc::Declaration resolve_template_entity(reflifc::TemplateDeclaration template_decl, RecursionContextArg ctx); // Deal with template specializations
bool does_specialization_fit(reflifc::Specialization specialization, RecursionContextArg ctx);
bool does_specialization_fit(reflifc::PartialSpecialization specialization, RecursionContextArg ctx);
