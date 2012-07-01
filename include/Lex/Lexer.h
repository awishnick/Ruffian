#pragma once

#include "Lex/Token.h"
#include <memory>
#include <string>
#include <sstream>
#include <vector>

class Lexer {
public:
  Lexer(const std::string& inputbuf);
  ~Lexer();

  Token GetCurToken() const;
  Token ConsumeCurToken();

private:
  std::stringstream input_;
  Token curtok_;

  std::vector<std::unique_ptr<std::string>> string_pool_;
};

