#pragma once
#include "neat/Reflection.h"

#include <vector>
#include <unordered_map>
#include <ostream>
#include <string>
#include <string_view>
#include <optional>

#include "reflifc/Expression.h"
#include "reflifc/Module.h"
#include "reflifc/TupleView.h"
#include "reflifc/Type.h"
#include "reflifc/Declaration.h"
#include "reflifc/decl/ClassOrStruct.h"
#include "reflifc/decl/Field.h"
#include "reflifc/decl/Function.h"
#include "reflifc/type/Base.h"
#include "ifc/Type.h"

namespace ifc
{
	class File;
	class Environment;
}

class CodeGenerator
{
public:
	CodeGenerator(const ifc::File& ifc_file, ifc::Environment& environment);

	void write_cpp_file(reflifc::Module module, std::ostream& out);

private:
	struct ReflectableType
	{
		reflifc::Declaration decl;
		reflifc::ClassOrStruct class_struct_decl;

		std::vector<reflifc::Field> fields;
		std::vector<reflifc::Method> methods;
		std::vector<reflifc::BaseType> bases;
	};
	struct ReflectableTypes
	{
		// TODO: Support multiple environments
		std::unordered_map<reflifc::Declaration, ReflectableType> types;
	};
	void scan(reflifc::Scope scope_desc, ReflectableTypes& out_types);
	void scan(reflifc::Declaration decl, ReflectableTypes& out_types);
	void scan(reflifc::TemplateDeclaration template_decl, reflifc::Declaration decl, ReflectableTypes& out_types);
	void scan(reflifc::ScopeDeclaration scope_decl, reflifc::Declaration decl, ReflectableTypes& out_types);
	void scan(reflifc::ClassOrStruct scope_decl, reflifc::Declaration decl, ReflectableTypes& out_types);
	void scan(reflifc::Type type, ReflectableTypes& out_types);
	void scan(reflifc::Expression expression, ReflectableTypes& out_types);

	void render(ReflectableType& type);
	std::string render_field(std::string_view outer_class_type, const reflifc::Field& field) const;
	std::string render_method(std::string_view outer_class_type, const reflifc::Method& method) const;
	std::string render_base_class(std::string_view outer_class_type, bool outer_is_class, const reflifc::BaseType& base_class) const;

private:
	std::string code;
	const ifc::File* ifc_file;
	ifc::Environment* environment;
};
