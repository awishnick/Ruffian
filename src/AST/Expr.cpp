#include "AST/Expr.h"

Expr::~Expr() {}

NumericLiteral::NumericLiteral(Token value)
  : value_(value)
{
}

NumericLiteral::~NumericLiteral() {}
