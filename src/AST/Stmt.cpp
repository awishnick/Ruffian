#include "AST/Stmt.h"
#include "AST/Expr.h"
#include "Support/iterator_range.h"
#include <memory>
#include <utility>
#include <vector>
using namespace std;

Stmt::~Stmt() {}

BlockStmt::BlockStmt(vector<unique_ptr<Stmt>> stmts)
  : stmts_(move(stmts)) {
}

BlockStmt::~BlockStmt() {}

ExprStmt::ExprStmt(unique_ptr<Expr> expr)
  : expr_(move(expr))
{
}

ExprStmt::~ExprStmt() {}
