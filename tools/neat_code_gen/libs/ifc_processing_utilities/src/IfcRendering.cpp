#include "IfcRendering.h"

#include "ContextualException.h"
#include "IfcConversion.h"
#include "IfcVisitor.h"

#include "ifc/Type.h"
#include "ifc/Expression.h"
#include "ifc/Declaration.h"
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
#include "reflifc/decl/Specialization.h"
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
#include <concepts>

using namespace std::string_literals;
using namespace std::string_view_literals;


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
		return render_full_typename(type.as_qualified().unqualified(), environment)
			+ " "
			+ render_qualifiers(type.as_qualified().qualifiers());
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
	case ifc::ExprSort::Literal:
		return render_full_typename(expr.as_literal());
	case ifc::ExprSort::Empty:
		return "";

		// This function is only for rendering a typename, most expression sorts are not used for that so will be ignored:
	default:
		throw ContextualException(std::format("Unexpected expression while rendering typename. ExprSort is: {}",
			magic_enum::enum_name(expr.sort())));
	}
}

std::string render_full_typename(reflifc::Literal literal)
{
	if (literal.is_null())
	{
		return "";
	}

	switch (literal.sort())
	{
	case ifc::LiteralSort::Immediate: // uint32
		return std::to_string(literal.int_value());
	case ifc::LiteralSort::Integer: // uint64
		//return std::to_string(literal.int64_value());
	case ifc::LiteralSort::FloatingPoint: // double (8 byte)
		//return std::to_string(literal.float_value());
	default:
		UNREACHABLE(); // Unknown LiteralSort enum value
		return "";
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
	if (!is_type_visible_from_module(field_declaration.type(), reflifc::Module{nullptr}, environment))
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
		throw ContextualException(std::format("Expected a member variable (Field) to be part of a Class or a Struct, but it's part of a {} instead.",
			type_basis_to_string(type)));
	}

	const Neat::Access member_access = convert_access_enum(field_declaration.access()).value_or(default_access);

	return (member_access == Neat::Access::Public || reflects_private_members);
}

bool is_member_publicly_accessible(reflifc::Method method_declaration, ifc::TypeBasis type, bool reflects_private_members, ifc::Environment& environment)
{
	if (!is_type_visible_from_module(method_declaration.type(), reflifc::Module{nullptr}, environment))
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
		throw ContextualException(std::format("Expected a member function (Method) to be part of a Class or a Struct, but it's part of a {} instead.",
			type_basis_to_string(type)));
	}

	const Neat::Access member_access = convert_access_enum(method_declaration.access()).value_or(default_access);

	return (member_access == Neat::Access::Public || reflects_private_members);
}

bool can_reflect_private_members(reflifc::Declaration type_decl, ifc::Environment& environment)
{
	for (auto expression : type_decl.friends())
	{
		if(expression.sort() == ifc::ExprSort::NamedDecl)
		{
			// If the friend declaration is a named function.
			auto named_decl = expression.referenced_decl();
			if (!named_decl.is_function()) {
				return false;
			}
			auto function_decl = named_decl.as_function();
			
			// Now check if the friend is equal to: `void Neat::reflect_types_and_members()`.

			// Check if the return type is `void`.
			auto return_type = function_decl.type().return_type();
			if (!return_type.is_fundamental()) {
				return false;
			}
			auto return_type_basis = return_type.as_fundamental().basis;

			if (return_type_basis != ifc::TypeBasis::Void) {
				return false;
			}

			// Check the parameters ().
			auto parameter_types = function_decl.type().parameters();
			if (!parameter_types.empty()) {
				return false;
			}

			// Check if the name is "reflect_types_and_members".
			auto name = function_decl.name();
			if (!name.is_identifier()) {
				return false;
			}
			auto name_str = name.as_identifier();
			if (name_str != "reflect_types_and_members"sv) {
				return false;
			}

			// Check if the namespace is "Neat::".
			auto friend_name = render_namespace(named_decl, environment) + render_refered_declaration(named_decl, environment);
			auto rendered_type = render_full_typename(named_decl.as_function().type(), environment);

			auto home_scope = get_home_scope(named_decl, environment);
			if (!home_scope || !home_scope.is_scope() || !home_scope.as_scope().is_namespace()) {
				return false;
			}
			auto namespace_ = home_scope.as_scope().as_namespace();
			auto namespace_str = namespace_.name().as_identifier();
			
			if (namespace_str != "Neat"sv) {
				return false;
			}

			auto home_scope_of_home_scope = get_home_scope(home_scope, environment);
			if (home_scope_of_home_scope) { // No more parent namespaces are expected.
				return false;
			}

			// The declaration has a friend expression to the `void Neat::reflect_types_and_members()` functions.
			return true;
		}
	}

	// No friend expression to `Neat::reflect_types_and_members()`.
	return false;
}

bool is_type_visible_from_module(reflifc::Type type, reflifc::Module module_, ifc::Environment& environment)
{
	switch (type.sort())
	{
	case ifc::TypeSort::Fundamental:
		return true;
	case ifc::TypeSort::Designated:
		return is_type_visible_from_module(type.designation(), module_, environment);
	case ifc::TypeSort::Syntactic:
		return is_type_visible_from_module(type.as_syntactic(), module_, environment);
	case ifc::TypeSort::Method:
		return is_type_visible_from_module(type.as_method(), module_, environment);
	case ifc::TypeSort::Qualified:
		return is_type_visible_from_module(type.as_qualified().unqualified(), module_, environment);
	default:
		throw ContextualException(std::format("Unexpected type while checking if the type was exported. type sort: {}",
			magic_enum::enum_name(type.sort())));
	}
}

bool is_type_visible_from_module(reflifc::MethodType method, reflifc::Module module_, ifc::Environment& environment)
{
	return is_type_visible_from_module(method.return_type(), module_, environment) &&
		std::ranges::all_of(method.parameters(), [module_, &environment](reflifc::Type type) { return is_type_visible_from_module(type, module_, environment); });
}

bool is_type_visible_from_module(reflifc::Declaration decl, reflifc::Module module_, ifc::Environment& environment)
{
	ContextArea area_{ "While checking if Declaration '{}' is visible from module 'TODO Fill module name in'."sv, 
		decl_sort_to_string(decl.sort()), 
		// TODO: module_.unit().name() 
	};
	using namespace magic_enum::bitwise_operators;

	// TODO: Implement this.
	const bool is_decl_in_module_import_chain = true;
	const bool is_decl_in_direct_module = true;

	auto specifiers = get_basic_specifiers(decl, environment);

	if (is_decl_in_direct_module) {
		return (magic_enum::enum_underlying(specifiers & ifc::BasicSpecifiers::IsMemberOfGlobalModule) == 0);
	} else if(is_decl_in_module_import_chain) {
		return (magic_enum::enum_underlying(specifiers & (ifc::BasicSpecifiers::IsMemberOfGlobalModule | ifc::BasicSpecifiers::NonExported)) == 0);
	} else {
		return false;
	}
}

bool is_type_visible_from_module(reflifc::Expression expr, reflifc::Module module_, ifc::Environment& environment)
{
	switch (expr.sort())
	{
	case ifc::ExprSort::NamedDecl:
		return is_type_visible_from_module(expr.referenced_decl(), module_, environment);
	case ifc::ExprSort::Type:
		return is_type_visible_from_module(expr.as_type(), module_, environment);
	case ifc::ExprSort::TemplateId:
		return is_type_visible_from_module(expr.as_template_id(), module_, environment);
	case ifc::ExprSort::Literal:
		return true; // ifc::LiteralSort only has the types uint32, uint64 or double. Each of these types are always visible. 
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
			expr_sort_to_string(expr.sort())));
	}
}

bool is_type_visible_from_module(reflifc::TemplateId template_id, reflifc::Module module_, ifc::Environment& environment)
{
	return is_type_visible_from_module(template_id.primary(), module_, environment) &&
		std::ranges::all_of(template_id.arguments(), [module_ , &environment](reflifc::Expression arg)
		{
			return is_type_visible_from_module(arg, module_, environment);
		});
}

template<typename T>
concept HasHomeScope = requires(T t) {
	{ t.home_scope() } -> std::same_as<reflifc::Declaration>;
};

reflifc::Declaration get_home_scope(const reflifc::Declaration& decl, ifc::Environment& environment)
{
	ContextArea area_{ "While getting the home scope."sv };

	return visit_declaration<reflifc::Declaration>(decl, environment, 
		[](HasHomeScope auto const& t) { return t.home_scope(); }
	);
}

template<typename T>
concept HasBasicSpecifiers = requires(T t) {
	{ t.specifiers() } -> std::same_as<ifc::BasicSpecifiers>;
};

ifc::BasicSpecifiers get_basic_specifiers(reflifc::Declaration decl, ifc::Environment& environment)
{
	ContextArea area_{ "While getting the basic specifiers."sv };

	return visit_declaration<ifc::BasicSpecifiers>(decl, environment,
		[] (HasBasicSpecifiers auto const& t) { return t.specifiers(); }
	);
}
