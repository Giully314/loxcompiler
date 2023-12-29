#include "llvm_visitor.hpp"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h> // Type
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>

#include <variant>

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


    auto LLVMVisitor::Visit(const ExprNode& node)
            -> void
    {
        constexpr auto size = std::variant_size_v<std::remove_cvref_t<decltype(node)>>;
        switch (node.index())
        {
        case 0: // BinaryExprNodePtr
            if constexpr (0 < size)
            {
                (*this)(*std::get_if<0>(&node));
            }
            break;
        case 1: // UnaryExprNodePtr
            if constexpr (1 < size)
            {
                (*this)(*std::get_if<1>(&node));
            }
            break; 
            
        case 2: // LiteralNodePtr
            if constexpr (2 < size)
            {
                (*this)(*std::get_if<2>(&node));
            }
            break;
        
        case 3: // GroupingNodePtr
            if constexpr (3 < size)
            {
                (*this)(*std::get_if<3>(&node));
            }
            break;

        default:
            break;
        }
    }


    auto LLVMVisitor::operator()(const GroupingNodePtr& node)
        -> void
    {
        Visit(node->expr);
    }


    auto LLVMVisitor::operator()(const UnaryExprNodePtr& node)
            -> void
    {
        Log("Unary");
        Visit(node->right);
        auto right = values.back();
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
            values.emplace_back(builder->CreateFNeg(right, "tmpneg"));
            break; 
        default:
            break;
        }
    }


    auto LLVMVisitor::operator()(const BinaryExprNodePtr& node)
        -> void
    {
        Log("Binary");
        Visit(node->left);
        auto left = values.back();
        Visit(node->right);
        auto right = values.back();

        if (!left || !right)
        {
            // Error 
            Error("Left or right operand in binary expression is null.");
        }

        switch (node->op.Type())
        {
        case TokenType::Plus:   // + 
            Log("+");
            values.emplace_back(builder->CreateFAdd(left, right, "addtmp"));
            break; 
        case TokenType::Minus:  // -
            values.emplace_back(builder->CreateFSub(left, right, "subtmp"));
            break;
        case TokenType::Star:   // *
            values.emplace_back(builder->CreateFMul(left, right, "multmp"));
            break;
        case TokenType::Slash:  // /
            values.emplace_back(builder->CreateFDiv(left, right, "divtmp"));
            break;
        default:
            break;
        }
    }


    auto LLVMVisitor::operator()(const LiteralNodePtr& node)
        -> void
    {
        Log("Literal");
        switch (node->literal.index())
        {
        case 0: // std::string
            // string not supported yet
            break;
        case 1: // f64
            values.emplace_back(
                llvm::ConstantFP::get(*context, 
                llvm::APFloat{*std::get_if<1>(&node->literal)}));
            // values.back()->getType()->print(llvm::outs());
            break;
            
        case 2: // bool
        case 3: // LoxNil
            // for now nil is not supported 
            break;
        default:
            // Do nothing
            break;
        }   
    }

} // namespace lox
