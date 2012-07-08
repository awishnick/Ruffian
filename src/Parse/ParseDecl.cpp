#include "Parse/Parser.h"
#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "Lex/Lexer.h"
#include "Support/make_unique.h"
#include "llvm/Support/ErrorHandling.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>
using namespace std;
using llvm::StringRef;

unique_ptr<Decl> Parser::parseDecl() {
  if (lex_.GetCurToken().GetKind() == Token::kw_func) {
    return parseFunctionDecl();
  }

  if (lex_.GetCurToken().GetKind() == Token::kw_var) {
    return parseVariableDecl();
  }

  diagnose(diag::expected_decl);
  return nullptr;
}

unique_ptr<FunctionDecl> Parser::parseFunctionDecl() {
  /* function_decl
      : func identifier ( function_decl_arg_list ) -> identifier { stmt }
      | func identifier ( ) -> identifier { stmt }
      | func identifier ( function_decl_arg_list ) { stmt }
      | func identifier ( ) { stmt }
   */

  if (!expectAndConsume(Token::kw_func)) {
    llvm_unreachable("This should only be called with the 'func' keyword");
  }

  // Get the name
  Token name_tok;
  if (!expectAndConsumeIdentifier(&name_tok)) {
    diagnose(diag::expected_identifier_after_func);
    return nullptr;
  }

  // Expect (
  if (!expectAndConsume(Token::lparen)) {
    // TODO: recover by assuming no args and searching for the
    // arrow, return type, or left bracket.
    diagnose(diag::expected_lparen_to_begin_function_args);
    return nullptr;
  }

  // Parse the argument list if the next token is not an rparen.
  function_decl_arg_list args;
  if (lex_.GetCurToken().GetKind() != Token::rparen
      && !parseFunctionDeclArgList(&args)) {
    diagnose(diag::could_not_parse_function_argument_declaration);
    // Recover by pretending there are no arguments, and finding the
    // matching rparen.
    args.clear();
    if (!ignoreTokensUntil(Token::rparen)) {
      diagnose(diag::could_not_recover_by_finding_rparen);
      return nullptr;
    }
  }

  // Expect )
  if (!expectAndConsume(Token::rparen)) {
    diagnose(diag::expected_rparen_to_end_function_args);
    // Recover by consuming up to the arrow
    if (!ignoreTokensUntil(Token::arrow)) {
      return nullptr;
    }
  }


  // Now build the decls for each argument.
  vector<unique_ptr<VariableDecl>> arg_decls;
  arg_decls.reserve(args.size());
  transform(args.begin(), args.end(),
            back_inserter(arg_decls),
            [](const function_decl_arg_pair& arg_pair) {
              return make_unique<VariableDecl>(arg_pair.first,
                                               arg_pair.second);
            });

  // If there's an arrow, parse the return type.
  unique_ptr<Token> maybe_return_type;
  if (lex_.GetCurToken().GetKind() == Token::arrow) {
    lex_.ConsumeCurToken();

    // Get the return type
    Token ret_type_tok;
    if (!expectAndConsumeIdentifier(&ret_type_tok)) {
      // TODO: recovery
      diagnose(diag::expected_function_return_type);
      return nullptr;
    }
    maybe_return_type.reset(new Token(ret_type_tok));
  }

  // Expect {, but don't consume it, since it's up to the
  // function body parser to do that.
  if (lex_.GetCurToken().GetKind() != Token::lbrace) {
    diagnose(diag::expected_lbrace_for_function_body);
    // For now, no recovery. We could recover by
    // pretending we saw the brace.
    return nullptr;
  }

  // Parse the body
  auto body = parseBlockStmt();
  if (!body) {
    diagnose(diag::error_parsing_function_body);
  }

  if (maybe_return_type) {
    return make_unique<FunctionDecl>(name_tok,
                                     *maybe_return_type,
                                     move(arg_decls),
                                     move(body));
  } else {
    return make_unique<FunctionDecl>(name_tok,
                                     move(arg_decls),
                                     move(body));
  }
  
}

bool Parser::parseFunctionDeclArgList(function_decl_arg_list* args) {
  /* function_decl_arg_list
      : function_decl_arg_pair , function_decl_arg_list
     function_decl_arg_pair
      : identifier identifier
   */

  // Read each function_decl_arg_pair
  for (;;) {
    function_decl_arg_pair arg;
    if (!parseFunctionArgPair(&arg)) {
      // TODO: error
      // TODO: recovery? Could emit an error but not return failure.
      return false;
    }
    args->push_back(move(arg));

    // If we've got a comma, eat it. Otherwise, we're done.
    if (lex_.GetCurToken().GetKind() == Token::comma) {
      lex_.ConsumeCurToken();
    } else {
      break;
    }
  }

  return true;
}

bool Parser::parseFunctionArgPair(function_decl_arg_pair* arg) {
  Token type;
  if (!expectAndConsumeIdentifier(&type)) {
    diagnose(diag::expected_type_ident_in_func_arg_list);
    return false;
  }
  Token name;
  if (!expectAndConsumeIdentifier(&name)) {
    diagnose(diag::expected_name_ident_in_func_arg_list);
    return false;
  }

  *arg = make_pair(type, name);
  return true;
}

unique_ptr<VariableDecl> Parser::parseVariableDecl() {
  /* variable_decl
      : var identifier identifier ;
      : var identifier identifier = expression ;
  */

  if (!expectAndConsume(Token::kw_var)) {
    llvm_unreachable("This should only be called with the 'var' keyword");
  }

  // Get the name
  Token name_tok;
  if (!expectAndConsumeIdentifier(&name_tok)) {
    diagnose(diag::expected_identifier_after_var);
    return nullptr;
  }

  // Get the type
  Token type_tok;
  if (!expectAndConsumeIdentifier(&type_tok)) {
    diagnose(diag::expected_type_in_var_decl);
    return nullptr;
  }

  // If the next token is '=', then consume it and parse the
  // initializer expression.
  unique_ptr<Expr> initializer;
  if (lex_.GetCurToken().GetKind() == Token::assign) {
    lex_.ConsumeCurToken();
    initializer = parseExpr();

    // If there was an error parsing the initializer, recover
    // by finding the semicolon.
    if (!initializer) {
      ignoreTokensUntil(Token::semicolon);
    }
  }

  // Expect a semicolon. Recover by pretending it was found.
  if (!expectAndConsume(Token::semicolon)) {
    diagnose(diag::expected_semicolon_after_var_decl);
  }

  return make_unique<VariableDecl>(name_tok, type_tok, move(initializer));
}

