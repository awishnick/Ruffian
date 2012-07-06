#include "AST/Expr.h"

Expr::~Expr() {}

IdentifierExpr::IdentifierExpr(Token ident)
  : ident_(ident)
{
}

IdentifierExpr::~IdentifierExpr() {}

NumericLiteral::NumericLiteral(Token value)
  : value_(value)
{
}

NumericLiteral::~NumericLiteral() {}
