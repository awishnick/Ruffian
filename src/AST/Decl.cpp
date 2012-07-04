#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "Lex/Token.h"
#include <memory>
#include <utility>
#include <vector>
using namespace std;

Decl::Decl() {}
Decl::~Decl() {}

VariableDecl::VariableDecl(Token name,
                           Token type,
                           unique_ptr<Expr> initializer)
  : name_(name)
  , type_(type)
  , initializer_(move(initializer))
{
}

VariableDecl::~VariableDecl() {}

FunctionDecl::FunctionDecl(Token name,
                           Token return_type,
                           vector<unique_ptr<VariableDecl>> args,
                           unique_ptr<BlockStmt> body)
  : name_(name)
  , return_type_(new Token(return_type))
  , args_(move(args))
  , body_(move(body))
{
}

FunctionDecl::FunctionDecl(Token name,
                           vector<unique_ptr<VariableDecl>> args,
                           unique_ptr<BlockStmt> body)
  : name_(name)
  , args_(move(args))
  , body_(move(body))
{
}

FunctionDecl::~FunctionDecl() {}

