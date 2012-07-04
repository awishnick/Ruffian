#include "Parse/Parser.h"
#include "AST/Module.h"
#include "AST/Decl.h"
#include "Lex/Lexer.h"
#include "Support/make_unique.h"
#include <iostream>
#include <memory>
using namespace std;

unique_ptr<Module> Parser::ParseModule() {
  auto module = make_unique<Module>();

  while (lex_.GetCurToken().GetKind() != Token::eof) {
    auto decl = parseTopLevelDecl();
    if (!decl) {
      return nullptr;
    }

    module->AddDecl(move(decl));
  }

  return module;
}

