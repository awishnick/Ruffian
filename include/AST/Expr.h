#pragma once

#include "Lex/Token.h"

class Expr {
public:
  virtual ~Expr();
private:
};

class IdentifierExpr : public Expr {
public:
  explicit IdentifierExpr(Token ident);
  virtual ~IdentifierExpr();

  const Token& GetIdentifier() const { return ident_; }
private:
  Token ident_;
};

class NumericLiteral : public Expr {
public:
  explicit NumericLiteral(Token value);
  virtual ~NumericLiteral();
  
  const Token& GetValue() const { return value_; }
private:
  Token value_;
};

