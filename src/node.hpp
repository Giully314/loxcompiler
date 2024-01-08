#ifndef LOX_NODE_HPP
#define LOX_NODE_HPP

/*
node.hpp

PURPOSE: Define type of nodes in the AST.

CLASSES:
    GroupingNode:       node for grouping expression.
    BinaryExprNode:     node for binary operators (+, -, *, /).
    UnaryExprNode:      node for unary operators (!, -).
    LiteralNode:        node for primitive types in Lox (double, string, nil). Check types.hpp for more info.
    AssignExprNode:     node for assignment.
    VarExprNode:        node for var expression (identifier).
    LogicalExprNode:    node for logical operators.
    CallExprNode:       node for function call.
    CompExprNode:       node for binary comparison

    ExprStmtNode:       node for expression statement.
    PrintStmtNode:      node for print statement.
    VarStmtNode:        node for var declaration.
    BlockStmtNode:      node for block statement (list of statements).      
    FunStmtNode:        node for function declaration.
    ReturnStmtNode:     node for return statement.
    IfStmtNode:         node for if/then/else statement.
    WhileStmtNode:      node for while statetement. This node is also used for the for statement after some change (check the parser).

DESCRIPTION:
    The implementation is based on std::variant to explore building an AST (and traverse it) and avoid dynamic dispatch.

*/


#include <variant>
#include <optional>
#include <memory>
#include <utility>
#include <vector>
#include <initializer_list>
#include <string>

#include "token.hpp"
#include "types.hpp"

namespace lox
{
    struct GroupingNode;
    struct BinaryExprNode;
    struct UnaryExprNode;
    struct LiteralNode;
    struct AssignExprNode;
    struct VarExprNode;
    struct LogicalExprNode;
    struct CallExprNode;
    struct CmpExprNode;

    using BinaryExprNodePtr = std::unique_ptr<BinaryExprNode>;
    using UnaryExprNodePtr = std::unique_ptr<UnaryExprNode>;
    using LiteralNodePtr = std::unique_ptr<LiteralNode>;
    using GroupingNodePtr = std::unique_ptr<GroupingNode>;
    using AssignExprNodePtr = std::unique_ptr<AssignExprNode>;
    using VarExprNodePtr = std::unique_ptr<VarExprNode>;
    using LogicalExprNodePtr = std::unique_ptr<LogicalExprNode>;
    using CallExprNodePtr = std::unique_ptr<CallExprNode>;
    using CmpExprNodePtr = std::unique_ptr<CmpExprNode>;


    using ExprNode = std::variant<BinaryExprNodePtr, UnaryExprNodePtr,
                        LiteralNodePtr, GroupingNodePtr,
                        AssignExprNodePtr, VarExprNodePtr, LogicalExprNodePtr,
                        CallExprNodePtr, CmpExprNodePtr>;


    struct ExprStmtNode;
    struct PrintStmtNode;
    struct VarStmtNode;
    struct BlockStmtNode;
    struct FunStmtNode;
    struct ReturnStmtNode;
    struct IfStmtNode;
    struct WhileStmtNode;

    using ExprStmtNodePtr = std::unique_ptr<ExprStmtNode>;
    using PrintStmtNodePtr = std::unique_ptr<PrintStmtNode>;
    using VarStmtNodePtr = std::unique_ptr<VarStmtNode>;
    using BlockStmtNodePtr = std::unique_ptr<BlockStmtNode>;
    using FunStmtNodePtr = std::unique_ptr<FunStmtNode>;
    using ReturnStmtNodePtr = std::unique_ptr<ReturnStmtNode>;
    using IfStmtNodePtr = std::unique_ptr<IfStmtNode>;
    using WhileStmtNodePtr = std::unique_ptr<WhileStmtNode>;

    

    using StmtNode = std::variant<ExprStmtNodePtr, PrintStmtNodePtr,
                        VarStmtNodePtr, BlockStmtNodePtr, FunStmtNodePtr,
                        ReturnStmtNodePtr, IfStmtNodePtr, WhileStmtNodePtr>;


    // ********************** EXPRESSION NODE *************************************

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
        // default nil literal value.
        explicit LiteralNode() = default;

        explicit LiteralNode(Literal literal_) :
            literal(std::move(literal_)) { }

        Literal literal;
    };


    struct AssignExprNode
    {
        explicit AssignExprNode(Token name_, ExprNode expr_) : 
            name(std::move(name_)), expr(std::move(expr_)) { }
        
        Token name;
        ExprNode expr;
    };


    struct VarExprNode
    {
        explicit VarExprNode(Token name_) : 
            name(std::move(name_)) { }
        Token name;
    };

    
    struct LogicalExprNode
    {
        explicit LogicalExprNode(Token op_, ExprNode left_, ExprNode right_) :
            op(std::move(op_)), left(std::move(left_)), right(std::move(right_)) { }

        Token op;
        ExprNode left;
        ExprNode right;
    };


    struct CallExprNode
    {
        explicit CallExprNode(Token paren_, std::string callee_, std::vector<ExprNode> args) :
            paren(std::move(paren_)), callee(std::move(callee_)), arguments(std::move(args)) { }

        // This token is stored to report errors at runtime or during compilation in case 
        // the function call is not right.
        Token paren;
        std::string callee;
        std::vector<ExprNode> arguments;
    };


    struct CmpExprNode
    {   
        explicit CmpExprNode(Token op_, ExprNode left_, ExprNode right_) :
            op(std::move(op_)), left(std::move(left_)), right(std::move(right_)) { }

        Token op;
        ExprNode left;
        ExprNode right;
    };


    // ************************ STATEMENT NODE **************************************

    struct ExprStmtNode
    {
        explicit ExprStmtNode(ExprNode expr_) : 
            expr(std::move(expr_)) { }

        ExprNode expr;
    };


    struct PrintStmtNode
    {
        explicit PrintStmtNode(ExprNode expr_) : 
            expr(std::move(expr_)) { }

        ExprNode expr;
    };


    struct VarStmtNode
    {
        explicit VarStmtNode(Token name_, ExprNode init) : 
            name(std::move(name_)), initializer(std::move(init)) { }

        Token name;
        ExprNode initializer;
    };


    struct BlockStmtNode
    {
        explicit BlockStmtNode(StmtNode node) 
        {
            statements.emplace_back(std::move(node));
        }

        explicit BlockStmtNode(StmtNode node1, StmtNode node2) 
        {
            statements.reserve(2);
            statements.emplace_back(std::move(node1));
            statements.emplace_back(std::move(node2));
        }

        explicit BlockStmtNode(std::vector<StmtNode> s) :
            statements(std::move(s)) { }

        std::vector<StmtNode> statements;
    };

    
    struct FunStmtNode
    {
        explicit FunStmtNode(Token name_, std::vector<Token> params, BlockStmtNodePtr body_) :
            name(std::move(name_)), parameters(std::move(params)), body(std::move(body_)) { }

        Token name;
        std::vector<Token> parameters;
        BlockStmtNodePtr body;
    };


    struct ReturnStmtNode
    {
        explicit ReturnStmtNode(Token keyword_, ExprNode value_) : 
            keyword(std::move(keyword_)), value(std::move(value_)) { }

        Token keyword;
        ExprNode value;
    };


    struct IfStmtNode
    {
        explicit IfStmtNode(ExprNode condition_, StmtNode then_branch_) : 
            condition(std::move(condition_)), then_branch(std::move(then_branch_)) { }

        explicit IfStmtNode(ExprNode condition_, StmtNode then_branch_, StmtNode else_branch_) : 
            condition(std::move(condition_)), then_branch(std::move(then_branch_)),
            else_branch(std::move(else_branch_)) { }

        explicit IfStmtNode(ExprNode condition_, StmtNode then_branch_, std::optional<StmtNode> else_branch_) : 
            condition(std::move(condition_)), then_branch(std::move(then_branch_)),
            else_branch(std::move(else_branch_)) { }

        ExprNode condition;
        StmtNode then_branch;
        std::optional<StmtNode> else_branch;
    };


    struct ForStmtNode
    {
        explicit ForStmtNode();

        std::optional<StmtNode> initializer;
        std::optional<ExprNode> condition;
        std::optional<ExprNode> increment;
        BlockStmtNodePtr body;
    };


    struct WhileStmtNode
    {
        explicit WhileStmtNode(ExprNode condition_, StmtNode body_) : 
            condition(std::move(condition_)), body(std::move(body_)) { }

        ExprNode condition;
        StmtNode body;
    };

} // namespace lox


#endif 
