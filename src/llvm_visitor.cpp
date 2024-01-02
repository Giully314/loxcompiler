#include "llvm_visitor.hpp"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h> // Type
#include <llvm/IR/Verifier.h>
#include <llvm/ADT/StringRef.h>

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
        
    }



    auto LLVMVisitor::Generate(const StmtNode& ast)
        -> void
    {
        // Create (the implicit) main function.
        using namespace llvm;
    
        FunctionType* main_proto = FunctionType::get(
            Type::getVoidTy(*context),
            {},
            false 
        );

        Function* main_func = Function::Create(
            main_proto, 
            GlobalValue::ExternalLinkage,
            "main",
            *mod 
        );

        BasicBlock* bb = BasicBlock::Create(
            *context,
            "entry",
            main_func
        );

        builder->SetInsertPoint(bb);

        VisitStatement(ast);
    }


    auto LLVMVisitor::End()
        -> void
    {
       
    }


    // ********************************* UTILITY *********************************

    auto LLVMVisitor::ReadLocalVarRecursive(llvm::BasicBlock* bb, std::string_view name)
            -> llvm::Value* 
        {
            using namespace llvm;

            Value* value {nullptr};
            
            // Create an empty phinode
            if (!current_def[bb].sealed)
            {
                PHINode* phi = AddEmptyPhi(bb, name);
                current_def[bb].incomplete_phis[phi] = name;
                value = phi;
            }
            else if (auto pred_bb = bb->getSinglePredecessor())
            {
                value = ReadLocalVar(pred_bb, name);
            }
            else
            {
                PHINode* phi = AddEmptyPhi(bb, name);
                value = phi;
                WriteLocalVar(bb, name, value);
                AddPhiOperands(bb, name, phi);
            }

            WriteLocalVar(bb, name, value);
            return value;
        }

    // ********************************* UTILITY *********************************

    // ******************************** VISIT STATEMENTS *************************************

    auto LLVMVisitor::operator()(const ExprStmtNodePtr& node)
        -> void
    {

    }


    auto LLVMVisitor::operator()(const PrintStmtNodePtr& node)
        -> void
    {

    }


    auto LLVMVisitor::operator()(const VarStmtNodePtr& node)
        -> void
    {
        llvm::StringRef var{node->name.Lexeme()};
        
        // name_vars[var] = 
    }


    auto LLVMVisitor::operator()(const BlockStmtNodePtr& node)
        -> void
    {

    }


    auto LLVMVisitor::operator()(const FunStmtNodePtr& node)
        -> void
    {

    }


    auto LLVMVisitor::operator()(const ReturnStmtNodePtr& node)
        -> void
    {

    }


    auto LLVMVisitor::operator()(const IfStmtNodePtr& node)
        -> void
    {
        using namespace llvm;

        // Create basic blocks for the 3 block instructions of the if.
        BasicBlock* true_bb = BasicBlock::Create(*context, "if.true", current_func);
        BasicBlock* false_bb = BasicBlock::Create(*context, "if.false", current_func);
        BasicBlock* exit_bb = BasicBlock::Create(*context, "if.exit", current_func);


        builder->SetInsertPoint(current_block);
        Visit(node->condition);
        auto condition = current_value;
        builder->CreateCondBr(condition, true_bb, false_bb);
        
        SetCurrentBlock(true_bb);
        Visit(node->then_branch);
        builder->CreateBr(exit_bb);

        if (node->else_branch)
        {
            SetCurrentBlock(false_bb);
            Visit(node->then_branch);
            builder->CreateBr(exit_bb);
        }

        SetCurrentBlock(exit_bb);
    }


    
    auto LLVMVisitor::operator()(const WhileStmtNodePtr& node)
        -> void
    {
        using namespace llvm;

        // Create basic blocks for the 3 block instructions of the while.
        BasicBlock* cond_bb = BasicBlock::Create(*context, "while.cond", current_func);
        BasicBlock* body_bb = BasicBlock::Create(*context, "while.body", current_func);
        BasicBlock* exit_bb = BasicBlock::Create(*context, "while.exit", current_func);

        // Create a branch to the while conditional block. 
        builder->SetInsertPoint(current_block);
        builder->CreateBr(cond_bb);
        
        SetCurrentBlock(cond_bb);
        Visit(node->condition);
        auto condition = current_value;
        // TODO: does condition need to be truncated? What's the type?
        builder->CreateCondBr(condition, body_bb, exit_bb);

        SetCurrentBlock(body_bb);
        Visit(node->body);
        builder->CreateBr(cond_bb);
        SetCurrentBlock(exit_bb);
    }

    
    // ******************************** VISIT STATEMENTS *************************************



    // ******************************** VISIT EXPRESSIONS *************************************

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
        auto left = current_value;
        Visit(node->right);
        auto right = current_value;

        if (!left || !right)
        {
            // Error 
            Error("Left or right operand in binary expression is null.");
        }

        switch (node->op.Type())
        {
        case TokenType::Plus:   // + 
            Log("+");
            current_value = builder->CreateFAdd(left, right, "addtmp");
            break; 
        case TokenType::Minus:  // -
            current_value = builder->CreateFSub(left, right, "subtmp");
            break;
        case TokenType::Star:   // *
            current_value = builder->CreateFMul(left, right, "multmp");
            break;
        case TokenType::Slash:  // /
            current_value = builder->CreateFDiv(left, right, "divtmp");
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
        auto right = current_value;
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
            current_value = builder->CreateFNeg(right, "tmpneg");
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

    // ******************************** VISIT EXPRESSIONS *************************************


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
        // TODO
    }

    auto LLVMVisitor::operator()(const f64& value)
        -> void
    {
        current_value = llvm::ConstantFP::get(builder->getDoubleTy(), value);
    }

    auto LLVMVisitor::operator()(const bool& value)
        -> void
    {
        current_value = builder->getInt8(value ? 1 : 0);
    }

    // ************************* VISIT LITERAL ***********************

} // namespace lox
