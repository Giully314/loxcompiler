#ifndef LOX_AST_PRINTER_HPP
#define LOX_AST_PRINTER_HPP

/*
ast_printer.hpp

PURPOSE: Implementation of a printer for the AST.

CLASSES:
    ASTPrinter: implement visit for the traversal of the AST.
*/

#include "node.hpp"
#include <variant>
#include <string_view>
#include <string>
#include <sstream>

namespace lox
{
    struct ASTPrinter
    {
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
            case 0: // std::string
                return *std::get_if<0>(&n->literal);
            case 1: // f64
                return std::to_string(*std::get_if<1>(&n->literal));
            case 2: // LoxNil
                return LoxNil::value;
            default:
                return "";
            }   
        }


        auto operator()(const GroupingNodePtr& n) const
            -> std::string
        {
            return Parenthesize("group", n->expr);
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
            constexpr auto size = 4;
            switch (n.index())
            {
            case 0: // BinaryExprNodePtr
            {
                if constexpr (0 < size)
                {
                    return (*this)(*std::get_if<0>(&n));
                }
            }

            case 1: // UnaryExprNodePtr
            {
                if constexpr (1 < size)
                {
                    return (*this)(*std::get_if<1>(&n));
                } 
            }    
                
            case 2: // LiteralNodePtr
            {
                if constexpr (2 < size)
                {
                    return (*this)(*std::get_if<2>(&n));
                } 
            }
            
            case 3: // GroupingNodePtr
            {
                if constexpr (3 < size)
                {
                    return (*this)(*std::get_if<3>(&n));
                } 
            }

            default:
                return "";

            }
        }
    };
} // namespace lox

#endif
