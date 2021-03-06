#pragma once

#include <memory>
#include "Lex/Token.h"
#include "Support/iterator_range.h"

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

class UnaryOpExpr : public Expr {
public:
  UnaryOpExpr(Token op, std::unique_ptr<Expr> expr);
  virtual ~UnaryOpExpr();

  const Token& GetOp() const { return op_; }
  const Expr* GetExpr() const { return expr_.get(); }
  Expr* GetExpr() { return expr_.get(); }
  
private:
  Token op_;
  std::unique_ptr<Expr> expr_;
};

class BinaryOpExpr : public Expr {
public:
  BinaryOpExpr(Token op, std::unique_ptr<Expr> left,
               std::unique_ptr<Expr> right);
  virtual ~BinaryOpExpr();

  const Token& GetOp() const { return op_; }

  const Expr* GetLeft() const { return left_.get(); }
  Expr* GetLeft() { return left_.get(); }

  const Expr* GetRight() const { return right_.get(); }
  Expr* GetRight() { return right_.get(); }
private:
  Token op_;
  std::unique_ptr<Expr> left_, right_;
};

class FunctionCall : public Expr {
public:
  FunctionCall(Token name, std::vector<std::unique_ptr<Expr>> args);
  virtual ~FunctionCall();

  const Token& GetName() const { return name_; }

  typedef std::vector<std::unique_ptr<Expr>> args_container_type;
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
  std::vector<std::unique_ptr<Expr>> args_;
};

