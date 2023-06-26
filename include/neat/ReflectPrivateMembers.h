#pragma once

namespace Neat
{
    void reflect_types_and_members(); // Function stub to in which we register type data.
                                      // Friending this function will give NeatReflection access to private members.

#define REFLECT_PRIVATE_MEMBERS friend void ::Neat::reflect_types_and_members()
}