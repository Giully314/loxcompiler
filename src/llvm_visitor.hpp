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


namespace lox
{
    class LLVMVisitor
    {
    public:
        explicit LLVMVisitor();

        // ~LLVMVisitor();

        auto operator()(const LiteralNodePtr& node)
            -> void;

        auto operator()(const BinaryExprNodePtr& node)
            -> void;

        auto operator()(const UnaryExprNodePtr& node)
            -> void;

        auto operator()(const GroupingNodePtr& node)
            -> void;


        auto Visit(const ExprNode& node)
            -> void;

        // Temporary method 
        auto End()
            -> void;

        auto Print()
            -> void
        {
            mod->print(llvm::outs(), nullptr);
        }

    private:
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> mod;
        std::unique_ptr<llvm::IRBuilder<>> builder;
        std::unordered_map<std::string, non_owned_ptr<llvm::Value>> named_values;

        // Temporary 
        std::vector<llvm::Value*> values; 

        llvm::FunctionType* ft;
        llvm::Function* f;
    };
} // namespace lox



#endif
