#pragma once
#include <iosfwd>

#include "Lex/Token.h"

class Lexer {
public:
  Lexer(std::istream& input);
  ~Lexer();

  Token GetCurToken() const;
  Token ConsumeCurToken();

private:
  std::istream& input_;
  Token cur_tok_;
};

