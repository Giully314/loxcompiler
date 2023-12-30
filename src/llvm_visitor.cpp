#include "llvm_visitor.hpp"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h> // Type
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>

#include <variant>
#include <utility>

#include "log.hpp"
#include "token.hpp"

namespace lox
{
    LLVMVisitor::LLVMVisitor() : 
        context{ std::make_unique<llvm::LLVMContext>() },
        mod{ std::make_unique<llvm::Module>("MyLoxCompiler", *context) },
        builder{ std::make_unique<llvm::IRBuilder<>>(*context) }
    {
        // context = std::make_unique<llvm::LLVMContext>();
        // builder = std::make_unique<llvm::IRBuilder<>>(*context);
        // mod = std::make_unique<llvm::Module>();

        using namespace llvm;
        // Create a temp anonymous function scope.
        ft = FunctionType::get(
            Type::getVoidTy(*context), false);

        f = Function::Create(ft, Function::InternalLinkage, "Anon func", mod.get()); 

        auto bb = BasicBlock::Create(*context, "entry", f);
        builder->SetInsertPoint(bb);

        auto l = ConstantFP::get(*context, APFloat{3.14f});
        auto r = ConstantFP::get(*context, APFloat{2.7f});
        builder->CreateFAdd(l, r, "addtemp");
    
    }


    auto LLVMVisitor::End()
        -> void
    {
        builder->CreateRetVoid();
        verifyFunction(*f);
    }



    auto LLVMVisitor::operator()(const GroupingNodePtr& node)
        -> void
    {
        Visit(node->expr);
    }


    auto LLVMVisitor::operator()(const BinaryExprNodePtr& node)
        -> void
    {
        Log("Binary");
        Visit(node->left);
        auto left = value;
        Visit(node->right);
        auto right = value;

        if (!left || !right)
        {
            // Error 
            Error("Left or right operand in binary expression is null.");
        }

        switch (node->op.Type())
        {
        case TokenType::Plus:   // + 
            Log("+");
            value = builder->CreateFAdd(left, right, "addtmp");
            break; 
        case TokenType::Minus:  // -
            value = builder->CreateFSub(left, right, "subtmp");
            break;
        case TokenType::Star:   // *
            value = builder->CreateFMul(left, right, "multmp");
            break;
        case TokenType::Slash:  // /
            value = builder->CreateFDiv(left, right, "divtmp");
            break;
        default:
            // log error
            break;
        }
    }


    auto LLVMVisitor::operator()(const UnaryExprNodePtr& node)
            -> void
    {
        Log("Unary");
        Visit(node->right);
        auto right = value;
        if (!right)
        {
            // Error
            Error("Right operand in unary is null.");
        }

        switch (node->op.Type())
        {
        case TokenType::Bang:   // !
            // TODO
            break;
        case TokenType::Minus:  // -
            value = builder->CreateFNeg(right, "tmpneg");
            break; 
        default:
            break;
        }
    }


    auto LLVMVisitor::operator()(const LiteralNodePtr& node)
        -> void
    {
        Visit(node->literal);
    }


    auto LLVMVisitor::operator()(const AssignExprNodePtr& node)
            -> void
    {
        // Check if the name is a variable. Otherwise throw exception.
    }


    auto LLVMVisitor::operator()(const VarExprNodePtr& node)
            -> void
    {
        // variables.emplace(
        //     std::make_pair(std::string{node->name.Lexeme})
        //     );
    }


    auto LLVMVisitor::operator()(const LogicalExprNodePtr& node)
            -> void
    {

    }



    // ************************* VISIT LITERAL ***********************

    auto LLVMVisitor::operator()(const LoxNil& value)
        -> void
    {
        // I don't know if this is correct.
        // value = llvm::ConstantPointerNull::get(llvm::PointerType::get())
    }
        
    auto LLVMVisitor::operator()(const std::string& value)
        -> void
    {

    }

    auto LLVMVisitor::operator()(const f64& value)
        -> void
    {
        this->value = llvm::ConstantFP::get(*context, llvm::APFloat{value});
    }

    auto LLVMVisitor::operator()(const bool& value)
        -> void
    {
        // this->value = llvm::Con
    }

    // ************************* VISIT LITERAL ***********************

} // namespace lox
