#include "Lex/Token.h"

Token::Kind Token::GetKind() const {
  return kind_;
}

void Token::SetKind(Kind kind) {
  kind_ = kind;
}

