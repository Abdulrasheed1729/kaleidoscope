#include <iostream>

#include "parser.h"

std::map<char, int> BinOpPrecedence;

int main() {
  BinOpPrecedence['<'] = 10;
  BinOpPrecedence['+'] = 20;
  BinOpPrecedence['-'] = 30;
  BinOpPrecedence['*'] = 40;

  std::fprintf(stderr, "ready> ");
  kaleidoscope::getNextTok();

  kaleidoscope::MainLoop();
  return 0;
}
