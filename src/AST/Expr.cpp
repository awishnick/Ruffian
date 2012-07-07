#include "AST/Expr.h"
#include <memory>
#include <utility>
using namespace std;

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

UnaryOpExpr::UnaryOpExpr(Token op, unique_ptr<Expr> expr)
  : op_(op)
  , expr_(move(expr))
{
}

UnaryOpExpr::~UnaryOpExpr() {}

