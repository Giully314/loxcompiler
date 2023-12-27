#ifndef LOX_TYPES_HPP
#define LOX_TYPES_HPP

/*
types.hpp

PURPOSE: Define the primitive Lox types.
*/

#include <string>
#include <variant>

#include "common.hpp"


namespace lox
{
    struct LoxNil
    {
        inline static const std::string value {"nil"};
    };

    // A literal in lox is a string or a double. In future I'm gonna add
    // support for integers and other primitive types.
    using Literal = std::variant<std::string, f64, bool, LoxNil>;
} // namespace lox


#endif
