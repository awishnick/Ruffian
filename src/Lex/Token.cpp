#include "Lex/Token.h"
#include "llvm/ADT/StringRef.h"
#include <type_traits>

Token Token::Create(Kind kind) {
  Token tok;
  tok.SetKind(kind);
  return tok;
}

Token::Kind Token::GetKind() const {
  return kind_;
}

void Token::SetKind(Kind kind) {
  kind_ = kind;
}

llvm::StringRef Token::GetIdentifier() const {
  assert(GetKind() == ident);
  return storage_.ident;
}

void Token::SetIdentifier(llvm::StringRef ident) {
  kind_ = Token::ident;
  storage_.ident = ident;
}

llvm::APInt Token::GetIntLiteral() const {
  assert(GetKind() == int_literal);
  return storage_.int_literal;
}

void Token::SetIntLiteral(llvm::APInt int_literal) {
  kind_ = Token::int_literal;
  storage_.int_literal = int_literal;
}

