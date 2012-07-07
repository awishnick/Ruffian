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

BinaryOpExpr::BinaryOpExpr(Token op, unique_ptr<Expr> left,
                           unique_ptr<Expr> right)
  : op_(op)
  , left_(move(left))
  , right_(move(right))
{
}

BinaryOpExpr::~BinaryOpExpr() {}
