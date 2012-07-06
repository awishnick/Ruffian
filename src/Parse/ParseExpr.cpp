#include "Parse/Parser.h"
#include "AST/Expr.h"
#include "Lex/Lexer.h"
#include "Support/make_unique.h"
#include "llvm/Support/ErrorHandling.h"
#include <memory>
using namespace std;

unique_ptr<Expr> Parser::parseExpr() {
  return parsePrimaryExpr();
}

unique_ptr<Expr> Parser::parsePrimaryExpr() {
  /* primary_expression
      : identifier
      | numeric_literal
  */

  if (lex_.GetCurToken().GetKind() == Token::ident) {
    return parseIdentifierExpr();
  }

  if (lex_.GetCurToken().GetKind() == Token::int_literal) {
    return parseNumericLiteral();
  }

  diagnose(diag::expected_expression);
  return nullptr;

}

unique_ptr<IdentifierExpr> Parser::parseIdentifierExpr() {
  Token ident_tok;
  if (!expectAndConsumeIdentifier(&ident_tok)) {
    llvm_unreachable("Expected an identifier token");
  }

  return make_unique<IdentifierExpr>(ident_tok);
}

unique_ptr<NumericLiteral> Parser::parseNumericLiteral() {
  Token literal_tok;
  if (!expectAndConsumeNumericLiteral(&literal_tok)) {
    llvm_unreachable("Expected a numeric literal token");
  }

  return make_unique<NumericLiteral>(literal_tok);
}