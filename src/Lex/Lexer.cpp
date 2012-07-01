#include "Lex/Lexer.h"
#include "Support/make_unique.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include <iterator>
#include <locale>
#include <sstream>
#include <string>
#include <utility>
using namespace std;

static bool is_char_ident_or_kw(char c) {
  if (isalnum(c)) return true;
  if (c == '_') return true;
  return false;
}

static Token GetIdentifierOrKeyword(stringstream& input,
                                    vector<unique_ptr<string>>& string_pool) {

  // First read this character that we just peeked, since we
  // know it's ident or kw
  assert(isalpha(input.peek()));
  auto ident_or_kw = make_unique<string>();
  ident_or_kw->push_back(input.get());

  // Now read until we hit a character that's not ident or kw
  while (is_char_ident_or_kw(input.peek())) {
    ident_or_kw->push_back(input.get());
  }

  // Check for keywords.
  if (!ident_or_kw->compare("func")) return Token::Create(Token::kw_func);
  if (!ident_or_kw->compare("var")) return Token::Create(Token::kw_var);

  // If we got here, it's not a keyword, so it must be an identifier
  Token tok;
  tok.SetIdentifier(*ident_or_kw);
  string_pool.push_back(move(ident_or_kw));
  return tok;
}

static bool is_char_numeric_literal(char c) {
  return isdigit(c);
}

static Token GetNumericLiteral(stringstream& input) {

  // First read this character that we just peeked, since we know
  // it's a numeric literal.
  assert(is_char_numeric_literal(input.peek()));
  llvm::SmallString<16> str;
  str.push_back(input.get());

  while (is_char_numeric_literal(input.peek())) {
    str.push_back(input.get());
  }

  // Make an APInt from the string. APInt wants to know how many
  // bits it should be -- we'll estimate this from the number of
  // digits. Since 9 = 0b1001, consider each decimal digit to be
  // 4 bits. We'll over-estimate the number of bits this way.
  llvm::APInt int_literal(/*numBits=*/4*str.size(),
                          str, /*radix=*/10);
  Token tok;
  tok.SetIntLiteral(int_literal);
  return tok;
}

static Token GetNextToken(stringstream& input,
                          vector<unique_ptr<string>>& string_pool) {
  input >> ws;

  // Check for EOF
  if (input.eof()) return Token::Create(Token::eof);

  // Check for a keyword or identifier. They both start
  // with an alphabetic character.
  if (isalpha(input.peek())) {
    return GetIdentifierOrKeyword(input, string_pool);
  }

  // Check for an numeric literal. They start with a numeric character.
  if (isdigit(input.peek())) {
    return GetNumericLiteral(input);
  }

  if (input.peek() == '-') {
    (void)input.get();
    return Token::Create(Token::minus);
  }

  // If we get here, it's an unknown token.
  Token tok;
  tok.SetKind(Token::unknown);
  return tok;
}

Lexer::Lexer(const string& inputbuf)
  : input_(inputbuf)
{
  input_ >> noskipws;

  curtok_ = GetNextToken(input_, string_pool_);
}

Token Lexer::GetCurToken() const {
  return curtok_;
}

Token Lexer::ConsumeCurToken() {
  const auto curtok = curtok_;
  curtok_ = GetNextToken(input_, string_pool_);
  return curtok;
}


Lexer::~Lexer() {}

