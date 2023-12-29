#ifndef LOX_LOG_HPP
#define LOX_LOG_HPP

#include <string_view>
#include <iostream>

namespace lox
{
    inline auto Log(const std::string_view msg)
        -> void 
    {
        std::cout << msg << std::endl;
    }

    inline auto Error(const std::string_view msg)
        -> void
    {
        std::cout << msg << std::endl;
    }
} // namespace lox


#endif 