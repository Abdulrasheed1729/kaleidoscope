//
// Created by rasheed on 2026/07/14.
//

#ifndef KALEIDOSCOPE_PARSER_H
#define KALEIDOSCOPE_PARSER_H
#include "ast.h"

#include <map>

extern std::map<char, int> BinOpPrecedence;
namespace kaleidoscope {
static int CurTok;
extern int get_tok();
extern double NumVal; // defined in lexer.cpp
extern std::string IdentifierStr;

int getNextTok();

std::unique_ptr<ExprAST> LogError(const char *Str);

std::unique_ptr<ProtoTypeAST> LogErrorP(const char *Str);

/// numberexpr ::= number
static std::unique_ptr<ExprAST> ParseNumber();
/// expression
///   ::= primary binoprhs
///
static std::unique_ptr<ExprAST> ParseExpression();

static std::unique_ptr<ExprAST> ParseParenExpr();

/// identifierexpr ::= identifier
///     | identifier '(' expression ')'
static std::unique_ptr<ExprAST> ParseIdentifierExpr();

/// primaryexpr ::= identifierexpr
///   | numberexpr
///   | parenexpr
static std::unique_ptr<ExprAST> ParsePrimary();

/// binoprhs ::= ('+' primary)*
static std::unique_ptr<ExprAST> ParseBinOpRHS(int ExpPrec,
                                              std::unique_ptr<ExprAST> LHS);

/// prototype ::= ident '(' ident* ')'
///
static std::unique_ptr<ProtoTypeAST> ParsePrototype();

/// definition ::= def prototype expression
///
static std::unique_ptr<FunctionAST> ParseDefinition();

/// external ::= 'extern' prototype
///
static std::unique_ptr<ProtoTypeAST> ParseExtern();

/// toplevelexpr ::= expression
static std::unique_ptr<FunctionAST> ParseTopLevelExpr();

/// top ::= definition | external | expression | ';'
///
void MainLoop();

static void HandleDefinition();

static void HandleExtern();

static void HandleTopLevelExpression();

static int GetTokenPrecedence();


} // namespace kaleidoscope
#endif // KALEIDOSCOPE_PARSER_H
