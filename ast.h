//
// Created by rasheed on 2026/07/14.
//

#ifndef KALEIDOSCOPE_AST_H
#define KALEIDOSCOPE_AST_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <llvm/IR/BasicBlock.h>
#include  <llvm/IR/IRBuilder.h>
#include  <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::unique_ptr<llvm::IRBuilder<>> TheBuilder;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::map<std::string, llvm::Value*> NamedValues;

namespace kaleidoscope
{
    class ProtoTypeAST;
    class ExprAST;

    std::unique_ptr<ExprAST> LogError(const char* Str);

    std::unique_ptr<ProtoTypeAST> LogErrorP(const char* Str);

    llvm::Value* LogErrorV(const char* Str);

    class ExprAST
    {
    public:
        virtual ~ExprAST() = default;
        virtual llvm::Value* codegen() = 0;
    };

    class NumberExprAST : public ExprAST
    {
        double Val;

    public:
        explicit NumberExprAST(const double Val) : Val(Val)
        {
        }

        llvm::Value* codegen() override;
    };

    class VariableExprAST : public ExprAST
    {
        std::string Name;

    public:
        explicit VariableExprAST(const std::string& Name) : Name(Name)
        {
        }

        llvm::Value* codegen() override;
    };

    class BinaryExprAST : public ExprAST
    {
        char Op;
        std::unique_ptr<ExprAST> LHS, RHS;

    public:
        explicit BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                               std::unique_ptr<ExprAST> RHS)
            : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS))
        {
        }

        llvm::Value* codegen() override;
    };

    class CallExprAST : public ExprAST
    {
        std::string Callee;
        std::vector<std::unique_ptr<ExprAST>> Args;

    public:
        CallExprAST(const std::string& Callee,
                    std::vector<std::unique_ptr<ExprAST>> Args)
            : Callee(Callee), Args(std::move(Args))
        {
        }

        llvm::Value* codegen() override;
    };

    class ProtoTypeAST
    {
        std::string Name;
        std::vector<std::string> Args;

    public:
        ProtoTypeAST(const std::string& Name, std::vector<std::string> Args)
            : Name(Name), Args(std::move(Args))
        {
        }

        [[nodiscard]] const std::string& get_name() const { return Name; }
    };

    class FunctionAST
    {
        std::unique_ptr<ProtoTypeAST> Proto;
        std::unique_ptr<ExprAST> Body;

    public:
        FunctionAST(std::unique_ptr<ProtoTypeAST> Proto,
                    std::unique_ptr<ExprAST> Body)
            : Proto(std::move(Proto)), Body(std::move(Body))
        {
        }
    };
} // namespace kaleidoscope

#endif
