#pragma once

class Token {
public:
  enum Kind {
    kw_func,
    kw_var,
    kw_identifier,
    lparen,
    rparen,
    lbrace,
    rbrace,
  };

  Kind GetKind() const;
  void SetKind(Kind kind);
private:
  Kind kind_;

};

