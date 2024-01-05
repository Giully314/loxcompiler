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
    // struct LoxNil : private NonCopyable, NonMovable
    // {
    //     static auto Instance()
    //         -> non_owned_ptr<LoxNil>
    //     {
    //         static LoxNil nil;
    //         return &nil;
    //     }

    //     inline static const std::string value {"nil"};

    // private:
    //     LoxNil() = default;
    // };

    struct LoxNil
    {
        inline static const std::string value {"nil"};
    };

    // A literal in lox is a string, double, nil or bool. 
    using Literal = std::variant<LoxNil, std::string, f64, bool>;
} // namespace lox


#endif
