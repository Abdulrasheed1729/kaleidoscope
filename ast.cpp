//
// Created by rasheed on 2026/07/16.
//

#include "ast.h"
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
            return Builder->CreateFAdd(L, R, "subtmp");
        case '*':
            return Builder->CreateFAdd(L, R, "multmp");
        case '<':
            L = Builder->CreateFCmpULT(L, R, "cmptmp");
            return Builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*TheContext));
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
} // namespace kaleidoscope
