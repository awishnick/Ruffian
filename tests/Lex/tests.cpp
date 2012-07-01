#include "gtest/gtest.h"
#include "Lex/Lexer.h"
#include "Lex/Token.h"
#include <sstream>

TEST(Lex, SingleTokens) {
  std::stringstream sstr("func");
  Lexer lex(sstr);
  auto tok = lex.ConsumeCurToken();
  EXPECT_EQ(Token::kw_func, tok.GetKind());
}

