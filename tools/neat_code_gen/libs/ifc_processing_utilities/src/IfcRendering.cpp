#include "IfcRendering.h"

#include "ContextualException.h"
#include "IfcConversion.h"

#include "ifc/Type.h"
#include "ifc/Expression.h"
#include "reflifc/Expression.h"
#include "reflifc/TemplateId.h"
#include "reflifc/decl/AliasDeclaration.h"
#include "reflifc/decl/ClassOrStruct.h"
#include "reflifc/decl/Concept.h"
#include "reflifc/decl/DeclarationReference.h"
#include "reflifc/decl/Enumeration.h"
#include "reflifc/decl/Function.h"
#include "reflifc/decl/Intrinsic.h"
#include "reflifc/decl/Namespace.h"
#include "reflifc/decl/Parameter.h"
#include "reflifc/decl/TemplateDeclaration.h"
#include "reflifc/decl/UsingDeclaration.h"
#include "reflifc/type/Function.h"
#include "reflifc/type/Pointer.h"
#include "reflifc/type/Placeholder.h"
#include "reflifc/type/Qualified.h"
#include "reflifc/type/Reference.h"

#include "magic_enum.hpp"

#include <algorithm>
#include <format>


std::string render_full_typename(reflifc::Type type, ifc::Environment& environment)
{
	switch (type.sort())
	{
	case ifc::TypeSort::Fundamental:
		return render_full_typename(type.as_fundamental());
	case ifc::TypeSort::Designated:
	{
		const auto designated_declaration = type.designation();
		return render_namespace(designated_declaration, environment) + render_refered_declaration(designated_declaration, environment);
	}
	case ifc::TypeSort::Pointer:
		return render_full_typename(type.as_pointer().pointee, environment) + "*";
	case ifc::TypeSort::LvalueReference:
		return render_full_typename(type.as_lvalue_reference().referee, environment) + "&";
	case ifc::TypeSort::RvalueReference:
		return render_full_typename(type.as_rvalue_reference().referee, environment) + "&&";
	case ifc::TypeSort::Qualified:
		return render_full_typename(type.as_qualified().unqualified(), environment) +
			render_qualifiers(type.as_qualified().qualifiers());
	case ifc::TypeSort::Placeholder:
		if (auto elaborated_type = type.as_placeholder().elaboration())
		{
			return render_full_typename(elaborated_type, environment);
		}
		throw ContextualException("IFC Doesn't contain deduced type for placeholder to be rendered with render_full_typename()");
	case ifc::TypeSort::Function: // U (*)(Args...);
		return render_full_typename(type.as_function(), environment);
	case ifc::TypeSort::Syntactic:
		return render_full_typename(type.as_syntactic(), environment);

		// Currently unsupported
	case ifc::TypeSort::Expansion: // variadic pack expansion (...)
	case ifc::TypeSort::PointerToMember: // U (T::*);
	case ifc::TypeSort::Method: // U (T::*)(Args...);
		// case ifc::TypeSort::Array: // T t[N]; Not implemented yet in ifc-reader
		// case ifc::TypeSort::Typename: // typename T::dependant_type; Not implementet yet in ifc-reader
	case ifc::TypeSort::Decltype: // Seems to complicated to support
	case ifc::TypeSort::Forall: // Template declaration. Not used yet in MSVC
	case ifc::TypeSort::Unaligned: // __unaligned T; Partition not implemented yet in ifc-reader

		// Not planned to be supported
	case ifc::TypeSort::VendorExtension:
	case ifc::TypeSort::Tor: // Compiler generated constructor
	case ifc::TypeSort::SyntaxTree: // General parse tree representation. Seems to complicated to support

	default:
		//assert(false && "Not supported yet");
		return std::format("<UNSUPPORTED_TYPE {}>", magic_enum::enum_name(type.sort()));
	}
}

std::string render_full_typename(reflifc::FunctionType function_type, ifc::Environment& environment) // U (*)(Args...);
{
	auto return_type = render_full_typename(function_type.return_type(), environment);
	std::string parameter_types;
	auto params = function_type.parameters();
	parameter_types.reserve(params.size() * 8);
	bool first = true;
	for (auto param : params)
	{
		if (first)
			first = false;
		else
			parameter_types += ", ";
		parameter_types += render_full_typename(param, environment);
	}
	return return_type + " (" + parameter_types + ")";
}

std::string render_full_typename(const ifc::FundamentalType& type)
{
	std::string rendered;
	rendered.reserve(16);

	if (type.sign == ifc::TypeSign::Unsigned)
	{
		rendered += "unsigned ";
	}

	switch (type.precision)
	{
	case ifc::TypePrecision::Default:
		break;
	case ifc::TypePrecision::Short:
		assert(type.basis == ifc::TypeBasis::Int);
		return "short";
	case ifc::TypePrecision::Long:
		rendered += "long ";
		break;
	case ifc::TypePrecision::Bit64:
		assert(type.basis == ifc::TypeBasis::Int);
		return "long long";
	case ifc::TypePrecision::Bit8:
		if (type.basis == ifc::TypeBasis::Char)
		{
			return "char8_t";
		}
	case ifc::TypePrecision::Bit16:
		if (type.basis == ifc::TypeBasis::Char)
		{
			return "char16_t";
		}
	case ifc::TypePrecision::Bit32:
		if (type.basis == ifc::TypeBasis::Char)
		{
			return "char32_t";
		}
	case ifc::TypePrecision::Bit128:
		rendered += std::format("<UNEXPECTED_BITNESS {}>",
			magic_enum::enum_name(type.precision));
		break;
	}

	switch (type.basis)
	{
	case ifc::TypeBasis::Void:
		rendered += "void";
		break;
	case ifc::TypeBasis::Bool:
		rendered += "bool";
		break;
	case ifc::TypeBasis::Char:
		rendered += "char";
		break;
	case ifc::TypeBasis::Wchar_t:
		rendered += "wchar_t";
		break;
	case ifc::TypeBasis::Int:
		rendered += "int";
		break;
	case ifc::TypeBasis::Float:
		rendered += "float";
		break;
	case ifc::TypeBasis::Double:
		rendered += "double";
		break;
	default:
		rendered += std::format("<UNEXPECTED_FUNCAMENTAL_TYPE {}>",
			magic_enum::enum_name(type.basis));
		break;
	}

	return rendered;
}


std::string render_full_typename(reflifc::TemplateId template_id, ifc::Environment& environment)
{
	return std::format("{}<{}>",
		render_full_typename(template_id.primary(), environment),
		render_full_typename(template_id.arguments(), environment));
}

std::string render_full_typename(reflifc::Expression expr, ifc::Environment& environment)
{
	switch (expr.sort())
	{
	case ifc::ExprSort::NamedDecl:
		return render_full_typename(expr.referenced_decl(), environment);
	case ifc::ExprSort::Type:
		return render_full_typename(expr.as_type(), environment);
	case ifc::ExprSort::TemplateId:
		return render_full_typename(expr.as_template_id(), environment);
	case ifc::ExprSort::Empty:
		return "";

		// This function is only for rendering a typename, most expression sorts are not used for that so will be ignored:
	default:
		throw ContextualException(std::format("Unexpected expression while rendering typename. ExprSort is: {}",
			magic_enum::enum_name(expr.sort())));
	}
}

std::string render_full_typename(reflifc::TupleExpressionView tuple, ifc::Environment& environment)
{
	std::string rendered;
	rendered.reserve(tuple.size() * 8); // Preallocate a reasonable amount

	bool first = true;
	for (auto expression : tuple)
	{
		if (!first)
		{
			rendered += ", ";
		}

		rendered += render_full_typename(expression, environment);

		first = false;
	}
	return rendered;
}

std::string render_full_typename(reflifc::Declaration decl, ifc::Environment& environment)
{
	return render_namespace(decl, environment) + render_refered_declaration(decl, environment);
}

std::string render_qualifiers(ifc::Qualifiers qualifiers)
{
	using namespace std::string_view_literals;

	constexpr auto const_rendered = "const "sv;
	constexpr auto volatile_rendered = "volatile "sv;
	constexpr auto longest_size = const_rendered.size() + volatile_rendered.size();

	std::string rendered;

	if (ifc::has_qualifier(qualifiers, ifc::Qualifiers::Const))
	{
		rendered.reserve(longest_size);
		rendered += const_rendered;
	}
	if (ifc::has_qualifier(qualifiers, ifc::Qualifiers::Volatile))
	{
		rendered.reserve(longest_size);
		rendered += volatile_rendered;
	}
	if (ifc::has_qualifier(qualifiers, ifc::Qualifiers::Restrict))
	{
		// Ignored
	}

	return rendered;
}



std::string render_refered_declaration(reflifc::Declaration decl, ifc::Environment& environment)
{
	switch (const auto kind = decl.sort())
	{
	case ifc::DeclSort::Parameter:
	{
		reflifc::Parameter param = decl.as_parameter();
		return param.name();
	}
	case ifc::DeclSort::Scope:
	{
		reflifc::ScopeDeclaration scope = decl.as_scope();
		return scope.name().as_identifier();
	}
	case ifc::DeclSort::Template:
	{
		reflifc::TemplateDeclaration template_declaration = decl.as_template();
		return template_declaration.name().as_identifier();
	}
	case ifc::DeclSort::Function:
	{
		reflifc::Function function = decl.as_function();
		return function.name().as_identifier();
	}
	case ifc::DeclSort::Reference:
	{
		auto referenced_declaration = decl.as_reference().referenced_declaration(environment);
		return render_refered_declaration(referenced_declaration, environment);
	}
	case ifc::DeclSort::Enumeration:
	{
		auto enumeration = decl.as_enumeration();
		return enumeration.name();
	}
	default:
		throw ContextualException(std::format("Cannot render a refered declaration with unsupported DeclSort: {}.", 
			magic_enum::enum_name(kind)));
	}
}

std::string render_namespace(reflifc::Declaration decl, ifc::Environment& environment)
{
	reflifc::Declaration home_scope = get_home_scope(decl, environment);

	if (!home_scope) {
		return "";
	}

	// Recursive call
	auto rendered_namespace = render_namespace(home_scope, environment) + render_refered_declaration(home_scope, environment);

	if (rendered_namespace.empty()) {
		return "";
	}

	return rendered_namespace + "::";
}

std::string render_as_neat_access_enum(ifc::Access access, std::string_view value_for_none)
{
	switch (access)
	{
	case ifc::Access::None: return std::string{ value_for_none };
	case ifc::Access::Private: return "Neat::Access::Private";
	case ifc::Access::Protected: return "Neat::Access::Protected";
	case ifc::Access::Public: return "Neat::Access::Public";
	}

	throw ContextualException("Invalid access value.",
		std::format("Expected 0 to 3 (inclusive). While {0} was given.", static_cast<uint8_t>(access)));
}

std::string render_neat_access_enum(Neat::Access access)
{
	switch (access)
	{
	case Neat::Access::Private: return "Neat::Access::Private";
	case Neat::Access::Protected: return "Neat::Access::Protected";
	case Neat::Access::Public: return "Neat::Access::Public";
	default:
		throw ContextualException("Invalid access value.",
			std::format("Expected 0 to 3 (inclusive). While {0} was given.", static_cast<uint8_t>(access)));
	}
}

bool is_member_publicly_accessible(reflifc::Field field_declaration, ifc::TypeBasis type, bool reflects_private_members, ifc::Environment& environment)
{
	if (!is_type_exported(field_declaration.type(), environment))
	{
		return false;
	}

	Neat::Access default_access;
	switch (type)
	{
	case ifc::TypeBasis::Class:
		default_access = Neat::Access::Private;
		break;
	case ifc::TypeBasis::Struct:
		default_access = Neat::Access::Public;
		break;
	default:
		default_access = Neat::Access::Public;
		//throw ContextualException(std::format("Expected a member to be part of a Class or a Struct, but it's part of a {} instead.",
		//	magic_enum::enum_name(type));
	}

	const Neat::Access member_access = convert_access_enum(field_declaration.access()).value_or(default_access);

	return (member_access == Neat::Access::Public || reflects_private_members);
}

bool is_member_publicly_accessible(reflifc::Method method_declaration, ifc::TypeBasis type, bool reflects_private_members, ifc::Environment& environment)
{
	if (!is_type_exported(method_declaration.type(), environment))
	{
		return false;
	}

	Neat::Access default_access;
	switch (type)
	{
	case ifc::TypeBasis::Class:
		default_access = Neat::Access::Private;
		break;
	case ifc::TypeBasis::Struct:
		default_access = Neat::Access::Public;
		break;
	default:
		default_access = Neat::Access::Public;
		//throw ContextualException(std::format("Expected a member to be part of a Class or a Struct, but it's part of a {} instead.",
		//	magic_enum::enum_name(type));
	}

	const Neat::Access member_access = convert_access_enum(method_declaration.access()).value_or(default_access);

	return (member_access == Neat::Access::Public || reflects_private_members);
}

bool reflects_private_members(reflifc::Declaration type_decl, ifc::Environment& environment)
{
	for (auto expr_index : type_decl.friends())
	{
		switch (expr_index.sort())
		{
		case ifc::ExprSort::NamedDecl:
		{
			auto named_decl = expr_index.referenced_decl();
			if (!named_decl.is_function())
				return false;

			// TODO OPT: Don't require allocations for these comparisons.
			auto friend_name = render_namespace(named_decl, environment) + render_refered_declaration(named_decl, environment);
			auto rendered_type = render_full_typename(named_decl.as_function().type(), environment);
			return friend_name == "Neat::reflect_private_members"
				&& rendered_type == "void ()";
		}
		case ifc::ExprSort::TemplateId:
			// Not supported yet

		default:
			throw ContextualException(std::format("Unexpected expr sort in friend declaration! {}\n", 
				magic_enum::enum_name(expr_index.sort())));
		}
	}

	return false;
}

bool is_type_exported(reflifc::Type type, ifc::Environment& environment)
{
	switch (type.sort())
	{
	case ifc::TypeSort::Fundamental:
		return true;
	case ifc::TypeSort::Designated:
		return is_type_exported(type.designation(), environment);
	case ifc::TypeSort::Syntactic:
		return is_type_exported(type.as_syntactic(), environment);
	case ifc::TypeSort::Method:
		return is_type_exported(type.as_method(), environment);
	default:
		throw ContextualException(std::format("Unexpected type while checking if the type was exported. type sort: {}",
			magic_enum::enum_name(type.sort())));
	}
}

bool is_type_exported(reflifc::MethodType method, ifc::Environment& environment)
{
	return is_type_exported(method.return_type(), environment) &&
		std::ranges::all_of(method.parameters(), [&environment](reflifc::Type type) { return is_type_exported(type, environment); });
}

bool is_type_exported(reflifc::Declaration decl, ifc::Environment& environment)
{
	ifc::BasicSpecifiers specifiers{};

	switch (decl.sort())
	{
	case ifc::DeclSort::Scope:
		specifiers = decl.as_scope().specifiers();
		break;
	case ifc::DeclSort::Enumeration:
		specifiers = decl.as_enumeration().specifiers();
		break;
	case ifc::DeclSort::Template:
		specifiers = decl.as_template().specifiers();
		break;
	case ifc::DeclSort::Reference:
		return is_type_exported(decl.as_reference().referenced_declaration(environment), environment);
	default:
		throw ContextualException(std::format("Unexpected declaration while checking if the type decl was exported. type decl sort: {}",
			magic_enum::enum_name(decl.sort())));
	}

	using namespace magic_enum::bitwise_operators;
	return (magic_enum::enum_underlying(specifiers & ifc::BasicSpecifiers::NonExported) == 0);
}

bool is_type_exported(reflifc::Expression expr, ifc::Environment& environment)
{
	switch (expr.sort())
	{
	case ifc::ExprSort::NamedDecl:
		return is_type_exported(expr.referenced_decl(), environment);
	case ifc::ExprSort::Type:
		return is_type_exported(expr.as_type(), environment);
	case ifc::ExprSort::TemplateId:
		return is_type_exported(expr.as_template_id(), environment);
	case ifc::ExprSort::Empty:
		return false;

		// Not supported currently, will need more investigation.
	case ifc::ExprSort::Lambda:
	case ifc::ExprSort::UnresolvedId:
	case ifc::ExprSort::SimpleIdentifier:

	case ifc::ExprSort::UnqualifiedId:
	case ifc::ExprSort::QualifiedName:
	case ifc::ExprSort::Path:

		// The rest is not supported, these expressions are for internals similar to a syntax tree.
	default:
		throw ContextualException(std::format("Unexpected expression while checking if the type can be exported. ExprSort is: {}",
			magic_enum::enum_name(expr.sort())));
	}
}

bool is_type_exported(reflifc::TemplateId template_id, ifc::Environment& environment)
{
	return is_type_exported(template_id.primary(), environment) &&
		std::ranges::all_of(template_id.arguments(), [&environment](reflifc::Expression arg)
			{
				return is_type_exported(arg, environment);
			});
}

reflifc::Declaration get_home_scope(const reflifc::Declaration& decl, ifc::Environment& environment)
{
	switch (const auto sort = decl.sort())
	{
	case ifc::DeclSort::Variable:
		return decl.as_variable().home_scope();
	case ifc::DeclSort::Field:
		return decl.as_field().home_scope();
	case ifc::DeclSort::Scope:
		return decl.as_scope().home_scope();
	case ifc::DeclSort::Intrinsic:
		return decl.as_intrinsic().home_scope();
	case ifc::DeclSort::Enumeration:
		return decl.as_enumeration().home_scope();
	case ifc::DeclSort::Alias:
		return decl.as_alias().home_scope();
	case ifc::DeclSort::Template:
		return decl.as_template().home_scope();
	case ifc::DeclSort::Concept:
		return decl.as_concept().home_scope();
	case ifc::DeclSort::Function:
		return decl.as_function().home_scope();
	case ifc::DeclSort::Method:
		return decl.as_method().home_scope();
	case ifc::DeclSort::Constructor:
		return decl.as_constructor().home_scope();
	case ifc::DeclSort::Destructor:
		return decl.as_destructor().home_scope();
	case ifc::DeclSort::UsingDeclaration:
		return decl.as_using().home_scope();
	case ifc::DeclSort::Reference: // Reference to an external module's declaration
		return get_home_scope(decl.as_reference().referenced_declaration(environment), environment);

		// Currently unsupported:
	case ifc::DeclSort::Bitfield:
	case ifc::DeclSort::PartialSpecialization:
	case ifc::DeclSort::InheritedConstructor:

		// Unable to get a home_scope for these:
	case ifc::DeclSort::Parameter:
	case ifc::DeclSort::VendorExtension:
	case ifc::DeclSort::Enumerator:
	case ifc::DeclSort::Temploid:
	//case ifc::DeclSort::ExplicitSpecialization: // Not implemented in ifc-reader
	//case ifc::DeclSort::ExplicitInstantiation: // Not implemented in ifc-reader
	case ifc::DeclSort::UsingDirective:
	case ifc::DeclSort::Friend:
	case ifc::DeclSort::Expansion:
	case ifc::DeclSort::DeductionGuide:
	case ifc::DeclSort::Barren:
	case ifc::DeclSort::Tuple:
	case ifc::DeclSort::SyntaxTree:
	case ifc::DeclSort::Property:
	case ifc::DeclSort::OutputSegment:
	default:
		throw ContextualException(std::format("Cannot get the home_scope for a decl sort of: {}", 
			magic_enum::enum_name(sort)));
	}
}
