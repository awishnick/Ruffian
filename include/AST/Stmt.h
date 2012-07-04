#pragma once

#include "Support/iterator_range.h"
#include <memory>
#include <vector>

class Stmt {
public:
  virtual ~Stmt();
};

class BlockStmt : public Stmt {
public:
  explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts);
  virtual ~BlockStmt();

  typedef std::vector<std::unique_ptr<Stmt>> stmt_container_type;
  typedef stmt_container_type::iterator stmt_iterator;
  typedef stmt_container_type::const_iterator const_stmt_iterator;

  auto stmts_range() -> iterator_range<stmt_iterator> {
    return make_iterator_range(stmts_.begin(), stmts_.end());
  }
  auto stmts_range() const -> iterator_range<const_stmt_iterator> {
    return make_iterator_range(stmts_.begin(), stmts_.end());
  }
private:
  std::vector<std::unique_ptr<Stmt>> stmts_;
  
};

