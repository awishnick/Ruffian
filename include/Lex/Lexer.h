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

  Token GetCurToken() const;
  Token ConsumeCurToken();

private:
  SourceManager& sm_;
  Token curtok_;

  std::vector<std::unique_ptr<std::string>> string_pool_;
};

