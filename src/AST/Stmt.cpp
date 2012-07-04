#include "AST/Stmt.h"
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

