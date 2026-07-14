//
// Created by rasheed on 2026/07/14.
//

#include <memory>

#include "parser.h"
#include "token.h"

#include <map>

namespace kaleidoscope {

int getNextTok() { return CurTok = get_tok(); }

std::unique_ptr<ExprAST> LogError(const char *Str) {
  std::fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}

std::unique_ptr<ProtoTypeAST> LogErrorP(const char *Str) {
  LogError(Str);
  return nullptr;
}

std::unique_ptr<ExprAST> ParseNumber() {
  auto Result = std::make_unique<kaleidoscope::NumberExprAST>(NumVal);
  getNextTok();
  return std::move(Result);
}

std::unique_ptr<ExprAST> ParseBinOpRHS(int ExpPrec,
                                       std::unique_ptr<ExprAST> LHS) {
  while (true) {
    int TokPrec = GetTokenPrecedence();
    if (TokPrec < ExpPrec)
      return LHS;

    int BinOp = CurTok;
    getNextTok();

    auto RHS = ParsePrimary();
    if (!RHS)
      return nullptr;

    int NextPrec = GetTokenPrecedence();
    if (TokPrec < NextPrec) {
      RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
      if (!RHS)
        return nullptr;
    }

    LHS =
        std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
  }
}

static std::unique_ptr<ExprAST> ParseExpression() {
  auto LHS = ParsePrimary();
  if (!LHS)
    return nullptr;

  return ParseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<ExprAST> ParseParenExpr() {
  getNextTok();
  auto V = ParseExpression();
  if (!V)
    return nullptr;

  if (CurTok != ')')
    return LogError("expected ')'");
  getNextTok();
  return V;
}

std::unique_ptr<ExprAST> ParseIdentifierExpr() {
  std::string IdName = IdentifierStr;

  getNextTok();
  if (CurTok != '(')
    return std::make_unique<VariableExprAST>(IdName);

  getNextTok();
  std::vector<std::unique_ptr<ExprAST>> Args;
  if (CurTok != '(') {
    while (true) {
      if (auto Arg = ParseExpression()) {
        Args.push_back(std::move(Arg));
      } else {
        return nullptr;
      }

      if (CurTok == ')')
        break;
      if (CurTok != ',')
        return LogError("Expected ')' or ',' in argument list");
      getNextTok();
    }
  }
  getNextTok();
  return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

std::unique_ptr<ExprAST> ParsePrimary() {
  switch (CurTok) {
  default:
    return LogError("unknown token when expecting an expression");
  case tok_identifier:
    return ParseIdentifierExpr();
  case tok_number:
    return ParseNumber();
  case '(':
    return ParseParenExpr();
  }
}

std::unique_ptr<ProtoTypeAST> ParsePrototype() {
  if (CurTok != tok_identifier)
    return LogErrorP("Expected function name in prototype");

  std::string FnName = IdentifierStr;
  getNextTok();
  if (CurTok != '(')
    return LogErrorP("Expected '(' in prototype");

  std::vector<std::string> ArgNames;
  while (getNextTok() == tok_identifier) {
    ArgNames.push_back(IdentifierStr);
  }
  if (CurTok != ')')
    return LogErrorP("Expected ')' in prototype");

  getNextTok();
  return std::make_unique<ProtoTypeAST>(FnName, std::move(ArgNames));
}

std::unique_ptr<FunctionAST> ParseDefinition() {
  getNextTok();
  auto Proto = ParsePrototype();
  if (!Proto)
    return nullptr;

  if (auto E = ParseExpression()) {
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }

  return nullptr;
}

std::unique_ptr<ProtoTypeAST> ParseExtern() {
  getNextTok();
  return ParsePrototype();
}

std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
  if (auto E = ParseExpression()) {
    auto Proto = std::make_unique<ProtoTypeAST>("__anon_expr",
                                                std::vector<std::string>());
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }

  return nullptr;
}

void MainLoop() {
  while (true) {
    std::fprintf(stderr, "ready> ");
    switch (CurTok) {
    case tok_eof:
      return;
    case ';':
      getNextTok();
      break;
    case tok_def:
      HandleDefinition();
      break;
    case tok_extern:
      HandleExtern();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}

int GetTokenPrecedence() {
  if (!isascii(CurTok))
    return -1;

  int TokPrec = BinOpPrecedence[CurTok];
  if (TokPrec <= 0)
    return -1;
  return TokPrec;
}
void HandleDefinition() {
  if (ParseDefinition()) {
    std::fprintf(stderr, "Parsed a function definition\n");
  } else {
    getNextTok();
  }
}

void HandleExtern() {
  if (ParseExtern()) {
    std::fprintf(stderr, "Parsed an extern\n");
  } else {
    getNextTok();
  }
}

void HandleTopLevelExpression() {
  if (ParseTopLevelExpr()) {
    std::fprintf(stderr, "Parsed a top-level expression\n");
  } else {
    getNextTok();
  }
}

} // namespace kaleidoscope