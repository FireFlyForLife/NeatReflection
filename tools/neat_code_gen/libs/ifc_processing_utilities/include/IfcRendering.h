#pragma once

#include "neat/Reflection.h"

#include "reflifc/Module.h"
#include "reflifc/TupleView.h"
#include "reflifc/Type.h"

#include <string>
#include <string_view>


std::string render_full_typename(reflifc::Type type);
std::string render_full_typename(reflifc::FunctionType type);
std::string render_full_typename(const ifc::FundamentalType& type);
std::string render_full_typename(reflifc::Expression expr);
std::string render_full_typename(reflifc::TupleExpressionView tuple);
std::string render_full_typename(reflifc::TemplateId template_id);
std::string render_full_typename(reflifc::Declaration decl);

std::string render_qualifiers(ifc::Qualifiers qualifiers);

std::string render_refered_declaration(reflifc::Declaration decl);

std::string render_namespace(reflifc::Declaration decl);

std::string render_as_neat_access_enum(ifc::Access access, std::string_view value_for_none = "");
std::string render_neat_access_enum(Neat::Access access);

bool is_member_publicly_accessible(reflifc::Field field_declaration, ifc::TypeBasis type, bool reflects_private_members);
bool is_member_publicly_accessible(reflifc::Method method_declaration, ifc::TypeBasis type, bool reflects_private_members);
bool reflects_private_members(reflifc::Declaration type_decl);
bool is_type_exported(reflifc::Type type);
bool is_type_exported(reflifc::MethodType method);
bool is_type_exported(reflifc::Declaration decl);
bool is_type_exported(reflifc::Expression expr);
bool is_type_exported(reflifc::TemplateId template_id);

reflifc::Declaration get_home_scope(const reflifc::Declaration& decl);
