#pragma once

#include "Lex/Token.h"
#include <memory>
#include <string>
#include <vector>

class SourceManager;

class Lexer {
public:
  Lexer(SourceManager& sm);
  ~Lexer();

  Token GetCurToken();
  Token ConsumeCurToken();

private:
  SourceManager& sm_;
  Token curtok_;
  bool no_tok_;
  
  void getTokenIfNecessary();

  std::vector<std::unique_ptr<std::string>> string_pool_;
};

