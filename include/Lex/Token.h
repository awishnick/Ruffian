#pragma once

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringRef.h"
#include "SourceLocation.h"

class Token {
public:
  enum Kind {
    kw_func,
    kw_var,
    kw_return,

    ident,

    int_literal,
    
    lparen,
    rparen,
    lbrace,
    rbrace,
    lbracket,
    rbracket,
    semicolon,
    comma,

    assign,

    minus,
    plus,
    star,
    slash,
    arrow,

    eof,
    unknown
  };

  static Token Create(Kind kind, SourceLocation loc);

  Kind GetKind() const;
  void SetKind(Kind kind);

  SourceLocation GetLocation() const;
  void SetLocation(SourceLocation loc);

  // If this token is an identifier, returns it as a string.
  // Do not call this if this isn't an identifier.
  llvm::StringRef GetIdentifier() const;
  // Makes this token have identifier kind, using the string passed in.
  void SetIdentifier(llvm::StringRef ident);

  // If this token is an integer literal, returns its value.
  // Do not call this if it isn't an integer literal.
  llvm::APInt GetIntLiteral() const;
  // Makes this token have int literal kind, using the value passe in.
  void SetIntLiteral(llvm::APInt int_literal);
private:
  Kind kind_;
  SourceLocation loc_;
  struct {
    llvm::StringRef ident;
    llvm::APInt int_literal;
  } storage_;
};

