#include <iostream>

#include "parser.h"

#include  <llvm/IR/IRBuilder.h>
#include  <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>


std::map<char, int> BinOpPrecedence;
std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string, llvm::Value*> NamedValues;

int main()
{
    BinOpPrecedence['<'] = 10;
    BinOpPrecedence['+'] = 20;
    BinOpPrecedence['-'] = 30;
    BinOpPrecedence['*'] = 40;

    std::fprintf(stderr, "ready> ");
    kaleidoscope::getNextTok();

    kaleidoscope::MainLoop();
    return 0;
}
