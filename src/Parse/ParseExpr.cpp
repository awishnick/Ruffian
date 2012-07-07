#include "Parse/Parser.h"
#include "AST/Expr.h"
#include "Lex/Lexer.h"
#include "Support/make_unique.h"
#include "llvm/Support/ErrorHandling.h"
#include <memory>
using namespace std;

unique_ptr<Expr> Parser::parseExpr() {
  // Expressions are parsed via operator precedence parsing.
  auto primary = parsePrimaryExpr();
  if (!primary) {
    return nullptr;
  }

  return parseBinaryOpExprRHS(move(primary), 0);
}

static bool IsPrefixUnaryOpToken(const Token& tok) {
  switch (tok.GetKind()) {
  case Token::minus:
    return true;
  default:
    return false;
  };
}

static bool IsTokenABinaryOp(const Token& tok) {
  switch (tok.GetKind()) {
    case Token::assign:
    case Token::plus:
    case Token::minus:
    case Token::star:
    case Token::slash:
      return true;
    default:
      return false;
  };
}

static int GetBinaryOpPrecedence(const Token& tok) {
  switch (tok.GetKind()) {
    case Token::assign:
      return 0;
    case Token::plus:
    case Token::minus:
      return 10;
    case Token::star:
    case Token::slash:
      return 20;
    default:
      llvm_unreachable("Not a binary operator");
  };
}

static bool IsBinaryOpRightAssociative(const Token& tok) {
  assert(IsTokenABinaryOp(tok) && "Token must be a binary operator");

  switch (tok.GetKind()) {
  case Token::assign:
    return true;
  default:
    return false;
  };
}

static bool IsTokenABinaryOpWithPrecedenceAtLeast(const Token& token,
                                                  int min_precedence) {
  if (!IsTokenABinaryOp(token)) return false;
  return GetBinaryOpPrecedence(token) >= min_precedence;
}

static bool IsTokenABinaryOpWithPrecedenceGreaterThan(const Token& token,
                                                      int min_precedence) {
  if (!IsTokenABinaryOp(token)) return false;
  return GetBinaryOpPrecedence(token) > min_precedence;
}

static bool IsTokenRightAssociativeWithPrecedence(const Token& token,
                                                  int precedence) {
  if (!IsTokenABinaryOp(token)) return false;
  if (!IsBinaryOpRightAssociative(token)) return false;
  return GetBinaryOpPrecedence(token) == precedence;
}

unique_ptr<Expr> Parser::parseBinaryOpExprRHS(unique_ptr<Expr> lhs,
                                              int min_precedence) {
  // Keep parsing until we hit a binary op with lower precedence,
  // or the end of the expression.
  while (IsTokenABinaryOpWithPrecedenceAtLeast(lex_.GetCurToken(),
                                               min_precedence)) {

    assert(IsTokenABinaryOp(lex_.GetCurToken()) && "Expected binary operator");
    Token binary_op = lex_.ConsumeCurToken();

    auto rhs = parsePrimaryExpr();
    if (!rhs) {
      diagnose(diag::expected_primary_expression_after_binary_op);
      return nullptr;
    }

    // Recurse for binary operators with higher precedence.
    int op_precedence = GetBinaryOpPrecedence(binary_op);
    while (IsTokenABinaryOpWithPrecedenceGreaterThan(lex_.GetCurToken(),
                                                     op_precedence)
           || IsTokenRightAssociativeWithPrecedence(lex_.GetCurToken(),
                                                    op_precedence)) {
      Token lookahead_op = lex_.GetCurToken();
      rhs = parseBinaryOpExprRHS(move(rhs),
                                 GetBinaryOpPrecedence(lookahead_op));
      if (!rhs) {
        // The innermost expression parsing method should show a diagnostic.
        return nullptr;
      }
    }

    assert(lhs && rhs);
    lhs = make_unique<BinaryOpExpr>(binary_op, move(lhs), move(rhs));
  }
  return lhs;
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

  if (IsPrefixUnaryOpToken(lex_.GetCurToken())) {
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
  if (!IsPrefixUnaryOpToken(lex_.GetCurToken())) {
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
