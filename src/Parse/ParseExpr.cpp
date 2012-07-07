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
      | ( expression )
      | unary_op_expression
  */

  if (lex_.GetCurToken().GetKind() == Token::ident) {
    return parseIdentifierExpr();
  }

  if (lex_.GetCurToken().GetKind() == Token::int_literal) {
    return parseNumericLiteral();
  }

  if (lex_.GetCurToken().GetKind() == Token::lparen) {
    return parseParenExpr();
  }

  if (lex_.GetCurToken().GetKind() == Token::minus) {
    return parseUnaryOpExpr();
  }

  diagnose(diag::expected_primary_expression);
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

unique_ptr<Expr> Parser::parseParenExpr() {
  /* paren_expression
      : ( expression )
  */

  // Expect '('
  if (!expectAndConsume(Token::lparen)) {
    llvm_unreachable("Expected a '('");
  }

  auto expr = parseExpr();

  // Expect ')'
  if (!expectAndConsume(Token::rparen)) {
    diagnose(diag::expected_rparen_after_expression);
    // Recover by pretending the rparen was found
  }

  return expr;
}

unique_ptr<UnaryOpExpr> Parser::parseUnaryOpExpr() {
  /* unary_op_expression
      : - primary_expression
  */

  // Expect a unary operator
  if (lex_.GetCurToken().GetKind() != Token::minus) {
    llvm_unreachable("Expected a unary operator");
  }
  Token unary_op = lex_.ConsumeCurToken();

  // Parse the primary expression
  auto primary_expr = parsePrimaryExpr();
  if (!primary_expr) {
    // Don't try to recover.
    diagnose(diag::expected_primary_expression_after_unary_op);
    return nullptr;
  }

  return make_unique<UnaryOpExpr>(unary_op, move(primary_expr));
}
