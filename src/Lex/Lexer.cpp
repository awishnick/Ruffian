#include "Lex/Lexer.h"
#include "Support/make_unique.h"
#include "llvm/ADT/StringRef.h"
#include <iterator>
#include <locale>
#include <sstream>
#include <string>
#include <utility>
using namespace std;

static bool is_ident_or_kw(char c) {
  if (isalnum(c)) return true;
  if (c == '_') return true;
  return false;
}

static Token GetNextToken(stringstream& input,
                          vector<unique_ptr<string>>& string_pool) {
  input >> ws;

  // Check for a keyword or identifier. They both start
  // with an alphabetic character.
  if (isalpha(input.peek())) {
    // First read this character that we just peeked, since we
    // know it's ident or kw
    auto ident_or_kw = make_unique<string>();
    ident_or_kw->push_back(input.get());

    // Now read until we hit a character that's not ident or kw
    while (is_ident_or_kw(input.peek())) {
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

  // If we get here, it's an unknown token
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

