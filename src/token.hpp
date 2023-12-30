#ifndef LOX_TOKEN_HPP
#define LOX_TOKEN_HPP

/*
token.hpp

PURPOSE: Define token used the Scanner and the Parser as atomic building block.

CLASSES:
    TokenType: Enum for the type of the token.
    Token: Info about the lox token.

DESCRIPTION:
    
*/

#include "common.hpp"
#include <string_view>


namespace lox
{
    enum class TokenType : std::uint8_t
    {
        // Single-character tokens.
        LeftParen, RightParen,
        LeftBrace, RightBrace,
        Comma, Dot, Minus, Plus,
        Semicolon, Slash, Star,
        // One or two character tokens.
        Bang, BangEqual,
        Equal, EqualEqual,
        Greater, GreaterEqual,
        Less, LessEqual,
        // Literals.
        Identifier, String, Number,
        // Keywords.
        And, Class, Else, False,
        For, Fun, If, Nil, Or,
        Print, Return, Super, This,
        True, Var, While,

        Error, Eof
    };


    class Token
    {
    public:
        constexpr explicit Token() = default;
        
        constexpr explicit Token(std::string_view lexeme_, TokenType type_, std::uint32_t line_) : lexeme(lexeme_), type(type_),
            line(line_) { }
        
        constexpr explicit Token(TokenType type_) : type(type_) { }


        constexpr auto Lexeme() const noexcept
            -> std::string_view
        {
            return lexeme;
        }

        constexpr auto Type() const noexcept
            -> TokenType
        {
            return type;
        }

        constexpr auto TypeInt() const noexcept
            -> u16
        {
            return static_cast<u16>(type);
        }


        constexpr auto Line() const noexcept
            -> std::uint32_t
        {
            return line;
        }


    private:
        // Point to source code 
        std::string_view lexeme{}; 
        
        TokenType type{TokenType::Error};   
        
        std::uint32_t line{0};
    };


} // namespace lox


#endif 
