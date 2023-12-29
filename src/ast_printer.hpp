#ifndef LOX_AST_PRINTER_HPP
#define LOX_AST_PRINTER_HPP

/*
ast_printer.hpp

PURPOSE: Implementation of a printer for the AST.

CLASSES:
    ASTPrinter: implement visit for the traversal of the AST.

DESCRIPTION:
    The implementation is slow and bad, each operation returns a string, it would be better 
    to have an internal stringstream to build strings but for now it works for visualizing the
    AST.
*/

#include "node.hpp"
#include <variant>
#include <string_view>
#include <string>
#include <sstream>

namespace lox
{
    // TODO: use a strinstream private variable instaed of returing a string from operator().
    class ASTPrinter
    {
    public:
        auto operator()(const BinaryExprNodePtr& n) const
            -> std::string
        {
            // auto op = std::string{b->op.Lexeme()};
            return Parenthesize(n->op.Lexeme(), n->left, n->right);
        }


        auto operator()(const UnaryExprNodePtr& n) const
            -> std::string
        {
            return Parenthesize(n->op.Lexeme(), n->right);
        }


        auto operator()(const LiteralNodePtr& n) const 
            -> std::string
        {
            switch (n->literal.index())
            {
            case 0: // LoxNil
                return LoxNil::value;
            case 1: // std::string
                return *std::get_if<1>(&n->literal);
            case 2: // f64
                return std::to_string(*std::get_if<2>(&n->literal));
            case 3: // bool
                return *std::get_if<3>(&n->literal) ? "true" : "false";
            default:
                return "";
            }   
        }


        auto operator()(const GroupingNodePtr& n) const
            -> std::string
        {
            return Parenthesize("group", n->expr);
        }


        auto operator()(const AssignExprNodePtr& n) const
            -> std::string
        {
            return Parenthesize("Assign", n->expr);
        }

        auto operator()(const VarExprNodePtr& n) const
            -> std::string
        {
            return std::string{n->name.Lexeme()};
        }

        auto operator()(const LogicalExprNodePtr& n) const
            -> std::string
        {
            return Parenthesize(n->op.Lexeme(), n->left, n->right);
        }

        auto operator()(const CallExprNodePtr& n) const
            -> std::string 
        {
            return "No support";
        }

        auto operator()(const ExprStmtNodePtr& n) const
            -> std::string 
        {
            return Visit(n->expr);
        }

        auto operator()(const PrintStmtNodePtr& n) const
            -> std::string
        {
            return Parenthesize("Print", n->expr);
        }

        auto operator()(const VarStmtNodePtr& n) const 
            -> std::string
        {
            return Parenthesize(n->name.Lexeme(), n->initializer);
        }

        auto operator()(const BlockStmtNodePtr& n) const 
            -> std::string
        {
            std::stringstream ss;
            for (const auto& s : n->statements)
            {
                ss << Visit(s) << "\n";
            }
            return ss.str();
        }

        auto operator()(const FunStmtNodePtr& n) const 
            -> std::string
        {
            std::stringstream ss;
            ss << n->name.Lexeme();
            ss << "(";
            for (const auto& p : n->parameters)
            {
                ss << p.Lexeme() << ", ";
            }
            ss.seekp(-2, ss.cur);
            ss << ")\nBody:\n";
            ss << (*this)(n->body);
            return ss.str();
        }

        auto Parenthesize(const std::string_view name, const ExprNode& n) const 
            -> std::string
        {
            std::stringstream ss;
            ss << "(" << name << " ";
            ss << Visit(n);
            ss << ")";
            return ss.str();
        }

        auto Parenthesize(const std::string_view name, const ExprNode& n1, const ExprNode& n2) const
            -> std::string
        {
            std::stringstream ss;
            ss << "(" << name << " ";
            ss << Visit(n1) << "  ";
            ss << Visit(n2);
            ss << ")";
            return ss.str();
        }

        auto Visit(const ExprNode& n) const
            -> std::string
        {
            return std::visit(*this, n);
        }

        auto Visit(const StmtNode& n) const 
            -> std::string
        {
            return std::visit(*this, n);
        }

    //     auto ToString()
    //         -> std::string
    //     {
    //         return ss.str();
    //     }

    // private:
    //     std::stringstream ss;
    };
} // namespace lox

#endif
