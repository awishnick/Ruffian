#pragma once

#include "llvm/ADT/StringRef.h"

class Token {
public:
  enum Kind {
    kw_func,
    kw_var,
    ident,
    lparen,
    rparen,
    lbrace,
    rbrace,
    unknown
  };

  static Token Create(Kind kind);

  Kind GetKind() const;
  void SetKind(Kind kind);

  // If this token is an identifier, returns it as a string
  // Do not call this if this isn't an identifier.
  llvm::StringRef GetIdentifier() const;
  // Makes this token have identifier kind, using the string passed in.
  void SetIdentifier(llvm::StringRef ident);
private:
  Kind kind_;
  llvm::StringRef ident_;
};

