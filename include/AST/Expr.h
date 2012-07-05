#pragma once

#include "Lex/Token.h"

class Expr {
public:
  virtual ~Expr();
private:
};

class NumericLiteral : public Expr {
public:
  explicit NumericLiteral(Token value);
  virtual ~NumericLiteral();
  
  const Token& GetValue() const { return value_; }
private:
  Token value_;
};

