#include "parser.hpp"

#include "log.hpp"

#include <memory>
#include <string>
#include <charconv>
#include <iostream>
#include <optional>

namespace lox
{

    auto Parser::Parse()
        -> std::vector<StmtNode>
    {
        std::vector<StmtNode> nodes;
        while (!IsAtEnd())
        {
            nodes.emplace_back(Declaration());
        }
        return nodes;
    }


    // ******************** DECLARATIONS **************************************

    auto Parser::Declaration()
        -> StmtNode
    {
        if (Match(TokenType::Var))
        {
            return VarDeclaration();
        }
        if (Match(TokenType::Fun))
        {
            return FunDeclaration();
        }
        else
        {
            return Statement();
        }
    }


    auto Parser::VarDeclaration()
        -> StmtNode
    {
        Consume(TokenType::Identifier, "Expect a variable name.");
        auto name = prev;
        ExprNode init = std::make_unique<LiteralNode>();
        
        if (Match(TokenType::Equal))
        {
            init = Expression();
        }
        Consume(TokenType::Semicolon, "Expect a ';' after variable declaration.");

        return std::make_unique<VarStmtNode>(std::move(name), std::move(init));
    }


    auto Parser::FunDeclaration()
        -> StmtNode
    {
        Consume(TokenType::Identifier, "Expect a function name.");
        auto fun_name = prev;

        Consume(TokenType::LeftParen, "Expect '(' after function name.");

        std::vector<Token> params;
        // Check for function parameters.
        if (!Check(TokenType::RightParen))
        {
            do
            {
                Consume(TokenType::Identifier, "Expect parameter name.");
                params.emplace_back(prev);
            } while (Match(TokenType::Comma));
        }

        Consume(TokenType::RightParen, "Expect ')' after function parameters.");
        Consume(TokenType::LeftBrace, "Expect '{' before function body.");

        // It is safe to cast directly the variant because we know the result of BlockStatement().
        auto bodyvar = BlockStatement();
        auto& body = *std::get_if<BlockStmtNodePtr>(&bodyvar);
        return std::make_unique<FunStmtNode>(std::move(fun_name),
            std::move(params), std::move(body));    
    }


    // ******************** DECLARATIONS **************************************


    // ******************** STATEMENTS **************************************

    auto Parser::Statement()
        -> StmtNode
    {
        if (Match(TokenType::Print))
        {
            return PrintStatement();   
        }
        else if (Match(TokenType::LeftBrace))
        {
            return BlockStatement();
        }
        else if (Match(TokenType::Return))
        {
            return ReturnStatement();
        }
        else if (Match(TokenType::For))
        {
            return ForStatement();
        } 
        else if (Match(TokenType::If))
        {
            return IfStatement();
        } 
        else if (Match(TokenType::While))
        {
            return WhileStatement();
        } 
        else 
        {
            return ExpressionStatement();
        }
    }


    auto Parser::ForStatement()
        -> StmtNode
    {
        // The for statement is written in terms of the while statement.

        Consume(TokenType::LeftParen, "Expect '(' after 'for'.");

        // Check for init condition.
        std::optional<StmtNode> initializer;
        if (Match(TokenType::Var)) // Var init
        {
            initializer = VarDeclaration();
        }
        else if (Match(TokenType::Semicolon)) // Empty init.
        {
            // Do nothing.
        }
        else // Expression init.
        {
            initializer = ExpressionStatement();
        }
        // Consume(TokenType::Semicolon, "Expect ';' after initializer");
        
        // Check for condition.
        std::optional<ExprNode> condition;
        if (!Check(TokenType::Semicolon))
        {
            condition = Expression();
        }

        Consume(TokenType::Semicolon, "Expect ';' after condition");

        // Check for increment.
        std::optional<StmtNode> increment;
        if (!Check(TokenType::RightParen))
        {
            increment = std::make_unique<ExprStmtNode>(Expression());
        }

        Consume(TokenType::RightParen, "Expect ')' after 'for' condition.");

        // Parse the body.
        auto body = Statement();

        // Construct the while loop from the for loop.
        if (increment)
        {
            body = std::make_unique<BlockStmtNode>(
                std::move(body),
                std::move(*increment)
            );
        }

        if (!condition)
        {
            condition = std::make_unique<LiteralNode>(true);
        }
        body = std::make_unique<WhileStmtNode>(std::move(*condition), std::move(body));

        if (initializer)
        {
            body = std::make_unique<BlockStmtNode>(std::move(*initializer), std::move(body));
        } 

        return body;
    }


    auto Parser::IfStatement()
        -> StmtNode
    {
        Consume(TokenType::LeftParen, "Expect '(' after 'if'.");

        auto condition = Expression();
        Consume(TokenType::RightParen, "Expect ')' after 'if' condition.");

        auto then = Statement();

        std::optional<StmtNode> else_branch;
        if (Match(TokenType::Else))
        {
            else_branch = Statement();
        }

        return std::make_unique<IfStmtNode>(std::move(condition), std::move(then),
                std::move(else_branch));    
    }


    auto Parser::WhileStatement()
        -> StmtNode
    {
        Consume(TokenType::LeftParen, "Expect '(' after 'while'");
        
        auto condition = Expression();
        
        Consume(TokenType::RightParen, "Expect ')' after 'while' condition");

        auto statement = Statement();

        return std::make_unique<WhileStmtNode>(
            std::move(condition),
            std::move(statement)
        ); 
    }


    auto Parser::PrintStatement()
        -> StmtNode
    {
        auto expr = Expression();
        Consume(TokenType::Semicolon, "Expect ';' after print.");
        return std::make_unique<PrintStmtNode>(std::move(expr));
    }


    auto Parser::BlockStatement()
        -> StmtNode
    {
        std::vector<StmtNode> statements;
        while (!(IsAtEnd() || Check(TokenType::RightBrace)))
        {
            statements.emplace_back(Declaration());
        }
        Consume(TokenType::RightBrace, "Expect '}' after block.");
        return std::make_unique<BlockStmtNode>(std::move(statements));
    }


    auto Parser::ExpressionStatement()
        -> StmtNode
    {
        auto expr = std::make_unique<ExprStmtNode>(Expression());
        Consume(TokenType::Semicolon, "Expect ';' after statement.");
        return expr;
    }


    auto Parser::ReturnStatement()
        -> StmtNode 
    {
        // Save the return token in case of error report.
        auto keyword = prev;
        // Default value is nil.
        ExprNode expr = std::make_unique<LiteralNode>();
        
        // Check if the return returns a value 
        if (!Check(TokenType::Semicolon))
        {
            expr = Expression();
        }
        Consume(TokenType::Semicolon, "Expect ';' after return value.");
        return std::make_unique<ReturnStmtNode>(std::move(keyword), std::move(expr));
    }

    // ******************** STATEMENTS **************************************


    // ******************** EXPRESSIONS **************************************

    auto Parser::Expression()
        -> ExprNode
    {
        return Assignment();
    }

    auto Parser::Assignment()
        -> ExprNode
    {
        auto expr = LogicOr();

        // TODO: support for class setter
        // If we match the "=" we need to check if the variable is a field of an instance.
        // and do the parsing with the right precedence even for nested access like
        // instance_a.b.c = 4;
        if (Match(TokenType::Equal))
        {
            auto equals = prev;

            // This allow nested assignment like a = b = c;
            auto value = Assignment();

            // Check if expr is a variableexpr (an identifier)
            if (auto v = std::get_if<VarExprNodePtr>(&expr))
            {
                return std::make_unique<AssignExprNode>((*v)->name, std::move(value));
            }

            // report an error if the left side of the assignment is not an identifier.
            ErrorAt(equals, "Invalid target assignment");
        }

        return expr;
    }


    auto Parser::LogicOr()
        -> ExprNode
    {
        auto expr = LogicAnd();

        if (Match(TokenType::Or))
        {
            auto op = prev;
            auto right = LogicAnd();
            expr = std::make_unique<LogicalExprNode>(std::move(op), 
                std::move(expr), std::move(right));
        }
        return expr;
    }


    auto Parser::LogicAnd()
        -> ExprNode
    {
        auto expr = Equality();

        if (Match(TokenType::And))
        {
            auto op = prev;
            auto right = Equality();
            expr = std::make_unique<LogicalExprNode>(std::move(op), 
                std::move(expr), std::move(right));
        }
        return expr;
    }


    auto Parser::Equality()
        -> ExprNode
    {
        auto expr = Comparison();

        if (Match({TokenType::BangEqual, TokenType::EqualEqual}))
        {
            auto op = prev;
            auto right = Comparison();
            expr = std::make_unique<BinaryExprNode>(std::move(op),
                std::move(expr), std::move(right));
        }

        return expr;
    }


    auto Parser::Comparison()
        -> ExprNode
    {
        auto expr = Term();

        if (Match({TokenType::Less, TokenType::LessEqual, 
                TokenType::Greater, TokenType::GreaterEqual}))
        {
            auto op = prev;
            auto right = Term();
            expr = std::make_unique<CompExprNode>(std::move(op),
                std::move(expr), std::move(right));
        }
        return expr;
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
        return Call();
    }


    auto Parser::Call()
        -> ExprNode
    {
        auto expr = Primary();

        // Parse function arguments.
        // TODO: add support for properties when classes will be supported.
        while (Match(TokenType::LeftParen))
        {
            std::vector<ExprNode> args;
            if (!Check(TokenType::RightParen))
            {
                do
                {
                    args.emplace_back(Expression());
                } while (Match(TokenType::Comma));
            }

            Consume(TokenType::RightParen, "Expect ')' after arguments.");
            auto paren = prev;
            expr = std::make_unique<CallExprNode>(std::move(paren),
                std::move(expr), std::move(args));
        }

        return expr;
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
                ErrorAtCurrent("Error converting number to string.");
            }
            return std::make_unique<LiteralNode>(d);
        }
        else if (Match(TokenType::String))
        {
            return std::make_unique<LiteralNode>(std::string{prev.Lexeme()});
        }
        else if (Match(TokenType::Identifier))
        {
            return std::make_unique<VarExprNode>(prev);
        }   
        else
        {
            // Error, not supported type or invalid token.
            ErrorAtCurrent("Invalid literal token.");
            return std::make_unique<LiteralNode>(LoxNil{});
        }
    }

    // ******************** EXPRESSIONS **************************************


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

        std::cout << ": " << msg << std::endl;
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
