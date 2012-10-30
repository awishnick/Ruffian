#include "Lex/Lexer.h"
#include "Support/make_unique.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include <iterator>
#include <istream>
#include <locale>
#include <string>
#include <utility>
#include <iostream>
#include "SourceManager.h"
using namespace std;

static bool is_char_ident_or_kw(char c) {
  if (isalnum(c)) return true;
  if (c == '_') return true;
  return false;
}

static Token GetIdentifierOrKeyword(SourceManager& sm,
                                    vector<unique_ptr<string>>& string_pool) {

  auto& input = sm.GetStream();

  // First read this character that we just peeked, since we
  // know it's ident or kw
  assert(isalpha(input.peek()));
  auto ident_or_kw = make_unique<string>();
  auto source_loc = sm.GetSourceLocationInStream();
  ident_or_kw->push_back(input.get());

  // Now read until we hit a character that's not ident or kw
  while (is_char_ident_or_kw(input.peek())) {
    ident_or_kw->push_back(input.get());
  }

  // Check for keywords.
  if (!ident_or_kw->compare("func")) return Token::Create(Token::kw_func,
                                                          source_loc);
  if (!ident_or_kw->compare("var")) return Token::Create(Token::kw_var,
                                                         source_loc);
  if (!ident_or_kw->compare("return")) return Token::Create(Token::kw_return,
                                                            source_loc);

  // If we got here, it's not a keyword, so it must be an identifier
  Token tok;
  tok.SetLocation(source_loc);
  tok.SetIdentifier(*ident_or_kw);
  string_pool.push_back(move(ident_or_kw));
  return tok;
}

static bool is_char_numeric_literal(char c) {
  return isdigit(c);
}

static Token GetNumericLiteral(SourceManager& sm) {

  // First read this character that we just peeked, since we know
  // it's a numeric literal.
  auto& input = sm.GetStream();
  auto source_loc = sm.GetSourceLocationInStream();
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
  tok.SetLocation(source_loc);
  tok.SetIntLiteral(int_literal);
  return tok;
}

// Skips comments, returning true if any were found
static bool SkipComments(istream& input) {
  // Skip single-line comments.
  if (input.peek() == '/') {
    (void)input.get();
    if (input.peek() == '/') {
      input.ignore(numeric_limits<streamsize>::max(), '\n');
      SkipComments(input);
      return true;
    } else {
      // Oops, this wasn't a comment! Rewind.
      input.unget();
    }
  }

  return false;
}

// Skips whitespace, returning true if any was found.
static bool SkipWhitespace(istream& input) {
  if (!isspace(input.peek())) return false;
  input >> ws;
  return true;
}

// Ignores input that will not be tokenized, like whitespace and comments.
static void SkipToNextToken(istream& input) {
  for (;;) {
    if (!SkipWhitespace(input) && !SkipComments(input)) break;
  }
}

static Token GetNextToken(SourceManager& sm,
                          vector<unique_ptr<string>>& string_pool) {
  auto& input = sm.GetStream();

  SkipToNextToken(input);

  // Check for EOF
  if (input.eof()) return Token::Create(Token::eof,
                                        sm.GetSourceLocationInStream());

  // Check for a keyword or identifier. They both start
  // with an alphabetic character.
  if (isalpha(input.peek())) {
    return GetIdentifierOrKeyword(sm, string_pool);
  }

  // Check for an numeric literal. They start with a numeric character.
  if (isdigit(input.peek())) {
    return GetNumericLiteral(sm);
  }

  auto source_loc = sm.GetSourceLocationInStream();
  switch (input.get()) {
    case '(': return Token::Create(Token::lparen, source_loc);
    case ')': return Token::Create(Token::rparen, source_loc);
    case '{': return Token::Create(Token::lbrace, source_loc);
    case '}': return Token::Create(Token::rbrace, source_loc);
    case '[': return Token::Create(Token::lbracket, source_loc);
    case ']': return Token::Create(Token::rbracket, source_loc);
    case ';': return Token::Create(Token::semicolon, source_loc);
    case ',': return Token::Create(Token::comma, source_loc);
    case '=': return Token::Create(Token::assign, source_loc);
    case '+': return Token::Create(Token::plus, source_loc);
    case '-':
      if (input.peek() == '>') {
        (void)input.get();
        return Token::Create(Token::arrow, source_loc);
      } else {
        return Token::Create(Token::minus, source_loc);
      }
    case '*': return Token::Create(Token::star, source_loc);
    case '/': return Token::Create(Token::slash, source_loc);
    default: break;
  }

  // If we get here, it's an unknown token.
  return Token::Create(Token::unknown, source_loc);
}

Lexer::Lexer(SourceManager& sm)
  : sm_(sm)
  , no_tok_(false)
{
  sm_.GetStream() >> noskipws;

  curtok_ = GetNextToken(sm_, string_pool_);
}

Token Lexer::GetCurToken() {
  getTokenIfNecessary();
  return curtok_;
}

Token Lexer::ConsumeCurToken() {
  getTokenIfNecessary();
  // Rather than immediately getting the next token, just clear
  // out the current way, and lazily get it when it's needed. This
  // makes the REPL work better, since a full declaration can finish
  // being parsed without having to wait for another token to be input.
  const auto curtok = curtok_;
  no_tok_  = true;
  return curtok;
}

void Lexer::getTokenIfNecessary() {
  if (!no_tok_) return;
  curtok_ = GetNextToken(sm_, string_pool_);
  no_tok_ = false;
}

Lexer::~Lexer() {}

