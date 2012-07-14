#include "Lex/Lexer.h"
#include "Lex/Token.h"
#include "SourceManager.h"
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
    SourceManager sm;
    sm.SetMainFileFromString(expected.first);
    Lexer lex(sm);
    EXPECT_EQ(expected.second, lex.ConsumeCurToken().GetKind())
      << "Keyword is \"" << expected.first << "\"";
  }
}

TEST(Lex, KeywordsSeparatedByWhitespace) {
  SourceManager sm;
  sm.SetMainFileFromString("\tfunc var\nvar  func");
  Lexer lex(sm);
  EXPECT_EQ(Token::kw_func, lex.ConsumeCurToken().GetKind());
  EXPECT_EQ(Token::kw_var, lex.ConsumeCurToken().GetKind());
  EXPECT_EQ(Token::kw_var, lex.ConsumeCurToken().GetKind());
  EXPECT_EQ(Token::kw_func, lex.ConsumeCurToken().GetKind());
}

TEST(Lex, ConcatenatedKeywords) {
  SourceManager sm;
  sm.SetMainFileFromString("funcvar");
  Lexer lex(sm);
  auto tok = lex.ConsumeCurToken();
  EXPECT_EQ(Token::ident, tok.GetKind());
  EXPECT_EQ("funcvar", tok.GetIdentifier());
}

TEST(Lex, WhitespaceInKeyword) {
  SourceManager sm;
  sm.SetMainFileFromString("fu nc");
  Lexer lex(sm);
  auto tok = lex.ConsumeCurToken();
  EXPECT_EQ(Token::ident, tok.GetKind());
  EXPECT_EQ("fu", tok.GetIdentifier());

  tok = lex.ConsumeCurToken();
  EXPECT_EQ(Token::ident, tok.GetKind());
  EXPECT_EQ("nc", tok.GetIdentifier());
}

