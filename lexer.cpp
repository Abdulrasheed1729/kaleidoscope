//
// Created by rasheed on 2026/07/14.
//
#include <string>

#include "lexer.h"
#include "token.h"

namespace kaleidoscope {

int get_tok() {
  static char LastChar = ' ';

  while (std::isspace(LastChar))
    LastChar = static_cast<char>(std::getchar());

  // identifier or keyword [a-zA-Z][a-zA-Z0-9]*
  if (std::isalpha(LastChar)) {
    IdentifierStr = LastChar;
    while (std::isalnum(LastChar = static_cast<char>(std::getchar())))
      IdentifierStr += LastChar;

    if (IdentifierStr == "def")
      return tok_def;
    if (IdentifierStr == "extern")
      return tok_extern;

    return tok_identifier;
  }

  // number [0-9.]+
  if (std::isdigit(LastChar) || LastChar == '.') {
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = static_cast<char>(std::getchar());
    } while (std::isdigit(LastChar) || LastChar == '.');

    NumVal = std::strtod(NumStr.c_str(), nullptr);

    return tok_number;
  }

  if (LastChar == '#') {
    do {
      LastChar = static_cast<char>(std::getchar());
    } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

    if (LastChar != EOF) {
      return get_tok();
    }
  }

  if (LastChar == EOF)
    return tok_eof;

  const int ThisChar = static_cast<unsigned char>(LastChar);
  LastChar = static_cast<char>(std::getchar());

  return ThisChar;
}
} // namespace kaleidoscope