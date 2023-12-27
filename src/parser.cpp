#include "parser.hpp"
#include <memory>
#include <string>
#include <charconv>
#include <iostream>

namespace lox
{

    auto Parser::Parse()
        -> ExprNode
    {
        return Expression();
    }

    auto Parser::Expression()
        -> ExprNode
    {
        return Term();
    }

    auto Parser::Term()
        -> ExprNode
    {
        // Parse left expression.
        auto expr = Factor();

        // Parse operator - or + if present and right expressions.
        while (Match({TokenType::Minus, TokenType::Plus}))
        {
            auto op = prev;
            auto right = Factor();
            expr = std::make_unique<BinaryExprNode>(
                std::move(op),
                std::move(expr),
                std::move(right)
            );
        }
        return expr;
    }


    auto Parser::Factor()
        -> ExprNode
    {
        // Parse left expression.
        auto expr = Unary();

        // Parse operator / or * if present and right expression.
        while (Match({TokenType::Slash, TokenType::Star}))
        {
            auto op = prev;
            auto right = Unary();
            expr = std::make_unique<BinaryExprNode>(
                std::move(op),
                std::move(expr),
                std::move(right)
            );
        }
        return expr;
    }

    
    auto Parser::Unary()
        -> ExprNode
    {
        if (Match({TokenType::Bang, TokenType::Minus}))
        {
            auto op = prev;
            auto right = Unary();
            return std::make_unique<UnaryExprNode>(
                std::move(op),
                std::move(right)
            );
        }
        return Primary();
    }


    auto Parser::Primary()
        -> ExprNode
    {
        if (Match(TokenType::True))
        {
            return std::make_unique<LiteralNode>(true);
        }
        else if (Match(TokenType::False))
        {
            return std::make_unique<LiteralNode>(false);
        }
        else if (Match(TokenType::Number))
        {
            f64 d;
            auto r = std::from_chars(prev.Lexeme().begin(), prev.Lexeme().end(), d);
            if (r.ec == std::errc::invalid_argument)
            {
                // TODO: report error
            }
            return std::make_unique<LiteralNode>(d);
        }
        else if (Match(TokenType::String))
        {
            return std::make_unique<LiteralNode>(std::string{prev.Lexeme()});
        }   
        else
        {
            // Error, not supported type or invalid token.
            ErrorAtCurrent("Invalid literal token.");
            return std::make_unique<LiteralNode>(LoxNil{});
        }
    }



    // ***************** ERROR HANDLING *******************************
    
    auto Parser::ErrorAt(const Token& t, const std::string_view msg)
        -> void
    {
        if (panic_mode) return;
        panic_mode = true;

        // TODO: refactor with std::format. (Right now I'm using clang14 and isn't available)
        std::cout << "[line " << t.Line() << "] Error"; 

        switch (t.Type())
        {
            case TokenType::Eof: 
                std::cout << " at end";
                break;
            case TokenType::Error:
                break;
            default:
                std::cout << " at " << t.Lexeme();
        }

        std::cout << msg << std::endl;
        had_error = true;
    }



    // ****************** UTILITY ******************************************
    
    auto Parser::Advance()
        -> void
    {
        prev = current;
        if (scanner->IsAtEnd()) [[unlikely]]
        {
            current = Token{TokenType::Eof};
            return;
        }
        current = scanner->NextToken();
    }


    auto Parser::Match(const TokenType type)
        -> bool
    {
        if (Check(type))
        {
            Advance();
            return true;
        }        
        return false;
    }


    auto Parser::Match(const std::initializer_list<TokenType> types)
        -> bool
    {
        for (const auto type : types)
        {
            if (Check(type))
            {
                Advance();
                return true;
            }        
        }
        return false;
    }


    auto Parser::Consume(const TokenType type, const std::string_view msg)
        -> void
    {
        if (Check(type))
        {
            Advance();
            return;
        }

        ErrorAtCurrent(msg);
    }
} // namespace lox
