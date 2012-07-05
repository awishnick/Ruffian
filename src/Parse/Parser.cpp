#include "Parse/Parser.h"
#include "AST/Decl.h"
#include "Lex/Lexer.h"
#include "Lex/Token.h"
#include "llvm/Support/ErrorHandling.h"
#include <iostream>
#include <memory>
using namespace std;

Parser::Parser(Lexer& lex)
  : lex_(lex)
{
}

std::ostream& Parser::diagnose(diag::Kind kind) {
  switch (kind) {
#define DIAG(ident, str) case diag::ident: return cerr << str << endl;
#include "Diagnostics.inc"
    default:
      llvm_unreachable("Unexpected diagnostic kind");
  }

}

bool Parser::expectAndConsume(Token::Kind kind) {
  if (lex_.GetCurToken().GetKind() == kind) {
    lex_.ConsumeCurToken();
    return true;
  } else {
    return false;
  }
}

bool Parser::expectAndConsumeIdentifier(Token* tok) {
  if (lex_.GetCurToken().GetKind() == Token::ident) {
    *tok = lex_.ConsumeCurToken();
    return true;
  } else {
    return false;
  }
}

bool Parser::expectAndConsumeNumericLiteral(Token* tok) {
  if (lex_.GetCurToken().GetKind() == Token::int_literal) {
    *tok = lex_.ConsumeCurToken();
    return true;
  } else {
    return false;
  }
}

bool Parser::ignoreTokensUntil(Token::Kind kind) {
  while (lex_.GetCurToken().GetKind() != kind) {
    if (lex_.GetCurToken().GetKind() == Token::eof) {
      return false;
    }
    lex_.ConsumeCurToken();
  }
  return true;
}

Parser::~Parser() {}
