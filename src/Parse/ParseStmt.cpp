#include "Parse/Parser.h"
#include "AST/Decl.h"
#include "AST/Stmt.h"
#include "Lex/Lexer.h"
#include "Support/make_unique.h"
#include "llvm/Support/ErrorHandling.h"
#include <memory>
#include <vector>
using namespace std;

std::unique_ptr<Stmt> Parser::parseStmt() {
  /* statement
      : block_statement
      | declaration
  */

  if (lex_.GetCurToken().GetKind() == Token::lbrace) {
    return parseBlockStmt();
  }

  return parseDecl();
}

std::unique_ptr<BlockStmt> Parser::parseBlockStmt() {
  /* block_statement
      : { }
      | { statement_list }
     statement_list
      : statement
      | statement statement_list
  */

  if (!expectAndConsume(Token::lbrace)) {
    llvm_unreachable("Expected a '{' at the beginning of a block statement.");
  }

  vector<unique_ptr<Stmt>> stmts;
  while (lex_.GetCurToken().GetKind() != Token::rbrace
         && lex_.GetCurToken().GetKind() != Token::eof) {
    auto stmt = parseStmt();
    if (!stmt) {
      // Recover by finding the matching brace
      ignoreTokensUntil(Token::rbrace);
      break;
    }

    assert(stmt);
    stmts.push_back(move(stmt));
  }

  // Match '}'
  if (!expectAndConsume(Token::rbrace)) {
    diagnose(diag::expected_rbrace_after_block_stmt);
    
    // The only way this should happen is if we get to the
    // end of file, so recover by continuing with what we have.
    assert(lex_.GetCurToken().GetKind() == Token::eof);
  }

  return make_unique<BlockStmt>(move(stmts));
}  
