#include "AST/Module.h"
#include "AST/Decl.h"
#include <utility>

Module::Module() {
}

void Module::AddDecl(std::unique_ptr<Decl> decl) {
  decls_.push_back(std::move(decl));
}

Module::~Module() {
}

