#include "Lex/Lexer.h"
#include "Lex/Token.h"
#include "SourceManager.h"
#include <sstream>
#include <string>
#include <utility>
#include "gtest/gtest.h"
using namespace std;

TEST(Lex, IntegerLiterals) {
  int expected_ints[] = { 1, -500, 92, 84, -109234, 0 };

  // Build a string representation of the numbers, separated by spaces
  stringstream sstr;
  for (int n : expected_ints) {
    sstr << n;
    sstr << ' ';
  }

  SourceManager sm;
  sm.SetMainFileFromStream(sstr);
  Lexer lex(sm);
  for (int n : expected_ints) {
    auto tok = lex.ConsumeCurToken();

    // Negative numbers have the leading minus sign as a separate token.
    if (n < 0) {
      EXPECT_EQ(Token::minus, tok.GetKind());
      tok = lex.ConsumeCurToken();
      n = -n;
    }

    EXPECT_EQ(Token::int_literal, tok.GetKind());
    EXPECT_EQ(n, tok.GetIntLiteral());
  }
}
