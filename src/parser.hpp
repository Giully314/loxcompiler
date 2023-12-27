#ifndef LOX_PARSER_HPP
#define LOX_PARSER_HPP

/*
parser.hpp

PURPOSE: Define the parser for the Lox language.

CLASSES:
    Parser: A recursive descent parser.

DESCRIPTION:

GRAMMAR:
https://craftinginterpreters.com/appendix-i.html
*/


#include "token.hpp"
#include "scanner.hpp"
#include "common.hpp"
#include "node.hpp"

#include <string_view>
#include <initializer_list>


namespace lox
{
    class Parser
    {
    public:
        explicit Parser(non_owned_ptr<Scanner> scanner_) :
            scanner(scanner_)
        {
            Advance();
        }

        auto Parse()
            -> ExprNode;


    // Parser internal methods to handle tokens.
    private:
        auto ErrorAt(const Token& t, const std::string_view msg)
            -> void;

        auto ErrorAtCurrent(const std::string_view msg)
            -> void
        {
            ErrorAt(current, msg);
        }

        // Get the next token inside current.
        // If the scanner is at the end, current is the EOF token.
        auto Advance()
            -> void;


        // Check if the current token is of the same type as the argument.
        constexpr auto Check(const TokenType type) const noexcept
            -> bool
        {
            return current.Type() == type;
        }

        // Return true and advance the scanner if the current token match the argument,
        // false otherwise.
        auto Match(const TokenType type) 
            -> bool;
        
        // Return true and advance the scanner if the current token match one of the types,
        // false otherwise.
        auto Match(const std::initializer_list<TokenType> types)
            -> bool;

        // Consume the current token if it has the same type as the argument,
        // otherwise print an error and set the parser in error mode.
        auto Consume(const TokenType type, const std::string_view msg)
            -> void; 
        

    private:
        auto Expression() 
            ->ExprNode;

        auto Term() 
            ->ExprNode;

        auto Factor() 
            ->ExprNode;

        auto Unary() 
            ->ExprNode;

        auto Primary() 
            ->ExprNode;

    private:
        non_owned_ptr<Scanner> scanner;
        Token current;
        Token prev;

        // Signal an error during parsing.
        bool had_error{ false };

        // When true the parser synchronize itself to get in a good state and trying
        // to continue the parsing. This is done because we want to catch all the errors
        // in one pass.
        bool panic_mode{ false };
    };
} // namespace lox


#endif 
