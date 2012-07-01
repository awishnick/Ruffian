#include "Lex/Lexer.h"
#include "Lex/Token.h"
#include "llvm/ADT/StringRef.h"
#include <string>
#include <utility>
#include "gtest/gtest.h"
using namespace std;

TEST(Lex, SingleKeywordTokens) {
  pair<const char*, Token::Kind> expected_pairs[] = {
    make_pair("func", Token::kw_func),
    make_pair("var", Token::kw_var)
  };

  for (auto expected : expected_pairs) {
    Lexer lex(expected.first);
    EXPECT_EQ(expected.second, lex.ConsumeCurToken().GetKind())
      << "Keyword is \"" << expected.first << "\"";
  }
}

TEST(Lex, KeywordsSeparatedByWhitespace) {
  Lexer lex("\tfunc var\nvar  func");
  EXPECT_EQ(Token::kw_func, lex.ConsumeCurToken().GetKind());
  EXPECT_EQ(Token::kw_var, lex.ConsumeCurToken().GetKind());
  EXPECT_EQ(Token::kw_var, lex.ConsumeCurToken().GetKind());
  EXPECT_EQ(Token::kw_func, lex.ConsumeCurToken().GetKind());
}

TEST(Lex, ConcatenatedKeywords) {
  Lexer lex("funcvar");
  auto tok = lex.ConsumeCurToken();
  EXPECT_EQ(Token::ident, tok.GetKind());
  EXPECT_EQ("funcvar", tok.GetIdentifier());
}

TEST(Lex, WhitespaceInKeyword) {
  Lexer lex("fu nc");
  auto tok = lex.ConsumeCurToken();
  EXPECT_EQ(Token::ident, tok.GetKind());
  EXPECT_EQ("fu", tok.GetIdentifier());

  tok = lex.ConsumeCurToken();
  EXPECT_EQ(Token::ident, tok.GetKind());
  EXPECT_EQ("nc", tok.GetIdentifier());
}

