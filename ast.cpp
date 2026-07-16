//
// Created by rasheed on 2026/07/16.
//

#include <memory>
#include "ast.h"

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

    llvm::Value* NumberExprAST::codegen()
    {
        return llvm::ConstantFP::get(*TheContext, llvm::APFloat(Val));
    }

    llvm::Value* VariableExprAST::codegen()
    {
        llvm::Value* V = NamedValues[Name];
        if (!V) return LogErrorV("Unknown named variable");
        return V;
    }
}
