//
// Created by rasheed on 2026/07/16.
//

#include "ast.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <vector>

namespace kaleidoscope
{
    std::unique_ptr<ExprAST> LogError(const char* Str)
    {
        std::fprintf(stderr, "Error: %s\n", Str);
        return nullptr;
    }

    std::unique_ptr<ProtoTypeAST> LogErrorP(const char* Str)
    {
        LogError(Str);
        return nullptr;
    }

    llvm::Value* LogErrorV(const char* Str)
    {
        LogError(Str);
        return nullptr;
    }

    llvm::Value* NumberExprAST::codegen() { return llvm::ConstantFP::get(*TheContext, llvm::APFloat(Val)); }

    llvm::Value* VariableExprAST::codegen()
    {
        llvm::Value* V = NamedValues[Name];
        if (!V)
            return LogErrorV("Unknown named variable");
        return V;
    }

    llvm::Value* BinaryExprAST::codegen()
    {
        llvm::Value* L = LHS->codegen();
        llvm::Value* R = RHS->codegen();
        if (!L || !R)
            return nullptr;

        switch (Op)
        {
        case '+':
            return Builder->CreateFAdd(L, R, "addtmp");
        case '-':
            return Builder->CreateFSub(L, R, "subtmp");
        case '*':
            return Builder->CreateFMul(L, R, "multmp");
        case '<':
            L = Builder->CreateFCmpULT(L, R, "cmptmp");
            return Builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*TheContext), "booltmp");
        default:
            return LogErrorV("Invalid binary operator");
        }
    }

    llvm::Value* CallExprAST::codegen()
    {
        llvm::Function* CalleeF = TheModule->getFunction(Callee);
        if (!CalleeF)
            return LogErrorV("uknown function referenced");

        if (CalleeF->arg_size() != Args.size())
            return LogErrorV("incorrect nunber of arguments passed");

        std::vector<llvm::Value*> ArgsV;
        for (unsigned i = 0, e = Args.size(); e != i; i++)
        {
            ArgsV.push_back(Args[i]->codegen());
            if (!Args.back())
                return nullptr;
        }


        return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
    }

    llvm::Function* ProtoTypeAST::codegen()
    {
        std::vector<llvm::Type*> Doubles(Args.size(), llvm::Type::getDoubleTy(*TheContext));
        llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*TheContext), Doubles, false);
        llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, TheModule.get());

        unsigned Idx = 0;
        for (auto& Arg : F->args())
            Arg.setName(Args[Idx++]);

        return F;
    }

    llvm::Function* FunctionAST::codegen()
    {
        llvm::Function* TheFunction = TheModule->getFunction(Proto->get_name());

        if (!TheFunction)
            TheFunction = Proto->codegen();
        if (!TheFunction)
            return nullptr;

        if (!TheFunction->empty())
            return static_cast<llvm::Function*>(LogErrorV("Function cannot be redefined."));

        llvm::BasicBlock* BB = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
        Builder->SetInsertPoint(BB);

        NamedValues.clear();
        for (auto& Arg : TheFunction->args())
            NamedValues[std::string(Arg.getName())] = &Arg;

        if (llvm::Value* RetVal = Body->codegen())
        {
            Builder->CreateRet(RetVal);
            llvm::verifyFunction(*TheFunction);

            return TheFunction;
        }

        TheFunction->eraseFromParent();
        return nullptr;
    }
} // namespace kaleidoscope
