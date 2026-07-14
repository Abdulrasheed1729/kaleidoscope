//
// Created by rasheed on 2026/07/14.
//

#ifndef KALEIDOSCOPE_TOKEN_H
#define KALEIDOSCOPE_TOKEN_H
namespace kaleidoscope {
enum Token {
  tok_eof = -1,

  // keywords
  tok_def = -2,
  tok_extern = -3,

  // primary
  tok_identifier = -4,
  tok_number = -5,
};
}
#endif // KALEIDOSCOPE_TOKEN_H
