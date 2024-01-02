#ifndef LOX_LLVM_VISITOR_HPP
#define LOX_LLVM_VISITOR_HPP

/*
llvm_visitor.hpp

PURPOSE: Implementation of a visitor for the AST node to produce llvm IR.

CLASSES:

DESCRIPTION:


TODO:
    - support for nested loops continue break (need to keep tracks of the loops using a stack).
*/



#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h> // FunctionType
#include <llvm/IR/Function.h> // Function
#include <llvm/IR/ValueHandle.h> // TrackingVH
#include <llvm/ADT/StringMap.h>
#include <llvm/IR/Instructions.h> // PHINode

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/APFloat.h>

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

        // Create the function main.
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


        auto Generate(const StmtNode& ast) 
            -> void;


    private:
        template <typename T>
            requires std::same_as<T, ExprNode> || std::same_as<T, Literal> || 
                std::same_as<T, StmtNode>
        auto Visit(const T& node)
            -> void
        {
            std::visit(*this, node);
        }


        // Visitor for expressions. Expressions always return a value (saved inside current_value).

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



        // Visitor for statements. 

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

        auto operator()(const IfStmtNodePtr& node)
            -> void;
        
        auto operator()(const WhileStmtNodePtr& node)
            -> void;


        // Visit for Literal.

        auto operator()(const LoxNil& value)
            -> void;
        
        auto operator()(const std::string& value)
            -> void;

        auto operator()(const f64& value)
            -> void;

        auto operator()(const bool& value)
            -> void;


    // Utility functions.
    private:

        // Set the current basic block and insert point.
        auto SetCurrentBlock(llvm::BasicBlock* bb)
            -> void 
        {
            current_block = bb;
            builder->SetInsertPoint(current_block);
        }

        
        auto WriteLocalVar(llvm::BasicBlock* bb, std::string_view name, llvm::Value* value)
            -> void
        {
            current_def[bb].defs[name] = value;
        }


        auto AddEmptyPhi(llvm::BasicBlock* bb, std::string_view name)
            -> llvm::PHINode*
        {
            // return bb->empty() ? 
            //         llvm::PHINode::Create()
        }

        auto ReadLocalVarRecursive(llvm::BasicBlock* bb, std::string_view name)
            -> llvm::Value*;


        auto ReadLocalVar(llvm::BasicBlock* bb, std::string_view name)
            -> llvm::Value*
        {
            if (auto value = current_def[bb].defs.find(name); value != current_def[bb].defs.end())
            {
                return value->second;
            }

            // Recurse through outer basic block to check if the name is defined outside.
            return ReadLocalVarRecutrsive(bb, name);
        }

    // Utility classes for internal usage.
    private:

        // This struct keeps track of declaration of names inside a basic block.
        struct BasicBlockDef
        {
            // It's safe to use a string_view because we are referring to a string
            // in the source code (it is freed after the llvm pass). 
            llvm::DenseMap<std::string_view, llvm::TrackingVH<llvm::Value>> defs;
            llvm::DenseMap<llvm::PHINode*, std::string_view> incomplete_phis;
            
            // true if we know all the predecessors of this block, false otherwise.
            bool sealed = false;
        };


    private:
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> mod;
        std::unique_ptr<llvm::IRBuilder<>> builder;
        
        llvm::StringMap<llvm::Value*> name_vars;

        // Map basic blocks to definitions inside the block
        llvm::DenseMap<llvm::BasicBlock*, BasicBlockDef> current_def;



        // Last value produced.
        llvm::Value* current_value;

        // Current function.
        llvm::Function* current_func;

        // Current block.
        llvm::BasicBlock* current_block;
    };
} // namespace lox



#endif
