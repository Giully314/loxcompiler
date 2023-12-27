#ifndef LOX_NODE_HPP
#define LOX_NODE_HPP

/*
node.hpp

PURPOSE: Define type of nodes in the AST.

CLASSES:
    GroupingNode:   node for grouping expression.
    BinaryExprNode: node for binary operators (+, -, *, /).
    UnaryExprNode:  node for unary operators (!, -).
    LiteralNode:    node for primitive types in Lox (double, string, nil). Check types.hpp for more info.

DESCRIPTION:
    The implementation is based on std::variant to explore building an AST (and traverse it) using a functional approach.
*/


#include <variant>
#include <memory>
#include <utility>

#include "token.hpp"
#include "types.hpp"

namespace lox
{
    struct GroupingNode;
    struct BinaryExprNode;
    struct UnaryExprNode;
    struct LiteralNode;


    using BinaryExprNodePtr = std::unique_ptr<BinaryExprNode>;
    using UnaryExprNodePtr = std::unique_ptr<UnaryExprNode>;
    using LiteralNodePtr = std::unique_ptr<LiteralNode>;
    using GroupingNodePtr = std::unique_ptr<GroupingNode>;


    using ExprNode = std::variant<BinaryExprNodePtr, UnaryExprNodePtr,
                        LiteralNodePtr, GroupingNodePtr>;


    struct GroupingNode
    {
        explicit GroupingNode(ExprNode node) : 
            expr(std::move(node)) { }

        ExprNode expr;
    };

    struct BinaryExprNode
    {   
        explicit BinaryExprNode(Token op_, ExprNode left_, ExprNode right_) :
            op(std::move(op_)), left(std::move(left_)), right(std::move(right_)) { }

        Token op;
        ExprNode left;
        ExprNode right;
    };

    struct UnaryExprNode
    {
        explicit UnaryExprNode(Token op_, ExprNode right_) :
            op(std::move(op_)), right(std::move(right_)) { }

        Token op;
        ExprNode right;
    };


    struct LiteralNode
    {
        explicit LiteralNode(Literal literal_) :
            literal(std::move(literal_)) { }

        Literal literal;
    };

} // namespace lox


#endif 
