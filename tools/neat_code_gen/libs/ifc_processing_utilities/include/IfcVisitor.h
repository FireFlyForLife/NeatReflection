#pragma once

#include "ContextualException.h"
#include "IfcConversion.h"

#include <ifc/Declaration.h>
#include <ifc/Environment.h>
#include <reflifc/Declaration.h>
#include <reflifc/decl/DeclarationReference.h>

#include <format>
#include <type_traits>
#include <string_view>


template<typename... Ts>
struct Overloaded : Ts... {
	using Ts::operator()...;
};

template<typename Return, typename Callable, typename... TArgs>
Return try_invoke(Callable&& c, TArgs&&... args) {
	if constexpr (std::is_invocable_r_v<Return, Callable, TArgs...>) {
		return c(std::forward<TArgs>(args)...);
	} else {
		throw ContextualException{ "Could not visit the declaration." };
	}
}

template<typename Return, typename Callable>
Return visit_declaration(reflifc::Declaration decl, ifc::Environment& environment, Callable&& c) {
	switch (decl.sort())
	{
	case ifc::DeclSort::VendorExtension: {
		break; 
	}
	case ifc::DeclSort::Enumerator: { 
		break; 
	}
	case ifc::DeclSort::Variable: { 
		return try_invoke<Return>(c, decl.as_variable());
	}
	case ifc::DeclSort::Parameter: { 
		return try_invoke<Return>(c, decl.as_parameter());
	}
	case ifc::DeclSort::Field: { 
		return try_invoke<Return>(c, decl.as_field());
	}
	case ifc::DeclSort::Bitfield: { 
		break; 
	}
	case ifc::DeclSort::Scope: { 
		return try_invoke<Return>(c, decl.as_scope());
	}
	case ifc::DeclSort::Enumeration: { 
		return try_invoke<Return>(c, decl.as_enumeration());
	}
	case ifc::DeclSort::Alias: { 
		return try_invoke<Return>(c, decl.as_alias());
	}
	case ifc::DeclSort::Temploid: { 
		break; 
	}
	case ifc::DeclSort::Template: { 
		return try_invoke<Return>(c, decl.as_template());
	}
	case ifc::DeclSort::PartialSpecialization: { 
		return try_invoke<Return>(c, decl.as_partial_specialization());
	}
	case ifc::DeclSort::Specialization: { 
		return try_invoke<Return>(c, decl.as_specialization());
		break; 
	}
	case ifc::DeclSort::DefaultArgument: { 
		break; 
	}
	case ifc::DeclSort::Concept: { 
		return try_invoke<Return>(c, decl.as_concept());
	}
	case ifc::DeclSort::Function: { 
		return try_invoke<Return>(c, decl.as_function());
	}
	case ifc::DeclSort::Method: { 
		return try_invoke<Return>(c, decl.as_method());
	}
	case ifc::DeclSort::Constructor: { 
		return try_invoke<Return>(c, decl.as_constructor());
	}
	case ifc::DeclSort::InheritedConstructor: { 
		break; 
	}
	case ifc::DeclSort::Destructor: { 
		return try_invoke<Return>(c, decl.as_destructor());
	}
	case ifc::DeclSort::Reference: { 
		auto referenced_decl = decl.as_reference().referenced_declaration(environment);
		return visit_declaration<Return>(referenced_decl, environment, std::forward<Callable>(c));
	}
	case ifc::DeclSort::UsingDeclaration: { 
		return try_invoke<Return>(c, decl.as_using());
	}     
	case ifc::DeclSort::UsingDirective: { 
		break; 
	}
	case ifc::DeclSort::Friend: { 
		break; 
	}
	case ifc::DeclSort::Expansion: { 
		break; 
	}
	case ifc::DeclSort::DeductionGuide: { 
		break; 
	}
	case ifc::DeclSort::Barren: { 
		break; 
	}
	case ifc::DeclSort::Tuple: { 
		break; 
	}
	case ifc::DeclSort::SyntaxTree: { 
		break; 
	}
	case ifc::DeclSort::Intrinsic: { 
		return try_invoke<Return>(c, decl.as_intrinsic());
	}
	case ifc::DeclSort::Property: { 
		break; 
	}
	case ifc::DeclSort::OutputSegment: { 
		break; 
	}
	default: {
		break;
	}
	}

	throw ContextualException(std::format("Could not visit the declaration, reflifc doesn't support it yet. DeclSort: {}", 
		decl_sort_to_string(decl.sort())));
}
