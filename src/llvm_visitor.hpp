#ifndef LOX_LLVM_VISITOR_HPP
#define LOX_LLVM_VISITOR_HPP

/*
llvm_visitor.hpp

PURPOSE: Implementation of a visitor for the AST node to produce llvm IR.

CLASSES:

DESCRIPTION:


*/


#include <llvm/ADT/APFloat.h>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h> // FunctionType
#include <llvm/IR/Function.h> // Function

#include <llvm/Support/raw_ostream.h>

#include "common.hpp"
#include "node.hpp"

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <string_view>
#include <concepts>


namespace lox
{
    class LLVMVisitor
    {
    public:
        explicit LLVMVisitor();

        // ~LLVMVisitor();
        
        // Temporary method 
        auto End()
            -> void;

        auto Print()
            -> void
        {
            mod->print(llvm::outs(), nullptr);
        }


        template <typename T>
            requires std::same_as<T, ExprNode> || std::same_as<T, StmtNode> ||
                std::same_as<T, Literal>
        auto Visit(const T& node)
            -> void
        {
            std::visit(*this, node);
        }


        auto operator()(const GroupingNodePtr& node)
            -> void;

        auto operator()(const BinaryExprNodePtr& node)
            -> void;

        auto operator()(const UnaryExprNodePtr& node)
            -> void;
        
        auto operator()(const LiteralNodePtr& node)
            -> void;

        auto operator()(const AssignExprNodePtr& node)
            -> void;

        auto operator()(const VarExprNodePtr& node)
            -> void;

        auto operator()(const LogicalExprNodePtr& node)
            -> void;

        auto operator()(const CallExprNodePtr& node)
            -> void;

        auto operator()(const CompExprNodePtr& node)
            -> void;


        auto operator()(const ExprStmtNodePtr& node)
            -> void;

        auto operator()(const PrintStmtNodePtr& node)
            -> void;

        auto operator()(const VarStmtNodePtr& node)
            -> void;

        auto operator()(const BlockStmtNodePtr& node)
            -> void;

        auto operator()(const FunStmtNodePtr& node)
            -> void;

        auto operator()(const ReturnStmtNodePtr& node)
            -> void;


    private:

        // Visit for Literal.

        auto operator()(const LoxNil& value)
            -> void;
        
        auto operator()(const std::string& value)
            -> void;

        auto operator()(const f64& value)
            -> void;

        auto operator()(const bool& value)
            -> void;


    private:
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> mod;
        std::unique_ptr<llvm::IRBuilder<>> builder;
        
        std::unordered_map<std::string, non_owned_ptr<llvm::Value>> named_values;
        std::unordered_map<std::string, non_owned_ptr<llvm::Value>> variables;

        // Last value produced.
        llvm::Value* value;

        // Current function.
        llvm::Function* func;

        // Current block.
        llvm::BasicBlock* block;

        // Temp
        llvm::FunctionType* ft;
        llvm::Function* f;
    };
} // namespace lox



#endif
