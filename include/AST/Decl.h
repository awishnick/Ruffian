#pragma once

#include "AST/Stmt.h"
#include "Lex/Token.h"
#include "Support/iterator_range.h"
#include <memory>
#include <vector>

class Expr;

class Decl : public Stmt {
public:
  Decl();
  virtual ~Decl();
};

class VariableDecl : public Decl {
public:
  VariableDecl(Token name,
               Token type,
               std::unique_ptr<Expr> initializer = nullptr);
  virtual ~VariableDecl();

  const Token& GetName() const { return name_; }
  const Token& GetType() const { return type_; }
  const Expr* GetInitializer() const { return initializer_.get(); }
  Expr* GetInitializer() { return initializer_.get(); }
private:
  Token name_;
  Token type_;
  std::unique_ptr<Expr> initializer_;
};

class FunctionDecl : public Decl {
public:
  // Declares a function with a return type.
  FunctionDecl(Token name,
               Token return_type,
               std::vector<std::unique_ptr<VariableDecl>> args,
               std::unique_ptr<BlockStmt> body);
  // Declares a function that does not return a value.
  FunctionDecl(Token name,
               std::vector<std::unique_ptr<VariableDecl>> args,
               std::unique_ptr<BlockStmt> body);
  virtual ~FunctionDecl();

  const Token& GetName() const { return name_; }
  const Token* GetReturnType() const { return return_type_.get(); }
  BlockStmt* GetBody() const { return body_.get(); }

  typedef std::vector<std::unique_ptr<VariableDecl>> args_container_type;
  typedef args_container_type::iterator args_iterator;
  typedef args_container_type::const_iterator const_args_iterator;
  
  auto args_range() -> iterator_range<args_iterator> {
    return make_iterator_range(args_.begin(), args_.end());
  }
  auto args_range() const -> iterator_range<const_args_iterator> {
    return make_iterator_range(args_.begin(), args_.end());
  }

private:
  Token name_;
  std::unique_ptr<Token> return_type_;
  std::vector<std::unique_ptr<VariableDecl>> args_;
  std::unique_ptr<BlockStmt> body_;
};


