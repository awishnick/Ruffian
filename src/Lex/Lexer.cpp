#include "Lex/Lexer.h"
#include <istream>

Lexer::Lexer(std::istream& input)
  : input_(input)
{
}

Token Lexer::GetCurToken() const {
  return cur_tok_;
}

Token Lexer::ConsumeCurToken() {
  return cur_tok_;
}


Lexer::~Lexer() {}

