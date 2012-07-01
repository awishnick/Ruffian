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
  return ident_;
}

void Token::SetIdentifier(llvm::StringRef ident) {
  kind_ = Token::ident;
  ident_ = ident;
}

