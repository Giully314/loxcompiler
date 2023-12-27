#ifndef LOX_SCANNER_HPP
#define LOX_SCANNER_HPP
/*
scanner.hpp

PURPOSE: Provide a tokenizer for the Lox language.

*/


#include "token.hpp"
#include "common.hpp"

#include <string_view>


namespace lox
{
    class Scanner
    {
    public:

        // pointer to the source code. Make sure that the source code outlives the scanner and until it produces EOF.
        explicit Scanner(std::string_view text_) : text(text_)
        {

        }

        // Return the next token.
        Token NextToken();

        bool IsAtEnd() const noexcept
        {
            // TODO: is correct or text.size() -1?
            return current == text.size();
        }

    private:
        char Advance() noexcept
        {
            return text[current++];
        }

        char Peek() const noexcept
        {
            return text[current];
        }

        char PeekNext() const noexcept
        {
            if (IsAtEnd())
            {
                return '\0';
            }
            return text[current+1];
        }

        bool Match(char c)
        {
            if (IsAtEnd())
            {
                return false;
            }

            if (text[current] != c)
            {
                return false;
            }
            ++current;
            return true;
        }

        TokenType CheckKeyword(uint32_t s, std::string_view rest, TokenType type)
        {
            auto subname = text.substr(start + s, rest.size());
            if (subname == rest)
            {
                return type;
            }
            return TokenType::Identifier;
        }


        // Skip every whitespace and increment line in case of newline.
        void SkipWhitespace();

        Token String();
        Token Number();
        Token Identifier();
        TokenType IdentifierType();

        Token MakeToken(TokenType type)
        {
            return Token{text.substr(start, current - start), type, line};
        }

        Token ErrorToken(std::string_view msg)
        {
            return Token{msg, TokenType::Error, line};
        }

    private:
        std::string_view text;
        
        // Start of the current token.
        std::uint32_t start{0};

        // Current idx.
        std::uint32_t current{0};
        
        // Keep track of the current line.
        std::uint32_t line{0};

    };

} // namespace lox



#endif
