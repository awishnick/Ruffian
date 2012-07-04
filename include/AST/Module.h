#pragma once

#include "Support/iterator_range.h"
#include <memory>
#include <vector>
class Decl;

class Module {
public:
  Module();
  ~Module();

  void AddDecl(std::unique_ptr<Decl> decl);

  typedef std::vector<std::unique_ptr<Decl>> decl_container;
  typedef decl_container::iterator decl_iterator;
  typedef decl_container::const_iterator const_decl_iterator;
  
  auto decls_range() -> iterator_range<decl_iterator> {
    return make_iterator_range(decls_.begin(), decls_.end());
  }

  auto decls_range() const -> iterator_range<const_decl_iterator> {
    return make_iterator_range(decls_.begin(), decls_.end());
  }
private:
  std::vector<std::unique_ptr<Decl>> decls_;
};
